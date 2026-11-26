import sys                     # lets us change where Python looks for files
import asyncio                 # Python's tool for doing many things at once
from pathlib import Path       # helps build folder paths

# Tell Python to also look in this folder (server/HttpServer) for our other hub files
sys.path.insert(0, str(Path(__file__).resolve().parent))

from packet_guard import check_and_decode   # checks a packet: clean reading, or None if bad
from event_bus import bus                   # the hub's shared message channel


class UDPReceiver(asyncio.DatagramProtocol):
    """Receive UDP sensor packets, check them with the packet guard, and pass them on."""

    def __init__(self, on_reading):
        self.on_reading = on_reading        # the web server's function to call with each good reading

    def connection_made(self, transport):
        # Called once, when the port is opened
        self.transport = transport          # keep hold of the open port (needed later to send commands)
        print("UDP receiver started")

    def datagram_received(self, data, addr):
        # Called every time a packet arrives. addr = (sender's IP address, sender's port)
        reading = check_and_decode(data)    # run every packet guard check
        if reading is None:                 # bad packet? the guard has already logged why
            return                          # so just ignore it

        reading["ip"] = addr[0]             # note which IP address it came from (Hemish needs this)

        self.on_reading(reading)                  # 1. give it to the web server (live dashboard)
        bus.publish("packet.received", reading)   # 2. share it with the rest of the hub (hazard engine, storage)

    def error_received(self, error):
        print(f"UDP receiver error: {error}")    # network-level errors: just print them


async def start_udp_receiver(on_reading, host="0.0.0.0", port=39394):
    # Open the UDP port and start listening.
    # host "0.0.0.0" = listen on every network the computer is connected to.
    # port 39394 = the port the ESPs and the simulator send to (agreed in WP0).
    loop = asyncio.get_running_loop()                  # the asyncio "engine" that's already running
    transport, protocol = await loop.create_datagram_endpoint(
        lambda: UDPReceiver(on_reading),               # make a receiver for this port
        local_addr=(host, port)                        # which address and port to listen on
    )
    return transport, protocol                         # give back the open port and the receiver