"""
Central configuration and hazard thresholds for the HazardHub server (WP0 / WP3).
All thresholds live here rather than being hardcoded in the logic modules.
"""

from pathlib import Path

# Network and file paths
UDP_PORT = 39394
HTTP_PORT = 8000
BLUEPRINT_PATH = Path(__file__).resolve().parents[2] / "config" / "blueprint.json"

# Node registry configuration
UNASSIGNED_ROOM = "UNASSIGNED"
OFFLINE_CHECK_INTERVAL_S = 5.0
NODE_OFFLINE_TIMEOUT_S = 30.0

# Room hazard levels
LEVEL_SAFE = "SAFE"
LEVEL_WARNING = "WARNING"
LEVEL_DANGER = "DANGER"

LEVEL_ORDER = {
    LEVEL_SAFE: 0,
    LEVEL_WARNING: 1,
    LEVEL_DANGER: 2,
}

# Sensor thresholds (WP0 placeholder values until hardware calibration)
SMOKE_WARNING_THRESHOLD = 5.0
SMOKE_DANGER_THRESHOLD = 15.0
SMOKE_DANGER_CO2_THRESHOLD = 1500.0

NATURAL_GAS_WARNING_THRESHOLD = 5.0
NATURAL_GAS_DANGER_THRESHOLD = 15.0

CARBON_MONOXIDE_WARNING_THRESHOLD = 5.0
CARBON_MONOXIDE_DANGER_THRESHOLD = 15.0

CO2_WARNING_THRESHOLD = 1500.0
CO2_DANGER_THRESHOLD = 2500.0

# Hysteresis: number of consecutive readings below threshold required to step down
CLEAR_READINGS_REQUIRED = 3

# Protocol DeviceStatus numbers (kept in config so hub modules stay decoupled from protocol.py)
DEVICE_STATUS_NORMAL = 2
DEVICE_STATUS_WARNING = 16
DEVICE_STATUS_DANGER = 17

# ESP-reported status numbers that indicate the node itself detected danger
ESP_DANGER_STATUSES = frozenset({12, 15, DEVICE_STATUS_DANGER})

COMMAND_SET_DEVICE_STATUS = "SET_DEVICE_STATUS"
