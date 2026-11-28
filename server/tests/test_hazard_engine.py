"""
WP3 Unit Tests: Hazard Engine (server/tests/test_hazard_engine.py)
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "HttpServer"))

import config
from hazard_engine import HazardEngine
from node_registry import _FallbackEventBus


SAMPLE_BLUEPRINT = {
    "rooms": {
        "A": {"name": "Lab 1", "x": 100, "y": 80},
        "B": {"name": "Corridor", "x": 250, "y": 80},
        "C": {"name": "Lobby", "x": 400, "y": 80},
        "EXIT1": {"name": "Main exit", "x": 550, "y": 80, "exit": True},
    },
    "connections": [
        ["A", "B", 5],
        ["B", "C", 5],
        ["C", "EXIT1", 8],
    ],
    "devices": {
        "02:00:00:00:00:01": "A",
        "02:00:00:00:00:02": "B",
        "02:00:00:00:00:03": "C",
    },
}


def make_reading(
    mac: str = "02:00:00:00:00:01",
    room: str = "A",
    status: int = config.DEVICE_STATUS_NORMAL,
    temperature: float = 21.0,
    humidity: float = 45.0,
    carbon_dioxide: float = 600.0,
    natural_gas: float = 0.2,
    smoke: float = 0.1,
    carbon_monoxide: float = 0.2,
    ts: float = 1700000000.0,
) -> dict:
    return {
        "mac": mac,
        "room": room,
        "ts": ts,
        "status": status,
        "temperature": temperature,
        "humidity": humidity,
        "carbon_dioxide": carbon_dioxide,
        "natural_gas": natural_gas,
        "smoke": smoke,
        "carbon_monoxide": carbon_monoxide,
    }


def test_all_readings_low_keeps_room_safe_and_publishes_nothing():
    """All readings low -> Room stays SAFE and no hazard.changed is published."""
    bus = _FallbackEventBus()
    engine = HazardEngine(bus=bus, blueprint=SAMPLE_BLUEPRINT)

    changes = []
    bus.subscribe("hazard.changed", changes.append)

    bus.publish("reading.updated", make_reading())

    assert engine.get_room_level("A") == "SAFE"
    assert changes == []


def test_smoke_20_with_co2_600_is_warning_not_danger():
    """Smoke 20 with CO2 600 -> WARNING, not DANGER (CO2 cross-check)."""
    bus = _FallbackEventBus()
    engine = HazardEngine(bus=bus, blueprint=SAMPLE_BLUEPRINT)

    changes = []
    bus.subscribe("hazard.changed", changes.append)

    bus.publish("reading.updated", make_reading(smoke=20.0, carbon_dioxide=600.0))

    assert engine.get_room_level("A") == "WARNING"
    assert len(changes) == 1
    assert changes[0]["room"] == "A"
    assert changes[0]["old_level"] == "SAFE"
    assert changes[0]["new_level"] == "WARNING"


def test_smoke_20_with_co2_1800_is_danger():
    """Smoke 20 with CO2 1800 -> DANGER."""
    bus = _FallbackEventBus()
    engine = HazardEngine(bus=bus, blueprint=SAMPLE_BLUEPRINT)

    changes = []
    bus.subscribe("hazard.changed", changes.append)

    bus.publish("reading.updated", make_reading(smoke=20.0, carbon_dioxide=1800.0))

    assert engine.get_room_level("A") == "DANGER"
    assert len(changes) == 1
    assert changes[0]["new_level"] == "DANGER"
    assert changes[0]["reason"] == "smoke and CO2 above threshold"


def test_natural_gas_20_is_danger():
    """Natural gas 20 -> DANGER."""
    bus = _FallbackEventBus()
    engine = HazardEngine(bus=bus, blueprint=SAMPLE_BLUEPRINT)

    changes = []
    bus.subscribe("hazard.changed", changes.append)

    bus.publish("reading.updated", make_reading(natural_gas=20.0))

    assert engine.get_room_level("A") == "DANGER"
    assert len(changes) == 1
    assert changes[0]["new_level"] == "DANGER"
    assert "natural gas" in changes[0]["reason"]


def test_carbon_monoxide_thresholds():
    """Carbon monoxide -> WARNING at 5, DANGER at 15."""
    bus = _FallbackEventBus()
    engine = HazardEngine(bus=bus, blueprint=SAMPLE_BLUEPRINT)

    changes = []
    bus.subscribe("hazard.changed", changes.append)

    bus.publish("reading.updated", make_reading(carbon_monoxide=6.0))
    assert engine.get_room_level("A") == "WARNING"

    bus.publish("reading.updated", make_reading(carbon_monoxide=18.0))
    assert engine.get_room_level("A") == "DANGER"
    assert len(changes) == 2
    assert "carbon monoxide" in changes[1]["reason"]


def test_temperature_60_everything_else_low_stays_safe():
    """Temperature 60, everything else low -> Still SAFE."""
    bus = _FallbackEventBus()
    engine = HazardEngine(bus=bus, blueprint=SAMPLE_BLUEPRINT)

    changes = []
    bus.subscribe("hazard.changed", changes.append)

    bus.publish("reading.updated", make_reading(temperature=60.0))

    assert engine.get_room_level("A") == "SAFE"
    assert changes == []


def test_two_readings_with_same_level_publish_only_one_hazard_changed():
    """Two readings that give the same level -> Only one hazard.changed is published."""
    bus = _FallbackEventBus()
    engine = HazardEngine(bus=bus, blueprint=SAMPLE_BLUEPRINT)

    changes = []
    bus.subscribe("hazard.changed", changes.append)

    bus.publish("reading.updated", make_reading(natural_gas=20.0))
    bus.publish("reading.updated", make_reading(natural_gas=25.0))

    assert engine.get_room_level("A") == "DANGER"
    assert len(changes) == 1


def test_danger_then_two_low_readings_stays_danger_until_third_low_reading():
    """DANGER, then two low readings -> Still DANGER; it drops only after the third low reading."""
    bus = _FallbackEventBus()
    engine = HazardEngine(bus=bus, blueprint=SAMPLE_BLUEPRINT)

    changes = []
    bus.subscribe("hazard.changed", changes.append)

    # Escalate immediately to DANGER
    bus.publish("reading.updated", make_reading(natural_gas=20.0))
    assert engine.get_room_level("A") == "DANGER"
    assert len(changes) == 1

    # 1st low reading -> still DANGER
    bus.publish("reading.updated", make_reading(natural_gas=0.1))
    assert engine.get_room_level("A") == "DANGER"
    assert len(changes) == 1

    # 2nd low reading -> still DANGER
    bus.publish("reading.updated", make_reading(natural_gas=0.1))
    assert engine.get_room_level("A") == "DANGER"
    assert len(changes) == 1

    # 3rd low reading in a row -> drops to SAFE
    bus.publish("reading.updated", make_reading(natural_gas=0.1))
    assert engine.get_room_level("A") == "SAFE"
    assert len(changes) == 2
    assert changes[1]["old_level"] == "DANGER"
    assert changes[1]["new_level"] == "SAFE"


def test_room_becomes_danger_sends_warning_command_to_neighbouring_rooms():
    """Room becomes DANGER -> command.send goes to the nodes in neighbouring rooms."""
    bus = _FallbackEventBus()
    engine = HazardEngine(bus=bus, blueprint=SAMPLE_BLUEPRINT)

    commands = []
    bus.subscribe("command.send", commands.append)

    bus.publish("reading.updated", make_reading(mac="02:00:00:00:00:01", room="A", natural_gas=20.0))

    assert engine.get_room_level("A") == "DANGER"
    assert len(commands) == 1
    assert commands[0] == {
        "mac": "02:00:00:00:00:02",
        "command": "SET_DEVICE_STATUS",
        "value": config.DEVICE_STATUS_WARNING,  # 16
    }


def test_neighbour_status_echo_does_not_cascade_danger_and_resets_on_safe():
    """
    Huzair's anti-cascade test:
    A goes DANGER -> B gets 16 (WARNING) -> B reports 16 back -> B stays on status 16 (WARNING),
    nothing else changes (no hazard.changed or command.send to C).
    When A drops back to SAFE after 3 low readings -> B gets NORMAL (2) and the note is cleared.
    """
    bus = _FallbackEventBus()
    engine = HazardEngine(bus=bus, blueprint=SAMPLE_BLUEPRINT)

    changes = []
    commands = []
    bus.subscribe("hazard.changed", changes.append)
    bus.subscribe("command.send", commands.append)

    # 1. Room A goes DANGER
    bus.publish(
        "reading.updated",
        make_reading(mac="02:00:00:00:00:01", room="A", natural_gas=20.0),
    )
    assert engine.get_room_level("A") == "DANGER"
    assert len(changes) == 1
    assert len(commands) == 1
    assert commands[0] == {
        "mac": "02:00:00:00:00:02",
        "command": "SET_DEVICE_STATUS",
        "value": 16,
    }
    assert engine.sent_statuses.get("02:00:00:00:00:02") == 16

    # 2. Node B reports status=16 back to the hub with normal sensor readings
    bus.publish(
        "reading.updated",
        make_reading(
            mac="02:00:00:00:00:02",
            room="B",
            status=16,
            natural_gas=0.1,
            smoke=0.1,
            carbon_monoxide=0.1,
        ),
    )

    # B's commanded status note stays WARNING (16), B does not escalate, nothing else changes
    assert engine.sent_statuses.get("02:00:00:00:00:02") == 16
    assert engine.get_room_level("B") != "DANGER"
    assert len(changes) == 1
    assert len(commands) == 1

    # 3. Room A gets 3 consecutive low readings and drops back to SAFE
    for _ in range(3):
        bus.publish(
            "reading.updated",
            make_reading(mac="02:00:00:00:00:01", room="A", natural_gas=0.1),
        )

    assert engine.get_room_level("A") == "SAFE"
    assert len(changes) == 2
    assert changes[1]["new_level"] == "SAFE"

    # Node B gets NORMAL (2) and its note in sent_statuses is cleared
    assert len(commands) == 2
    assert commands[1] == {
        "mac": "02:00:00:00:00:02",
        "command": "SET_DEVICE_STATUS",
        "value": config.DEVICE_STATUS_NORMAL,  # 2
    }
    assert "02:00:00:00:00:02" not in engine.sent_statuses


def test_esp_own_danger_status_counts_as_danger():
    """Rule 3: An ESP that sets its own status to DANGER (17) counts as DANGER."""
    bus = _FallbackEventBus()
    engine = HazardEngine(bus=bus, blueprint=SAMPLE_BLUEPRINT)

    changes = []
    commands = []
    bus.subscribe("hazard.changed", changes.append)
    bus.subscribe("command.send", commands.append)

    bus.publish(
        "reading.updated",
        make_reading(
            mac="02:00:00:00:00:01",
            room="A",
            status=config.DEVICE_STATUS_DANGER,
        ),
    )

    assert engine.get_room_level("A") == "DANGER"
    assert len(changes) == 1
    assert changes[0]["new_level"] == "DANGER"
    assert len(commands) == 1
    assert commands[0]["mac"] == "02:00:00:00:00:02"
    assert commands[0]["value"] == config.DEVICE_STATUS_WARNING
