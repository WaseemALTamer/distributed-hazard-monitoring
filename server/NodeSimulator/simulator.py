from __future__ import annotations


from pathlib import Path
import socket
import time
import sys

current_dir = Path(__file__).resolve().parent
sys.path.insert(0, str(current_dir))


from nodes import (VirtualNode,
                   NormalNode,
                   DangerousNode
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
        port: int = 5005,
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
            f"smoke={node.smoke:5.1f} | "
            f"bytes={len(payload)}"
        )
        return payload

    def transmit_all(self) -> None:
        for node in self.nodes:
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