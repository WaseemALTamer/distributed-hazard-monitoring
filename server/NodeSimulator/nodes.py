from __future__ import annotations



from dataclasses import dataclass
from typing import Optional
from pathlib import Path
import random
import time
import sys



current_dir = Path(__file__).resolve().parent
sys.path.insert(0, str(current_dir))

from protocol import (
    ContentType,
    DeviceStatus,
    Information,
    Message,
)



# parent class 
@dataclass
class VirtualNode: 
    device_name: str
    mac_address: int
    status: DeviceStatus = DeviceStatus.NORMAL
    temperature: float = 21.0
    humidity: float = 45.0
    carbon_dioxide: float = 600.0
    natrual_gas: float = 0.0
    smoke: float = 0.0

    @staticmethod
    def generate_mac_address(used: set[int]) -> int:
        """
            this will genearte a mac address the mac address follows
            a squence so  you will  always get  the same mac address
            as long as your pramatesr are the same
        """
        mac = 0x020000000001

        while mac in used:
            mac += 1

        used.add(mac)
        return mac

    @staticmethod
    def format_mac(mac: int) -> str:
        """
            convert our mac address from int formate to string formate
        """
        return ":".join(f"{(mac >> shift) & 0xFF:02X}" for shift in range(40, -1, -8))

    def update_readings(self) -> None:
        self.temperature = round(max(-10.0, min(50.0, self.temperature + random.uniform(-0.4, 0.4))), 2)
        self.humidity = round(max(10.0, min(95.0, self.humidity + random.uniform(-1.5, 1.5))), 2)
        self.carbon_dioxide = round(max(350.0, min(2000.0, self.carbon_dioxide + random.uniform(-35.0, 35.0))), 2)
        self.natrual_gas = round(max(0.0, min(100.0, self.natrual_gas + random.uniform(-0.1, 0.1))), 2)
        self.smoke = round(max(0.0, min(100.0, self.smoke + random.uniform(-0.1, 0.1))), 2)

    def to_message(self) -> Message:
        """
            convert the current values into a message this doesnt
            not conver the message to bytes for you  you will get
            a Message Data type refere to the protocal
        """
        info = Information(
            status=int(self.status),
            temperature=self.temperature,
            humidity=self.humidity,
            carbon_dioxide=self.carbon_dioxide,
            natrual_gas=self.natrual_gas,
            smoke=self.smoke,
        )

        message = Message()
        message.device_name = self.device_name
        message.mac_address = self.mac_address
        message.time_stamp = time.time()
        message.is_encrypted = 0
        message.content_type = ContentType.INFORMATION
        message.content = info
        return message


class NormalNode(VirtualNode):
    # override the update reading function
    def update_readings(self) -> None:
        """
            this function will fabraicate normal data
        """

        self.status = DeviceStatus.NORMAL
        self.temperature = round(random.uniform(18.0, 27.0), 2)
        self.humidity = round(random.uniform(30.0, 60.0), 2)
        self.carbon_dioxide = round(random.uniform(400.0, 900.0), 2)
        self.natrual_gas = round(random.uniform(0.0, 0.5), 2)
        self.smoke = round(random.uniform(0.0, 0.5), 2)


class DangerousNode(VirtualNode):
    # override the update reading function
    def update_readings(self) -> None:
        """
            this function will fabraicate dangerous date
        """

        self.status = DeviceStatus.DANGER
        hazard = random.choice(("temperature", "carbon_dioxide", "natural_gas", "smoke"))
        self.temperature = round(random.uniform(18.0, 30.0), 2)
        self.humidity = round(random.uniform(30.0, 70.0), 2)
        self.carbon_dioxide = round(random.uniform(450.0, 1000.0), 2)
        self.natrual_gas = round(random.uniform(0.0, 1.0), 2)
        self.smoke = round(random.uniform(0.0, 1.0), 2)

        if hazard == "temperature":
            self.temperature = round(random.uniform(45.0, 60.0), 2)
        elif hazard == "carbon_dioxide":
            self.carbon_dioxide = round(random.uniform(2500.0, 5000.0), 2)
        elif hazard == "natural_gas":
            self.natrual_gas = round(random.uniform(15.0, 50.0), 2)
        else:
            self.smoke = round(random.uniform(15.0, 50.0), 2)