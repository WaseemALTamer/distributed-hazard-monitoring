"""
WP3: Hazard Engine (hazard_engine.py)

Subscribes to "reading.updated", evaluates room hazard levels (SAFE / WARNING / DANGER)
using the WP0 rules in config.py (including the smoke + CO2 cross-check, ESP status
check, multi-node room aggregation, and 3-reading de-escalation hysteresis), publishes
"hazard.changed" when a room's committed level changes, and publishes "command.send"
to warn or reset nodes in neighbouring rooms without causing a DANGER cascade.
"""

from __future__ import annotations

from pathlib import Path
from typing import Any

try:
    from server.HttpServer import config
    from server.HttpServer.node_registry import _get_default_bus, load_blueprint
except ImportError:
    import config  # type: ignore
    from node_registry import _get_default_bus, load_blueprint  # type: ignore


class HazardEngine:
    """Evaluate sensor readings and manage per-room hazard state and neighbour alerts."""

    def __init__(
        self,
        bus: Any = None,
        blueprint: dict[str, Any] | str | Path | None = None,
    ) -> None:
        self.bus = bus if bus is not None else _get_default_bus()
        bp = load_blueprint(blueprint)

        # Map MAC -> Room from blueprint
        raw_devices = bp.get("devices", {}) or {}
        self._device_rooms: dict[str, str] = {
            str(mac).strip().upper(): str(room)
            for mac, room in raw_devices.items()
        }

        # Build undirected room adjacency map from blueprint["connections"]
        self._neighbours: dict[str, list[str]] = {}
        connections = bp.get("connections", []) or []
        if isinstance(connections, dict):
            for room_a, targets in connections.items():
                for room_b in targets:
                    self._add_edge(str(room_a), str(room_b))
        elif isinstance(connections, list):
            for edge in connections:
                if isinstance(edge, (list, tuple)) and len(edge) >= 2:
                    self._add_edge(str(edge[0]), str(edge[1]))

        # Committed hazard level per room (starts at SAFE)
        self.room_levels: dict[str, str] = {
            str(room_id): config.LEVEL_SAFE
            for room_id in (bp.get("rooms", {}) or {}).keys()
        }

        # Latest instantaneous (level, reason) for each node inside each room
        self._node_evaluations: dict[str, dict[str, tuple[str, str]]] = {}

        # Consecutive lower-reading streak per room for the 3-reading rule
        self._low_streaks: dict[str, list[tuple[str, str]]] = {}

        # Status value last sent by the hub to each node MAC via command.send
        self.sent_statuses: dict[str, int] = {}

        # Rooms currently in DANGER from their own readings or their own ESP-set status
        self._active_danger_rooms: set[str] = set()

        self.bus.subscribe("reading.updated", self.on_reading_updated)

    def _add_edge(self, room_a: str, room_b: str) -> None:
        if room_a == room_b:
            return
        self._neighbours.setdefault(room_a, [])
        if room_b not in self._neighbours[room_a]:
            self._neighbours[room_a].append(room_b)

        self._neighbours.setdefault(room_b, [])
        if room_a not in self._neighbours[room_b]:
            self._neighbours[room_b].append(room_a)

    def get_room_level(self, room: str) -> str:
        """Return the current committed level for a room (defaults to SAFE)."""
        return self.room_levels.get(room, config.LEVEL_SAFE)

    def evaluate_reading(self, reading: dict[str, Any]) -> tuple[str, str]:
        """
        Compute the instantaneous hazard (level, reason) for a single reading
        according to the WP0 threshold rules and Rule 3 (ESP-reported status).
        Temperature is intentionally ignored.
        """
        mac = str(reading.get("mac", "")).strip().upper()
        smoke = float(reading.get("smoke", 0.0))
        co2 = float(reading.get("carbon_dioxide", 0.0))
        natural_gas = float(reading.get("natural_gas", 0.0))
        co = float(reading.get("carbon_monoxide", 0.0))
        status = reading.get("status")

        danger_reasons: list[str] = []
        warning_reasons: list[str] = []

        # 1. Smoke (with CO2 cross-check for DANGER)
        if smoke >= config.SMOKE_DANGER_THRESHOLD:
            if co2 >= config.SMOKE_DANGER_CO2_THRESHOLD:
                danger_reasons.append("smoke and CO2")
            else:
                warning_reasons.append("smoke")
        elif smoke >= config.SMOKE_WARNING_THRESHOLD:
            warning_reasons.append("smoke")

        # 2. Natural gas
        if natural_gas >= config.NATURAL_GAS_DANGER_THRESHOLD:
            danger_reasons.append("natural gas")
        elif natural_gas >= config.NATURAL_GAS_WARNING_THRESHOLD:
            warning_reasons.append("natural gas")

        # 3. Carbon monoxide
        if co >= config.CARBON_MONOXIDE_DANGER_THRESHOLD:
            danger_reasons.append("carbon monoxide")
        elif co >= config.CARBON_MONOXIDE_WARNING_THRESHOLD:
            warning_reasons.append("carbon monoxide")

        # 4. Carbon dioxide (standalone check if not already covered by smoke+CO2 danger)
        if co2 >= config.CO2_DANGER_THRESHOLD:
            if "smoke and CO2" not in danger_reasons:
                danger_reasons.append("CO2")
        elif co2 >= config.CO2_WARNING_THRESHOLD:
            if "smoke and CO2" not in danger_reasons:
                warning_reasons.append("CO2")

        # 5. Rule 3: ESP's own status says danger (ignore if it matches the status the hub sent it)
        if status is not None:
            status_int = int(status)
            hub_sent = self.sent_statuses.get(mac)
            if status_int != hub_sent and status_int in config.ESP_DANGER_STATUSES:
                danger_reasons.append("device status")

        if danger_reasons:
            return config.LEVEL_DANGER, self._format_reason(danger_reasons)

        if warning_reasons:
            return config.LEVEL_WARNING, self._format_reason(warning_reasons)

        return config.LEVEL_SAFE, "all readings below threshold"

    @staticmethod
    def _format_reason(causes: list[str]) -> str:
        if causes == ["device status"]:
            return "device reported danger status"
        joined = " and ".join(causes)
        return f"{joined} above threshold"

    def _get_room_macs(self, room: str) -> list[str]:
        """Return all known node MAC addresses belonging to a room."""
        macs: list[str] = []
        for mac, assigned_room in self._device_rooms.items():
            if assigned_room == room and mac not in macs:
                macs.append(mac)
        for mac in self._node_evaluations.get(room, {}).keys():
            if mac not in macs:
                macs.append(mac)
        return macs

    def _warn_neighbours(self, danger_room: str) -> None:
        """Rule 7: Send WARNING (16) to all nodes in rooms neighbouring danger_room."""
        for neighbour_room in self._neighbours.get(danger_room, []):
            for mac in self._get_room_macs(neighbour_room):
                self.sent_statuses[mac] = config.DEVICE_STATUS_WARNING
                self.bus.publish(
                    "command.send",
                    {
                        "mac": mac,
                        "command": config.COMMAND_SET_DEVICE_STATUS,
                        "value": config.DEVICE_STATUS_WARNING,
                    },
                )

    def _clear_neighbours(self, cleared_room: str) -> None:
        """
        When a previously dangerous room drops back to SAFE, send NORMAL (2) to its
        neighbours and clear the sent_statuses note (unless another neighbour is still DANGER).
        """
        for neighbour_room in self._neighbours.get(cleared_room, []):
            if self.get_room_level(neighbour_room) == config.LEVEL_DANGER:
                continue

            other_danger_adjacent = any(
                adj in self._active_danger_rooms
                for adj in self._neighbours.get(neighbour_room, [])
            )
            if other_danger_adjacent:
                continue

            for mac in self._get_room_macs(neighbour_room):
                self.sent_statuses.pop(mac, None)
                self.bus.publish(
                    "command.send",
                    {
                        "mac": mac,
                        "command": config.COMMAND_SET_DEVICE_STATUS,
                        "value": config.DEVICE_STATUS_NORMAL,
                    },
                )

    def on_reading_updated(self, reading: dict[str, Any]) -> None:
        """Process a reading.updated event and update room hazard levels."""
        if not isinstance(reading, dict):
            return

        room = str(reading.get("room", config.UNASSIGNED_ROOM))
        mac = str(reading.get("mac", "")).strip().upper()
        if not mac:
            return

        node_level, node_reason = self.evaluate_reading(reading)
        room_nodes = self._node_evaluations.setdefault(room, {})
        room_nodes[mac] = (node_level, node_reason)

        # Rule 4: A room's instantaneous target level is the highest of its nodes
        target_level = config.LEVEL_SAFE
        target_reason = "all readings below threshold"
        for candidate_level, candidate_reason in room_nodes.values():
            if config.LEVEL_ORDER[candidate_level] > config.LEVEL_ORDER[target_level]:
                target_level = candidate_level
                target_reason = candidate_reason

        current_level = self.room_levels.get(room, config.LEVEL_SAFE)
        current_rank = config.LEVEL_ORDER[current_level]
        target_rank = config.LEVEL_ORDER[target_level]

        # Going UP: happens straight away
        if target_rank > current_rank:
            self._low_streaks[room] = []
            self._transition_room(room, current_level, target_level, target_reason)
            return

        # Staying at the SAME level: resets any partial downward streak, publishes nothing
        if target_rank == current_rank:
            self._low_streaks[room] = []
            return

        # Going DOWN: Rule 5 requires 3 consecutive readings below the current level
        streak = self._low_streaks.setdefault(room, [])
        streak.append((target_level, target_reason))

        if len(streak) >= config.CLEAR_READINGS_REQUIRED:
            # Drop to the highest level observed across the 3 consecutive lower readings
            new_level = config.LEVEL_SAFE
            new_reason = "all readings below threshold"
            for streak_level, streak_reason in streak:
                if config.LEVEL_ORDER[streak_level] >= config.LEVEL_ORDER[new_level]:
                    new_level = streak_level
                    new_reason = streak_reason

            self._low_streaks[room] = []
            self._transition_room(room, current_level, new_level, new_reason)

    def _transition_room(
        self,
        room: str,
        old_level: str,
        new_level: str,
        reason: str,
    ) -> None:
        if old_level == new_level:
            return

        self.room_levels[room] = new_level

        self.bus.publish(
            "hazard.changed",
            {
                "room": room,
                "old_level": old_level,
                "new_level": new_level,
                "reason": reason,
            },
        )

        if new_level == config.LEVEL_DANGER:
            self._active_danger_rooms.add(room)
            self._warn_neighbours(room)
        elif new_level == config.LEVEL_SAFE and room in self._active_danger_rooms:
            self._active_danger_rooms.discard(room)
            self._clear_neighbours(room)
