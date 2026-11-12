import sys                     # lets us change where Python looks for files
from pathlib import Path       # helps build folder paths

# Tell Python to also look in server/HttpServer, where packet_guard.py lives
sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "HttpServer"))

import packet_guard                                   # the whole file (to read rejected_count)
from packet_guard import check_and_decode, format_mac # the two functions we're testing
from protocol import ContentType, Information, Message # Waseem's protocol, to build fake packets


def make_packet(**changes):
    """Build a real 105-byte sensor packet, like the simulator sends."""
    info = Information(            # the sensor readings part
        status=0,                  # node says "all fine"
        temperature=21.5,
        humidity=40.0,
        carbon_dioxide=600.0,
        natrual_gas=1.0,           # Waseem's spelling, on purpose
        carbon_monoxide=2.0,
        smoke=0.5,
    )
    for name, value in changes.items():   # let a test swap in a bad value
        setattr(info, name, value)        # e.g. smoke = "nan"
    message = Message(             # the 80-byte header part
        device_name="NODE-1",
        mac_address=0x020000000001,       # a MAC address written as one number
        time_stamp=1700000000.0,          # a fixed Unix time
        content_type=ContentType.INFORMATION,  # "this is a sensor reading"
        content=info,
    )
    return message.to_bytes()      # turn it into raw bytes


# 1. A normal packet should be accepted
def test_valid_packet_is_accepted():
    raw = make_packet()
    assert len(raw) == 105                    # right size
    result = check_and_decode(raw)
    assert result is not None                 # not rejected
    assert result["device_name"] == "NODE-1"
    assert result["smoke"] == 0.5


# 2. The MAC number becomes readable text
def test_mac_is_formatted_as_text():
    result = check_and_decode(make_packet())
    assert result["mac"] == "02:00:00:00:00:01"
    assert format_mac(0xAABBCCDDEE01) == "AA:BB:CC:DD:EE:01"


# 3. The misspelt field gets the correct name
def test_misspelt_field_is_renamed():
    result = check_and_decode(make_packet())
    assert "natural_gas" in result            # correct spelling is there
    assert "natrual_gas" not in result        # wrong spelling is gone


# 4. Too short to even have a header: rejected
def test_short_packet_is_rejected():
    assert check_and_decode(make_packet()[:50]) is None


# 5. Wrong size for a sensor reading: rejected
def test_wrong_size_is_rejected():
    assert check_and_decode(make_packet() + b"\x00") is None   # 106 bytes
    assert check_and_decode(make_packet()[:100]) is None       # 100 bytes


# 6. A message that isn't a sensor reading: rejected
def test_non_sensor_message_is_rejected():
    raw = Message(device_name="NODE-1", content_type=ContentType.NONE).to_bytes()
    assert check_and_decode(raw) is None


# 7. A reading that isn't a real number: rejected
def test_nan_reading_is_rejected():
    assert check_and_decode(make_packet(smoke=float("nan"))) is None


# 8. Every rejection is counted
def test_rejections_are_counted():
    before = packet_guard.rejected_count       # count before
    check_and_decode(b"hello")                 # obvious junk
    assert packet_guard.rejected_count == before + 1   # went up by one