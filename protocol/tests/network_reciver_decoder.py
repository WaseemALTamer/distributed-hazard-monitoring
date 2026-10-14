"""

    this file is for you to test other devices that are transmiting
    the encoded data using the protocal


"""








import sys
from pathlib import Path
from pprint import PrettyPrinter

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



import socket

HOST = "0.0.0.0"
PORT = 39394

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((HOST, PORT))

print(f"LISTENING ON {HOST}:{PORT}...")

pp = PrettyPrinter(indent=4, width=80, sort_dicts=False)

try:
    while True:
        data, addr = sock.recvfrom(65535)

        try:
            m = Message()
            m.decode(data)
            pp.pprint(m)

            print("====================================================================================")
        except Exception as e:
            print(e)

except KeyboardInterrupt:
    print("\nLISTENER STOPPED.")
finally:
    sock.close()