#!/usr/bin/env bash
# WP1: Raspberry Pi 3 B+ Automated Setup Script for HazardHub
# Usage:
#   HOTSPOT_PASSWORD='<team-password>' bash server/setup/install.sh
# (If HOTSPOT_PASSWORD is not set in the environment, the script will prompt for it.)

set -euo pipefail

SSID="${HOTSPOT_SSID:-HazardHub}"
WIFI_IFACE="${WIFI_IFACE:-wlan0}"
HUB_PORT="${HUB_PORT:-8000}"
HUB_USER="${SUDO_USER:-$(whoami)}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
VENV_DIR="${REPO_DIR}/.venv"
SERVICE_SRC="${SCRIPT_DIR}/hub.service"
SERVICE_DST="/etc/systemd/system/hub.service"

echo "=== [1/5] Updating system packages and installing dependencies ==="
sudo apt update
sudo apt full-upgrade -y
sudo apt install -y git python3-venv python3-pip network-manager

echo "=== [2/5] Creating Python virtual environment and installing requirements ==="
if [ ! -d "${VENV_DIR}" ]; then
    python3 -m venv "${VENV_DIR}"
fi

"${VENV_DIR}/bin/pip" install --upgrade pip

if [ -s "${REPO_DIR}/server/requirements.txt" ]; then
    "${VENV_DIR}/bin/pip" install -r "${REPO_DIR}/server/requirements.txt"
else
    echo "Notice: server/requirements.txt is missing or empty; installing default FastAPI/Uvicorn stack..."
    "${VENV_DIR}/bin/pip" install fastapi "uvicorn[standard]" pydantic
fi

echo "=== [3/5] Installing and enabling systemd service (hub.service) ==="
TMP_SERVICE="$(mktemp)"
sed \
    -e "s|__HUB_USER__|${HUB_USER}|g" \
    -e "s|__REPO_DIR__|${REPO_DIR}|g" \
    "${SERVICE_SRC}" > "${TMP_SERVICE}"

sudo cp "${TMP_SERVICE}" "${SERVICE_DST}"
rm -f "${TMP_SERVICE}"

sudo systemctl daemon-reload
sudo systemctl enable --now hub

echo "=== [4/5] Configuring 2.4 GHz Wi-Fi Hotspot (${SSID}) ==="
# Keep the hotspot password out of Git; read from env or prompt interactively.
if [ -z "${HOTSPOT_PASSWORD:-}" ]; then
    read -r -s -p "Enter Wi-Fi hotspot password for '${SSID}' (min 8 chars): " HOTSPOT_PASSWORD
    echo ""
fi

if [ "${#HOTSPOT_PASSWORD}" -lt 8 ]; then
    echo "Error: Wi-Fi hotspot password must be at least 8 characters." >&2
    exit 1
fi

echo "Warning: Starting the hotspot on ${WIFI_IFACE} will disconnect the Pi from upstream Wi-Fi."
echo "Plug in Ethernet if you still need internet access."

sudo systemctl enable --now NetworkManager

# Remove any existing Hotspot profile to keep the script idempotent on rebuilds
if sudo nmcli connection show "Hotspot" >/dev/null 2>&1; then
    sudo nmcli connection delete "Hotspot"
fi

sudo nmcli device wifi hotspot ifname "${WIFI_IFACE}" ssid "${SSID}" password "${HOTSPOT_PASSWORD}"
sudo nmcli connection modify Hotspot connection.autoconnect yes 802-11-wireless.band bg
sudo nmcli connection up Hotspot

echo "=== [5/5] Verifying setup ==="
ip addr show "${WIFI_IFACE}"
sudo systemctl status hub --no-pager || true

echo ""
echo "Setup complete! Connect a phone or laptop to '${SSID}' and open:"
echo "  http://10.42.0.1:${HUB_PORT}"
