# WP1: Raspberry Pi Platform Setup (`HazardHub`)

This folder contains the service definition and automated installer to turn a Raspberry Pi 3 B+ into the standalone **HazardHub** access point and web server (`http://10.42.0.1:8000`).

> **Security Note:** Never commit the Wi-Fi hotspot password to Git. `install.sh` reads `HOTSPOT_PASSWORD` from the environment or prompts for it interactively.

---

## 1. Hardware Checklist
- Raspberry Pi 3 B+ with fan fitted
- SanDisk 32 GB microSD card
- 5V / 2.5A micro-USB power supply

---

## 2. Flashing the SD Card
1. Open **Raspberry Pi Imager** on your laptop.
2. Select **Raspberry Pi OS Lite (64-bit)**.
3. Open OS Customisation settings and configure:
   - **Hostname:** `hazardhub`
   - **Username & Password:** set your Pi user credentials
   - **Wireless LAN:** enter your home Wi-Fi or phone hotspot credentials (used only for the initial package download and `git clone`)
   - **Services:** enable **SSH** (password authentication)
4. Flash the card, insert it into the Pi, and power on.

---

## 3. First Boot & Installation
1. SSH into the Pi from your laptop:
   ```bash
   ssh <username>@hazardhub.local
   ```
2. Install `git` and clone the repository:
   ```bash
   sudo apt update && sudo apt install -y git
   git clone https://github.com/WaseemALTamer/distributed-hazard-monitoring.git
   cd distributed-hazard-monitoring
   ```
3. Run the automated setup script (do this last, as switching `wlan0` into hotspot mode disconnects upstream Wi-Fi; use Ethernet if you still need internet access):
   ```bash
   bash server/setup/install.sh
   ```
   *(Enter the team's hotspot password when prompted, or pass `HOTSPOT_PASSWORD='<password>'`.)*

---

## 4. What `install.sh` Does
1. Updates Raspberry Pi OS (`apt update && apt full-upgrade -y`) and installs `git`, `python3-venv`, `python3-pip`, and `network-manager`.
2. Creates `.venv` in the repository root and installs dependencies from `server/requirements.txt`.
3. Populates `__HUB_USER__` and `__REPO_DIR__` in `server/setup/hub.service`, copies it to `/etc/systemd/system/hub.service`, and enables it on boot (`sudo systemctl enable --now hub`) with `Restart=always` on port `8000`.
4. Creates the `HazardHub` Wi-Fi hotspot on `wlan0` forced to the **2.4 GHz band (`802-11-wireless.band bg`)** so ESP32 nodes can join, and sets `connection.autoconnect yes`.

---

## 5. Verification & Physical Pi Test Log (To Complete on Hardware)

Once the physical Raspberry Pi board is available, run the following checks and record the results below for the report:

- **Verify Hotspot IP (`10.42.0.1`):**
  ```bash
  ip addr show wlan0
  ```
- **Verify Service Status:**
  ```bash
  sudo systemctl status hub
  ```
- **Verify Dashboard Access:**
  Join `HazardHub` from a phone or laptop and open `http://10.42.0.1:8000`.
- **Verify Thermal Performance (after 30 minutes with fan fitted):**
  ```bash
  vcgencmd measure_temp
  ```
- **Verify Cold-Boot Clock Behaviour (no RTC battery on Pi):**
  Unplug power, wait 1 minute, power back on without internet, and run:
  ```bash
  timedatectl
  ```
  *(Report the post-reboot clock drift/epoch behaviour to Huzair.)*

### Test Log Template

| Test | Date | Result / Notes |
| :--- | :--- | :--- |
| Cold boot test (hotspot + page live within 2 min) | YYYY-MM-DD | *Pending physical Pi* |
| Clean SD re-flash & rebuild from this guide | YYYY-MM-DD | *Pending physical Pi* |
| 30-minute temperature check (`vcgencmd measure_temp`) | YYYY-MM-DD | *Pending physical Pi* |
| Cold-boot `timedatectl` clock check | YYYY-MM-DD | *Pending physical Pi* |
