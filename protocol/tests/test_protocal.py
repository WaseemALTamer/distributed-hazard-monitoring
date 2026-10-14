import unittest
import time

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


class TestMessageEncoding(unittest.TestCase):

    def assert_round_trip(self, original, message_class=None):
        """Encode a message, decode it, and return the decoded object."""
        encoded = original.to_bytes()

        if message_class is None:
            message_class = type(original)

        decoded = message_class()
        decoded.decode(encoded)

        return encoded, decoded

    # --------------------------------------------------
    # Information
    # --------------------------------------------------

    def test_information_round_trip(self):
        original = Information(
            status=DeviceStatus.NORMAL,
            temperature=23.5,
            humidity=55.2,
            carbon_dioxide=650.0,
            natrual_gas=0.12,
            smoke=0.05,
        )

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), Information.__size__)
        self.assertEqual(decoded.status, DeviceStatus.NORMAL)
        self.assertAlmostEqual(decoded.temperature, 23.5)
        self.assertAlmostEqual(decoded.humidity, 55.2, places=5)
        self.assertAlmostEqual(decoded.carbon_dioxide, 650.0)
        self.assertAlmostEqual(decoded.natrual_gas, 0.12, places=5)
        self.assertAlmostEqual(decoded.smoke, 0.05, places=5)

    def test_information_zero_values(self):
        original = Information()

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), 21)
        self.assertEqual(decoded.status, DeviceStatus.NONE)
        self.assertEqual(decoded.temperature, 0.0)
        self.assertEqual(decoded.humidity, 0.0)
        self.assertEqual(decoded.carbon_dioxide, 0.0)
        self.assertEqual(decoded.natrual_gas, 0.0)
        self.assertEqual(decoded.smoke, 0.0)

    # --------------------------------------------------
    # Discovery
    # --------------------------------------------------

    def test_discovery_round_trip(self):
        original = Discovery()

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(encoded, b"")
        self.assertEqual(len(encoded), Discovery.__size__)
        self.assertIsInstance(decoded, Discovery)

    # --------------------------------------------------
    # Wi-Fi configuration
    # --------------------------------------------------

    def test_wifi_connect_round_trip(self):
        original = WifiConnect(
            ssid="TestNetwork",
            password="ExamplePassword123",
        )

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), WifiConnect.__size__)
        self.assertEqual(decoded.ssid, original.ssid)
        self.assertEqual(decoded.password, original.password)

    def test_wifi_connect_empty_credentials(self):
        original = WifiConnect()

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), 96)
        self.assertEqual(decoded.ssid, "")
        self.assertEqual(decoded.password, "")

    def test_wifi_connect_maximum_length_strings(self):
        original = WifiConnect(
            ssid="S" * 32,
            password="P" * 64,
        )

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), 96)
        self.assertEqual(decoded.ssid, "S" * 32)
        self.assertEqual(decoded.password, "P" * 64)

    def test_wifi_connect_string_too_long(self):
        original = WifiConnect(
            ssid="S" * 33,
            password="password",
        )

        with self.assertRaises(ValueError):
            original.to_bytes()

    # --------------------------------------------------
    # Set device name
    # --------------------------------------------------

    def test_set_device_name_round_trip(self):
        original = SetDeviceName(
            device_name="ESP32_LivingRoom"
        )

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), 64)
        self.assertEqual(decoded.device_name, "ESP32_LivingRoom")

    def test_set_device_name_maximum_length(self):
        original = SetDeviceName(device_name="A" * 64)

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), 64)
        self.assertEqual(decoded.device_name, "A" * 64)

    def test_set_device_name_too_long(self):
        original = SetDeviceName(device_name="A" * 65)

        with self.assertRaises(ValueError):
            original.to_bytes()

    # --------------------------------------------------
    # Set device status
    # --------------------------------------------------

    def test_set_device_status_normal(self):
        original = SetDeviceStatus(
            status=DeviceStatus.NORMAL
        )

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), 1)
        self.assertEqual(decoded.status, DeviceStatus.NORMAL)

    def test_set_device_status_danger(self):
        original = SetDeviceStatus(
            status=DeviceStatus.DANGER
        )

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(decoded.status, DeviceStatus.DANGER)

    # --------------------------------------------------
    # Command
    # --------------------------------------------------

    def test_command_without_payload(self):
        original = Command(
            sequence_number=12345,
            content_type=ContentType.NONE,
        )

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), 9)
        self.assertEqual(decoded.sequence_number, 12345)
        self.assertEqual(decoded.content_type, ContentType.NONE)
        self.assertIsNone(decoded.content)

    def test_command_with_set_device_name(self):
        original = Command(
            sequence_number=42,
            content_type=ContentType.SET_DEVICE_NAME,
            content=SetDeviceName(
                device_name="ESP32_Test"
            ),
        )

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), 9 + 64)
        self.assertEqual(decoded.sequence_number, 42)
        self.assertEqual(
            decoded.content_type,
            ContentType.SET_DEVICE_NAME,
        )
        self.assertIsInstance(decoded.content, SetDeviceName)
        self.assertEqual(
            decoded.content.device_name,
            "ESP32_Test",
        )

    def test_command_with_set_device_status(self):
        original = Command(
            sequence_number=43,
            content_type=ContentType.SET_DEVICE_STATUS,
            content=SetDeviceStatus(
                status=DeviceStatus.NETWORK_ERROR
            ),
        )

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), 10)
        self.assertEqual(decoded.sequence_number, 43)
        self.assertIsInstance(decoded.content, SetDeviceStatus)
        self.assertEqual(
            decoded.content.status,
            DeviceStatus.NETWORK_ERROR,
        )

    # --------------------------------------------------
    # Outer Message envelope
    # --------------------------------------------------

    def test_message_with_information(self):
        original = Message(
            device_name="ESP32_Test",
            mac_address=0x123456789ABC,
            time_stamp=1700000000.0,
            is_encrypted=0,
            content_type=ContentType.INFORMATION,
            content=Information(
                status=DeviceStatus.NORMAL,
                temperature=22.5,
                humidity=48.0,
                carbon_dioxide=500.0,
                natrual_gas=0.0,
                smoke=0.0,
            ),
        )

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), 80 + 21)
        self.assertEqual(decoded.device_name, "ESP32_Test")
        self.assertEqual(decoded.mac_address, 0x123456789ABC)
        self.assertEqual(decoded.time_stamp, 1700000000.0)
        self.assertEqual(decoded.is_encrypted, 0)
        self.assertEqual(
            decoded.content_type,
            ContentType.INFORMATION,
        )

        self.assertIsInstance(decoded.content, Information)
        self.assertEqual(decoded.content.status, DeviceStatus.NORMAL)
        self.assertAlmostEqual(decoded.content.temperature, 22.5)
        self.assertAlmostEqual(decoded.content.humidity, 48.0)
        self.assertAlmostEqual(
            decoded.content.carbon_dioxide, 500.0
        )

    def test_message_with_command_and_device_name(self):
        original = Message(
            device_name="PC",
            mac_address=0xAABBCCDDEEFF,
            time_stamp=1700000001.0,
            content_type=ContentType.COMMAND,
            content=Command(
                sequence_number=100,
                content_type=ContentType.SET_DEVICE_NAME,
                content=SetDeviceName(
                    device_name="ESP32_Kitchen"
                ),
            ),
        )

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), 80 + 9 + 64)
        self.assertIsInstance(decoded.content, Command)
        self.assertEqual(decoded.content.sequence_number, 100)
        self.assertIsInstance(
            decoded.content.content,
            SetDeviceName,
        )
        self.assertEqual(
            decoded.content.content.device_name,
            "ESP32_Kitchen",
        )

    def test_message_default_values(self):
        original = Message()

        encoded, decoded = self.assert_round_trip(original)

        self.assertEqual(len(encoded), Message.__size__)
        self.assertEqual(decoded.device_name, "")
        self.assertEqual(decoded.mac_address, 0)
        self.assertEqual(decoded.is_encrypted, 0)
        self.assertEqual(decoded.content_type, ContentType.NONE)
        self.assertIsNone(decoded.content)

    def test_message_name_too_long(self):
        original = Message(device_name="A" * 65)

        with self.assertRaises(ValueError):
            original.to_bytes()

    def test_message_mac_address_too_large(self):
        original = Message(mac_address=1 << 48)

        with self.assertRaises(OverflowError):
            original.to_bytes()

    # --------------------------------------------------
    # Basic binary layout
    # --------------------------------------------------

    def test_information_payload_size(self):
        self.assertEqual(Information.__size__, 21)

    def test_command_payload_size(self):
        self.assertEqual(Command.__size__, 9)

    def test_message_header_size(self):
        self.assertEqual(Message.__size__, 80)

    def test_message_timestamp_is_eight_bytes(self):
        original = Message(time_stamp=1700000000.0)
        encoded = original.to_bytes()

        # Timestamp starts after version (1), device name (64),
        # and MAC address (6).
        timestamp_bytes = encoded[71:79]

        self.assertEqual(len(timestamp_bytes), 8)


if __name__ == "__main__":
    unittest.main(verbosity=2)