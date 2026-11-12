import logging           # for writing warning messages
import math              # for checking numbers are real numbers
import sys               # lets us change where Python looks for files
from pathlib import Path # helps build folder paths

# Tell Python to also look in the repo's protocol folder (Waseem's library)
# parents[2] = go up from HttpServer -> server -> repo root
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "protocol"))

from protocol import ContentType, Information, Message  # Waseem's message classes

logger = logging.getLogger(__name__)  # logger named "packet_guard"

# Work out sizes from the protocol itself, so they stay right if it changes
HEADER_SIZE = sum(size for _, size in Message.__fields__.values())           # 80 bytes
INFORMATION_SIZE = sum(size for _, size in Information.__fields__.values())  # 25 bytes
SENSOR_PACKET_SIZE = HEADER_SIZE + INFORMATION_SIZE                           # 105 bytes

# The sensor readings inside each Information message 
READING_FIELDS = ["temperature", "humidity", "carbon_dioxide",
                  "natrual_gas", "carbon_monoxide", "smoke"]

# Fix the protocol's spelling before anyone else sees it
RENAMED = {"natrual_gas": "natural_gas"}

rejected_count = 0  # how many bad packets we've thrown away (useful for testing)


def format_mac(mac):
    """Turn a MAC number into text like '02:00:00:00:00:01' (the blueprint uses this)."""
    return ":".join(f"{(mac >> shift) & 0xFF:02X}" for shift in range(40, -1, -8))


def _reject(reason, raw):
    """Count a bad packet, log why, and return None (meaning 'rejected')."""
    global rejected_count                  # we're changing the counter defined above
    rejected_count += 1                    # add one to the count
    logger.warning("Rejected packet (%d bytes): %s", len(raw), reason)  # log the reason
    return None                            # None = "this packet was rejected"


def check_and_decode(raw):
    """Check a raw UDP packet and turn it into a clean dictionary, or None if it's bad."""

    # CHECK 1: is it at least as long as the header?
    if len(raw) < HEADER_SIZE:
        return _reject("shorter than the 80-byte header", raw)

    # Try to decode it with Waseem's library; if that crashes, the packet is bad
    try:
        message = Message().decode(raw)
    except Exception as error:
        return _reject(f"could not decode: {error}", raw)

    # CHECK 2: is it a sensor reading? (the only kind the hub handles for now)
    if message.content_type != ContentType.INFORMATION:
        return _reject(f"content type {message.content_type} is not a sensor reading", raw)

    # CHECK 3: is it exactly the right size for a sensor reading?
    if len(raw) != SENSOR_PACKET_SIZE:
        return _reject(f"sensor packets must be {SENSOR_PACKET_SIZE} bytes", raw)

    info = message.content  # the sensor part of the message

    # Start building the clean dictionary with the header details
    result = {
        "device_name": message.device_name,     # e.g. "NODE-1"
        "mac": format_mac(message.mac_address), # e.g. "02:00:00:00:00:01"
        "ts": message.time_stamp,               # when the reading was taken (Unix time)
        "status": info.status,                  # the node's own status number
    }

    # CHECK 4: every reading must be a real number (not "nan" or infinity)
    for field in READING_FIELDS:
        value = getattr(info, field)            # get e.g. info.smoke
        if not math.isfinite(value):
            return _reject(f"{field} is not a real number", raw)
        result[RENAMED.get(field, field)] = value  # store it, fixing the spelling if needed

    return result  # a clean, checked dictionary