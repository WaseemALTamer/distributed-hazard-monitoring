from __future__ import annotations

from pathlib import Path
import socket
import sys
import threading
import time

current_dir = Path(__file__).resolve().parent
sys.path.insert(0, str(current_dir))

from nodes import (
    DangerousNode,
    NormalNode,
    VirtualNode,
)
from protocol import (
    ContentType,
    DeviceStatus,
    Information,
    Message,
)


class Simulator:
    """Create virtual nodes and broadcast their messages over UDP."""

    def __init__(
        self,
        normal_nodes: int = 3,
        dangerous_nodes: int = 1,
        interval: float = 10.0,
        broadcast_ip: str = "255.255.255.255",
        port: int = 39394,
        bind_interface: str = "0.0.0.0",
    ) -> None:
        if normal_nodes < 0 or dangerous_nodes < 0:
            raise ValueError("Node counts cannot be negative.")
        if interval <= 0:
            raise ValueError("interval must be greater than zero.")
        if not 1 <= port <= 65535:
            raise ValueError("port must be between 1 and 65535.")

        self.interval = interval
        self.destination = (broadcast_ip, port)
        self.nodes: list[VirtualNode] = []
        self._lock = threading.Lock()
        used_macs: set[int] = set()

        for i in range(1, normal_nodes + 1):
            mac = VirtualNode.generate_mac_address(used_macs)
            self.nodes.append(
                NormalNode(
                    device_name=f"NODE-{i}",
                    mac_address=mac,
                )
            )

        for i in range(1, dangerous_nodes + 1):
            mac = VirtualNode.generate_mac_address(used_macs)
            self.nodes.append(
                DangerousNode(
                    device_name=f"DANGER-NODE-{i}",
                    mac_address=mac,
                )
            )

        # One socket is shared by every virtual node.
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.socket.bind((bind_interface, 0))

    def set_node_state(
        self,
        node_number: int,
        dangerous: bool,
        *,
        transmit: bool = False,
    ) -> VirtualNode:
        """
        Switch a 1-based node index between NormalNode and DangerousNode while
        preserving its device_name and mac_address.
        """
        with self._lock:
            if not 1 <= node_number <= len(self.nodes):
                raise IndexError(
                    f"Node number {node_number} is out of range (1..{len(self.nodes)})."
                )

            current = self.nodes[node_number - 1]
            node_cls = DangerousNode if dangerous else NormalNode
            updated = node_cls(
                device_name=current.device_name,
                mac_address=current.mac_address,
            )
            updated.update_readings()
            self.nodes[node_number - 1] = updated

        mode_label = "DANGER" if dangerous else "NORMAL"
        print(
            f"[Simulator] Switched node {node_number} ({updated.device_name}, "
            f"MAC {updated.format_mac(updated.mac_address)}) -> {mode_label}"
        )

        if transmit:
            self.transmit_node(updated)

        return updated

    def handle_command(self, command_line: str, *, transmit: bool = False) -> bool:
        """
        Parse and execute an interactive terminal command such as:
          danger 2
          normal 2
          status / list
        Returns True if the command was recognised and executed, False otherwise.
        """
        parts = command_line.strip().split()
        if not parts:
            return False

        action = parts[0].lower()

        if action in ("list", "status"):
            with self._lock:
                for idx, node in enumerate(self.nodes, start=1):
                    mode = "DANGER" if isinstance(node, DangerousNode) else "NORMAL"
                    print(
                        f"  [{idx}] {node.device_name:<14} | "
                        f"MAC {node.format_mac(node.mac_address)} | mode={mode}"
                    )
            return True

        if action in ("danger", "normal") and len(parts) == 2:
            try:
                node_number = int(parts[1])
                self.set_node_state(
                    node_number,
                    dangerous=(action == "danger"),
                    transmit=transmit,
                )
                return True
            except (ValueError, IndexError) as exc:
                print(f"[Simulator] Invalid command '{command_line.strip()}': {exc}")
                return False

        print(
            f"[Simulator] Unknown command '{command_line.strip()}'. "
            "Use 'danger <n>', 'normal <n>', or 'status'."
        )
        return False

    def _input_loop(self) -> None:
        """Background listener for terminal commands while the simulator is running."""
        while True:
            try:
                line = sys.stdin.readline()
            except Exception:
                break
            if not line:
                break
            self.handle_command(line, transmit=True)

    def transmit_node(self, node: VirtualNode) -> bytes:
        """Refresh a node, serialize its message, and broadcast it."""
        node.update_readings()
        message = node.to_message()
        payload = message.to_bytes()
        self.socket.sendto(payload, self.destination)
        print(
            f"Sent {node.device_name:>14} | "
            f"MAC {node.format_mac(node.mac_address)} | "
            f"status={node.status.name:<6} | "
            f"temp={node.temperature:5.1f} C | "
            f"humidity={node.humidity:5.1f}% | "
            f"CO2={node.carbon_dioxide:6.1f} | "
            f"gas={node.natrual_gas:5.1f} | "
            f"CO={node.carbon_monoxide:5.1f} | "
            f"smoke={node.smoke:5.1f} | "
            f"bytes={len(payload)}"
        )
        return payload

    def transmit_all(self) -> None:
        with self._lock:
            current_nodes = list(self.nodes)
        for node in current_nodes:
            self.transmit_node(node)

    def run(self, *, send_immediately: bool = True) -> None:
        """Broadcast all nodes repeatedly until interrupted."""
        if not self.nodes:
            print("No nodes configured; nothing to transmit.")
            return

        print(
            f"Broadcasting {len(self.nodes)} nodes to "
            f"{self.destination[0]}:{self.destination[1]} "
            f"every {self.interval:g} seconds. Press Ctrl+C to stop."
        )
        print(
            "Type 'danger <node_num>', 'normal <node_num>', or 'status' "
            "and press Enter to toggle nodes live."
        )

        input_thread = threading.Thread(target=self._input_loop, daemon=True)
        input_thread.start()

        try:
            while True:
                cycle_started = time.monotonic()
                self.transmit_all()
                if not send_immediately:
                    send_immediately = True

                # Keep cycles approximately interval seconds apart, accounting
                # for time spent sending every node.
                elapsed = time.monotonic() - cycle_started
                time.sleep(max(0.0, self.interval - elapsed))
        except KeyboardInterrupt:
            print("\nSimulator stopped.")
        finally:
            self.close()

    def close(self) -> None:
        self.socket.close()

    def __enter__(self) -> "Simulator":
        return self

    def __exit__(self, exc_type, exc, traceback) -> None:
        self.close()
