"""
WP3 Unit Tests: Node Registry (server/tests/test_node_registry.py)
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "HttpServer"))

import config
from node_registry import NodeRegistry, _FallbackEventBus


SAMPLE_BLUEPRINT = {
    "rooms": {
        "A": {"name": "Lab 1", "x": 100, "y": 80},
        "B": {"name": "Corridor", "x": 250, "y": 80},
    },
    "connections": [
        ["A", "B", 5],
    ],
    "devices": {
        "02:00:00:00:00:01": "A",
        "02:00:00:00:00:02": "B",
    },
}


def make_packet_event(
    mac: str = "02:00:00:00:00:01",
    ip: str = "10.42.0.23",
    ts: float = 1700000000.0,
    status: int = 2,
    temperature: float = 22.0,
    humidity: float = 45.0,
    carbon_dioxide: float = 600.0,
    natural_gas: float = 0.2,
    smoke: float = 0.1,
    carbon_monoxide: float = 0.3,
) -> dict:
    """Create a packet.received dictionary matching packet_guard + sender IP."""
    return {
        "device_name": "NODE-1",
        "mac": mac,
        "ip": ip,
        "ts": ts,
        "status": status,
        "temperature": temperature,
        "humidity": humidity,
        "carbon_dioxide": carbon_dioxide,
        "natural_gas": natural_gas,
        "smoke": smoke,
        "carbon_monoxide": carbon_monoxide,
    }


def test_packet_from_new_mac_creates_record_and_publishes_reading_updated():
    """Test 1: A packet from a new MAC creates a node record and publishes reading.updated."""
    bus = _FallbackEventBus()
    now = 1000.0
    registry = NodeRegistry(bus=bus, blueprint=SAMPLE_BLUEPRINT, clock=lambda: now)

    updated_events = []
    bus.subscribe("reading.updated", updated_events.append)

    bus.publish("packet.received", make_packet_event(mac="02:00:00:00:00:01", ip="10.42.0.23"))

    assert "02:00:00:00:00:01" in registry.nodes
    record = registry.nodes["02:00:00:00:00:01"]
    assert record["room"] == "A"
    assert record["ip"] == "10.42.0.23"
    assert record["last_seen"] == 1000.0
    assert record["online"] is True

    assert len(updated_events) == 1
    published = updated_events[0]
    expected_keys = {
        "mac",
        "room",
        "ts",
        "status",
        "temperature",
        "humidity",
        "carbon_dioxide",
        "natural_gas",
        "smoke",
        "carbon_monoxide",
    }
    assert set(published.keys()) == expected_keys
    assert published["mac"] == "02:00:00:00:00:01"
    assert published["room"] == "A"
    assert published["temperature"] == 22.0
    assert published["carbon_monoxide"] == 0.3


def test_mac_not_in_blueprint_is_marked_unassigned():
    """Test 2: A MAC not in the blueprint has its room set to UNASSIGNED."""
    bus = _FallbackEventBus()
    registry = NodeRegistry(bus=bus, blueprint=SAMPLE_BLUEPRINT, clock=lambda: 1000.0)

    updated_events = []
    bus.subscribe("reading.updated", updated_events.append)

    bus.publish("packet.received", make_packet_event(mac="FF:EE:DD:CC:BB:AA"))

    assert registry.nodes["FF:EE:DD:CC:BB:AA"]["room"] == config.UNASSIGNED_ROOM
    assert len(updated_events) == 1
    assert updated_events[0]["room"] == "UNASSIGNED"


def test_no_packet_for_30s_publishes_offline_once_and_new_packet_publishes_online():
    """Test 3: No packet for 30 s publishes node.offline once; a new packet publishes node.online."""
    bus = _FallbackEventBus()
    current_time = [1000.0]
    registry = NodeRegistry(
        bus=bus,
        blueprint=SAMPLE_BLUEPRINT,
        clock=lambda: current_time[0],
    )

    offline_events = []
    online_events = []
    bus.subscribe("node.offline", offline_events.append)
    bus.subscribe("node.online", online_events.append)

    # Initial packet at t = 1000s
    bus.publish("packet.received", make_packet_event(mac="02:00:00:00:00:01"))
    assert offline_events == []
    assert online_events == []

    # Check at t = 1025s (only 25s elapsed -> still online)
    current_time[0] = 1025.0
    registry.check_offline()
    assert offline_events == []

    # Check at t = 1030s (30s elapsed -> node.offline published once)
    current_time[0] = 1030.0
    registry.check_offline()
    assert len(offline_events) == 1
    assert offline_events[0] == {
        "mac": "02:00:00:00:00:01",
        "room": "A",
        "last_seen": 1000.0,
    }

    # Check again at t = 1035s -> must NOT publish node.offline a second time
    current_time[0] = 1035.0
    registry.check_offline()
    assert len(offline_events) == 1

    # Node comes back at t = 1040s -> publishes node.online
    current_time[0] = 1040.0
    bus.publish("packet.received", make_packet_event(mac="02:00:00:00:00:01"))
    assert len(online_events) == 1
    assert online_events[0]["mac"] == "02:00:00:00:00:01"
    assert online_events[0]["room"] == "A"
    assert registry.nodes["02:00:00:00:00:01"]["online"] is True
