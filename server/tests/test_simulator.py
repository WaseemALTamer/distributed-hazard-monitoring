"""
WP8 Unit Tests: Node Simulator Extensions (server/tests/test_simulator.py)
"""

import sys
from pathlib import Path

repo_root = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(repo_root / "protocol"))
sys.path.insert(0, str(repo_root / "server" / "NodeSimulator"))

from protocol import ContentType, DeviceStatus, Information, Message
from nodes import DangerousNode, NormalNode, VirtualNode
from simulator import Simulator


def test_simulated_packet_decodes_with_protocol_library_including_co():
    """Simulated packet -> Decodes correctly with Waseem's protocol library, including CO."""
    node = NormalNode(device_name="SIM-NODE-1", mac_address=0x020000000001)
    node.update_readings()
    node.carbon_monoxide = 1.25

    raw_bytes = node.to_message().to_bytes()
    assert len(raw_bytes) == 105

    decoded = Message().decode(raw_bytes)
    assert decoded.device_name == "SIM-NODE-1"
    assert decoded.mac_address == 0x020000000001
    assert decoded.content_type == ContentType.INFORMATION
    assert isinstance(decoded.content, Information)
    assert decoded.content.status == DeviceStatus.NORMAL
    assert abs(decoded.content.carbon_monoxide - 1.25) < 1e-4
    assert 0.0 <= decoded.content.natrual_gas <= 0.5
    assert 0.0 <= decoded.content.smoke <= 0.5


def test_dangerous_node_generates_danger_readings_including_co():
    """DangerousNode sets status to DANGER and generates elevated hazard readings."""
    node = DangerousNode(device_name="DANGER-1", mac_address=0x020000000002)

    for _ in range(25):
        node.update_readings()
        raw_bytes = node.to_message().to_bytes()
        decoded = Message().decode(raw_bytes)
        info = decoded.content

        assert info.status == DeviceStatus.DANGER
        is_hazard = (
            info.carbon_dioxide >= 2500.0
            or info.natrual_gas >= 15.0
            or info.carbon_monoxide >= 15.0
            or (info.smoke >= 15.0 and info.carbon_dioxide >= 1500.0)
        )
        assert is_hazard


def test_simulator_live_command_toggles_node_between_normal_and_danger():
    """Typing 'danger 2' or 'normal 2' switches the chosen node between normal and dangerous."""
    with Simulator(normal_nodes=3, dangerous_nodes=0, port=39394) as sim:
        assert isinstance(sim.nodes[1], NormalNode)
        original_name = sim.nodes[1].device_name
        original_mac = sim.nodes[1].mac_address

        # Switch node 2 to danger mode
        assert sim.handle_command("danger 2", transmit=False) is True
        assert isinstance(sim.nodes[1], DangerousNode)
        assert sim.nodes[1].device_name == original_name
        assert sim.nodes[1].mac_address == original_mac
        assert sim.nodes[1].status == DeviceStatus.DANGER

        # Switch node 2 back to normal mode
        assert sim.handle_command("normal 2", transmit=False) is True
        assert isinstance(sim.nodes[1], NormalNode)
        assert sim.nodes[1].device_name == original_name
        assert sim.nodes[1].mac_address == original_mac
        assert sim.nodes[1].status == DeviceStatus.NORMAL
