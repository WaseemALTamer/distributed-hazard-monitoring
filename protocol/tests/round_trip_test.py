import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from protocol import (
    MessageBase,
    Message,
    Information,
    Discovery,
    Command,
    WifiConnect,
    SetDeviceName,
    SetDeviceStatus,
    ContentType,
    DeviceStatus,
)


def test_message_round_trip():
    m = Message()

    m.content_type = ContentType.COMMAND

    c = Command()
    c.content_type = ContentType.WIFI_CONNECT

    w = WifiConnect()
    w.ssid = "waseem"
    w.password = "waseem2005"

    c.content = w
    m.content = c

    encoded = m.to_bytes()

    decoded = Message()
    decoded.decode(encoded)


    assert decoded.content_type == ContentType.COMMAND

    assert isinstance(decoded.content, Command)
    assert decoded.content.content_type == ContentType.WIFI_CONNECT

    assert isinstance(decoded.content.content, WifiConnect)
    assert decoded.content.content.ssid == "waseem"
    assert decoded.content.content.password == "waseem2005"

    assert decoded.to_bytes() == encoded



if __name__ == "__main__":
    test_message_round_trip()
    print("passed test")