import asyncio
import socket
import pytest

from generated.protocol import Message, Information, ContentType
from server.HttpServer.udp_receiver import UDPReceiver, start_udp_receiver


def test_udp_receiver_decodes_sensor_reading():
    # Store readings received by the callback
    received_readings = []

    # Create the UDP receiver
    receiver = UDPReceiver(received_readings.append)

    # Create a simulated sensor message
    message = Message(
        device_name="TEST-NODE",
        mac_address=0xAABBCCDDEE01,
        content_type=ContentType.INFORMATION,
        content=Information(
            temperature=25.5,
            humidity=60.0,
            carbon_dioxide=450.0,
            status=2
        )
    )

    # Convert the message to binary data
    packet = message.to_bytes()

    # Pass the packet to the receiver
    receiver.datagram_received(packet, ("127.0.0.1", 39394))

    # Check that exactly one reading was received
    assert len(received_readings) == 1

    reading = received_readings[0]

    # Verify the decoded sensor information (WP0 names)
    assert reading["device_name"] == "TEST-NODE"        # was "device"
    assert reading["mac"] == "AA:BB:CC:DD:EE:01"        # was "mac_address", now with colons
    assert reading["temperature"] == 25.5
    assert reading["humidity"] == 60.0
    assert reading["carbon_dioxide"] == 450.0
    assert reading["status"] == 2


@pytest.mark.anyio
async def test_udp_receiver_socket():
    received_readings = []

    # Start a real UDP listener on a temporary local port
    transport, protocol = await start_udp_receiver(
        received_readings.append,
        host="127.0.0.1",
        port=0
    )

    try:
        # Find the port assigned by the operating system
        port = transport.get_extra_info("sockname")[1]

        # Create a simulated sensor packet
        message = Message(
            device_name="SOCKET-TEST",
            mac_address=0xAABBCCDDEE02,
            content_type=ContentType.INFORMATION,
            content=Information(
                temperature=27.5,
                humidity=55.0,
                status=2
            )
        )

        # Send the packet over a real UDP socket
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sender:
            sender.sendto(message.to_bytes(), ("127.0.0.1", port))

        # Give the receiver a short opportunity to process the packet
        for _ in range(50):
            if received_readings:
                break
            await asyncio.sleep(0.01)

        # Check the received sensor information
        assert len(received_readings) == 1
        assert received_readings[0]["device_name"] == "SOCKET-TEST"   # was "device"
        assert received_readings[0]["temperature"] == 27.5
        assert received_readings[0]["humidity"] == 55.0

    finally:
        # Always close the UDP listener
        transport.close()


# NEW: every good reading is also shared on the event bus, with the sender's IP
def test_reading_is_published_on_event_bus():
    from event_bus import bus                       # the same bus the receiver uses
    published = []                                  # collects what arrives on the bus
    bus.subscribe("packet.received", published.append)   # listen on the channel

    receiver = UDPReceiver(lambda reading: None)    # we don't care about the web server here
    message = Message(
        device_name="BUS-TEST",
        mac_address=0xAABBCCDDEE04,
        content_type=ContentType.INFORMATION,
        content=Information(smoke=1.5, status=2)
    )
    receiver.datagram_received(message.to_bytes(), ("10.42.0.23", 39394))   # pretend it came from 10.42.0.23

    assert len(published) == 1                      # exactly one reading went on the bus
    assert published[0]["device_name"] == "BUS-TEST"
    assert published[0]["ip"] == "10.42.0.23"       # the sender's IP was added
    assert published[0]["smoke"] == 1.5


# NEW: junk packets never reach the web server
def test_bad_packet_is_ignored():
    received_readings = []
    receiver = UDPReceiver(received_readings.append)
    receiver.datagram_received(b"junk", ("127.0.0.1", 39394))   # not a real packet
    assert received_readings == []                  # nothing was passed on