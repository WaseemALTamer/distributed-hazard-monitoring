from __future__ import annotations

from dataclasses import dataclass, field
from typing import get_type_hints
from dataclasses import dataclass
from enum import IntEnum

import struct
import time

_version = 1


class MessageBase:

    __fields__ = {}
    

    def to_bytes(self) -> bytes:
        data = b""

        for field, (field_type, size) in self.__fields__.items():
            
            value = getattr(self, field)
            if value is None:
                continue

            if field not in self.__fields__:
                data += value.to_bytes()
                continue

            
            if isinstance(value, str):
                _bytes = value.encode("utf-8")
                if len(_bytes) > size:
                    raise ValueError(
                        f"Field '{field}' is too long: "
                        f"{len(_bytes)} bytes (maximum {size})"
                    )

                data += _bytes.ljust(size, b"\x00")
                
            elif isinstance(value, int):
                data += value.to_bytes(size, byteorder="little")
            elif isinstance(value, float):
                if size == 8:
                    data += struct.pack("<d", value)
                elif size == 4:
                    data += struct.pack("<f", value)
                else:
                    raise ValueError(
                        f"Invalid float size: {size}"
                    )
            else:
                pass

        if (
            hasattr(self, "content_type")
            and hasattr(self, "content")
        ):
            value = self.content
            if isinstance(value, MessageBase):
                data += value.to_bytes()

        return data


    def decode(self, data: bytes):

        offset = 0

        for field, (field_type, size) in self.__fields__.items():
            chunk = data[offset:offset + size]
            field_type = get_type_hints(type(self))[field]



            if field_type is str:
                value = chunk.rstrip(b"\x00").decode("utf-8")
            elif field_type is int:
                value = int.from_bytes(
                    chunk,
                    byteorder="little"
                )
            elif field_type is float:
                if size == 8:
                    value = struct.unpack("<d", chunk)[0]
                elif size == 4:
                    value = struct.unpack("<f", chunk)[0]
                else:
                    raise ValueError(
                        f"Invalid float size: {size}"
                    )
            else:
                raise TypeError(
                    f"Cannot decode field '{field}'"
                )

            setattr(self, field, value)
            offset += size


            
        if (
            hasattr(self, "content_type")
            and hasattr(self, "content")
        ):
            remaining = data[offset:]

            if (
                self.content_type != ContentType.NONE
                and len(remaining) > 0
            ):
                message_class = MESSAGE_TYPE_REGISTRY.get(
                    ContentType(self.content_type)
                )

                if message_class is None:
                    raise ValueError(
                        f"Unknown content type: {self.content_type}"
                    )

                self.content = message_class()
                self.content.decode(remaining)


        return self




class ContentType(IntEnum):
    NONE = 0
    MESSAGE = 1
    COMMAND = 2
    INFORMATION = 3
    DISCOVERY = 4
    WIFI_CONNECT = 5
    SET_DEVICE_STATUS = 6
    SET_DEVICE_NAME = 7




class DeviceStatus(IntEnum):
    NONE = 0
    BOOTING = 1
    
    NORMAL = 2

    NETWORK_ERROR = 3
    UNKNOWN_ERROR = 4
    SENSORS_ERROR = 5

    # Temperature / Humidity / CO2 sensor
    TH_SENSOR_ERROR = 6
    TEMPERATURE_ERROR = 7
    HUMIDITY_ERROR = 8
    CO2_ERROR = 9

    # Methane sensor
    METHANE_SENSOR_ERROR = 10
    METHANE_DETECTED = 11
    METHANE_LEVEL_HIGH = 12

    # Smoke sensor
    SMOKE_SENSOR_ERROR = 13
    SMOKE_DETECTED = 14
    SMOKE_LEVEL_HIGH = 15


    WARNING = 16
    DANGER = 17
    





@dataclass
class Message(MessageBase):
    #version: int = _version # this is not going to be inlcuded in the demo
    device_name: str = ""
    mac_address: int = 0
    time_stamp:float = field(default_factory=lambda: time.time())
    is_encrypted: int = 0
    content_type: int = ContentType.NONE
    content: MessageBase = None

    __size__ = 80 # dont try to caulcate it since it will be used to auto genearte the c code
    __fields__ = {
        #"version": (int, 1),
        "device_name": (str, 64),
        "mac_address": (int, 6),
        "time_stamp": (float, 8),
        "is_encrypted": (int, 1),
        "content_type": (int, 1),
    }


@dataclass
class Information(MessageBase):
    status: int = DeviceStatus.NONE
    temperature: float = 0.0
    humidity: float = 0.0
    carbon_dioxide: float = 0.0
    natrual_gas: float = 0.0
    smoke: float = 0.0

    __size__ = 21
    __fields__ = {
        "status": (int, 1),
        "temperature": (float, 4),
        "humidity": (float, 4),
        "carbon_dioxide": (float, 4),
        "natrual_gas": (float, 4),
        "smoke": (float, 4),
    }


@dataclass
class Discovery(MessageBase):
    # empty message this meassage  will be used
    # to ping the esp32 or discover if it is on
    # the network

    __size__ = 0
    __fields__ = {

    }



@dataclass
class Command(MessageBase): # this class can be encreapted later on

    sequence_number: int = 0
    
    content_type:int = ContentType.NONE
    content: MessageBase = None

    __size__ = 9
    __fields__ = {
        "sequence_number": (int, 8),
        "content_type": (int, 1),
    }




# Command Message
@dataclass
class WifiConnect(MessageBase):
    ssid:str = ""
    password:str = ""

    __size__ = 96
    __fields__ = {
        "ssid": (str, 32),
        "password": (str, 64),
    }

# Command Message
@dataclass
class SetDeviceName(MessageBase):
    device_name: str = ""

    __size__ = 64
    __fields__ = {
        "device_name": (str, 64),
    }

# Command Message
@dataclass
class SetDeviceStatus(MessageBase):
    status:int = DeviceStatus.NONE
    
    __size__ = 1
    __fields__ = {
        "status": (int, 1),
    }






MESSAGE_TYPE_REGISTRY = {
    ContentType.MESSAGE: Message,
    ContentType.COMMAND: Command,
    ContentType.WIFI_CONNECT: WifiConnect,
    ContentType.SET_DEVICE_STATUS : SetDeviceStatus,
    ContentType.INFORMATION : Information,
    ContentType.SET_DEVICE_NAME : SetDeviceName,
    ContentType.DISCOVERY : Discovery
}