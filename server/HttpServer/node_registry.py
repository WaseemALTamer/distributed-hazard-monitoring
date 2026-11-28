"""
WP3: Node Registry (node_registry.py)

Listens for validated sensor packets on "packet.received", maintains one record
per MAC address, maps MACs to rooms using the building blueprint, publishes
"reading.updated", and monitors node heartbeats every 5 s to publish
"node.offline" (after 30 s of silence) and "node.online" (when an offline node returns).
"""

from __future__ import annotations

import asyncio
import json
from pathlib import Path
import time
from typing import Any, Callable

try:
    from server.HttpServer import config
except ImportError:
    import config  # type: ignore


class _FallbackEventBus:
    """Minimal publish/subscribe bus used when event_bus.py is not yet merged."""

    def __init__(self) -> None:
        self._subscribers: dict[str, list[Callable[[Any], Any]]] = {}

    def subscribe(self, channel: str, handler: Callable[[Any], Any]) -> None:
        self._subscribers.setdefault(channel, []).append(handler)

    def publish(self, channel: str, data: Any = None) -> None:
        for handler in list(self._subscribers.get(channel, [])):
            handler(data)


def _get_default_bus() -> Any:
    try:
        from server.HttpServer.event_bus import bus
        return bus
    except ImportError:
        try:
            from event_bus import bus  # type: ignore
            return bus
        except ImportError:
            return _FallbackEventBus()


def load_blueprint(blueprint_source: dict[str, Any] | str | Path | None = None) -> dict[str, Any]:
    """Load a blueprint dictionary from a dict, file path, or the default config path."""
    if isinstance(blueprint_source, dict):
        return blueprint_source

    path = Path(blueprint_source) if blueprint_source is not None else config.BLUEPRINT_PATH
    if not path.exists():
        return {"rooms": {}, "connections": [], "devices": {}}

    with open(path, "r", encoding="utf-8") as file:
        return json.load(file)


class NodeRegistry:
    """Track active sensor nodes, map MACs to rooms, and detect offline/online transitions."""

    def __init__(
        self,
        bus: Any = None,
        blueprint: dict[str, Any] | str | Path | None = None,
        clock: Callable[[], float] = time.time,
        offline_timeout: float = config.NODE_OFFLINE_TIMEOUT_S,
        check_interval: float = config.OFFLINE_CHECK_INTERVAL_S,
    ) -> None:
        self.bus = bus if bus is not None else _get_default_bus()
        self._clock = clock
        self.offline_timeout = offline_timeout
        self.check_interval = check_interval

        bp = load_blueprint(blueprint)
        raw_devices = bp.get("devices", {}) or {}
        self._device_rooms: dict[str, str] = {
            str(mac).strip().upper(): str(room)
            for mac, room in raw_devices.items()
        }

        # One record per MAC address
        self.nodes: dict[str, dict[str, Any]] = {}
        self._monitor_task: asyncio.Task[None] | None = None

        self.bus.subscribe("packet.received", self.on_packet_received)

    def resolve_room(self, mac: str) -> str:
        """Look up a MAC's room in the blueprint, or return UNASSIGNED if unknown."""
        normalized_mac = str(mac).strip().upper()
        return self._device_rooms.get(normalized_mac, config.UNASSIGNED_ROOM)

    def on_packet_received(self, data: dict[str, Any]) -> None:
        """
        Handle a packet_guard dict from "packet.received":
        {"device_name", "mac", "ts", "status", "temperature", "humidity",
         "carbon_dioxide", "natural_gas", "carbon_monoxide", "smoke", "ip"}
        """
        if not isinstance(data, dict) or "mac" not in data:
            return

        now = self._clock()
        mac = str(data["mac"]).strip().upper()
        room = self.resolve_room(mac)

        previous_record = self.nodes.get(mac)
        was_offline = previous_record is not None and not previous_record.get("online", True)

        record = {
            "mac": mac,
            "device_name": data.get("device_name", ""),
            "room": room,
            "ip": data.get("ip", ""),
            "ts": float(data.get("ts", now)),
            "status": int(data.get("status", 0)),
            "temperature": float(data.get("temperature", 0.0)),
            "humidity": float(data.get("humidity", 0.0)),
            "carbon_dioxide": float(data.get("carbon_dioxide", 0.0)),
            "natural_gas": float(data.get("natural_gas", 0.0)),
            "smoke": float(data.get("smoke", 0.0)),
            "carbon_monoxide": float(data.get("carbon_monoxide", 0.0)),
            "last_seen": now,
            "online": True,
        }
        self.nodes[mac] = record

        if was_offline:
            self.bus.publish(
                "node.online",
                {
                    "mac": mac,
                    "room": room,
                    "last_seen": now,
                },
            )

        self.bus.publish(
            "reading.updated",
            {
                "mac": mac,
                "room": room,
                "ts": record["ts"],
                "status": record["status"],
                "temperature": record["temperature"],
                "humidity": record["humidity"],
                "carbon_dioxide": record["carbon_dioxide"],
                "natural_gas": record["natural_gas"],
                "smoke": record["smoke"],
                "carbon_monoxide": record["carbon_monoxide"],
            },
        )

    def check_offline(self, now: float | None = None) -> list[str]:
        """
        Check last-seen times for all tracked nodes.
        If a node has not been seen for at least offline_timeout (30 s) and is currently
        marked online, mark it offline and publish "node.offline" once.
        """
        current_time = self._clock() if now is None else now
        newly_offline: list[str] = []

        for mac, record in self.nodes.items():
            if not record.get("online", True):
                continue

            if current_time - record["last_seen"] >= self.offline_timeout:
                record["online"] = False
                newly_offline.append(mac)
                self.bus.publish(
                    "node.offline",
                    {
                        "mac": mac,
                        "room": record["room"],
                        "last_seen": record["last_seen"],
                    },
                )

        return newly_offline

    async def start_monitoring(self) -> None:
        """Run the periodic 5-second last-seen check in the background."""
        if self._monitor_task is not None and not self._monitor_task.done():
            return

        async def _loop() -> None:
            while True:
                await asyncio.sleep(self.check_interval)
                self.check_offline()

        self._monitor_task = asyncio.create_task(_loop())

    async def stop_monitoring(self) -> None:
        """Stop the background monitoring task."""
        if self._monitor_task is not None:
            self._monitor_task.cancel()
            try:
                await self._monitor_task
            except asyncio.CancelledError:
                pass
            self._monitor_task = None
