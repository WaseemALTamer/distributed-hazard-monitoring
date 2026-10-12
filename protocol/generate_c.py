from pathlib import Path
BASE_DIR = Path(__file__).resolve().parent




import protocol



from dataclasses import is_dataclass, fields, MISSING
from enum import Enum
import inspect
import shutil
import os


INT_MAP = {
    1: "uint8_t",
    2: "uint16_t",
    4: "uint32_t",
    8: "uint64_t",
}

FLOAT_MAP = {
    4: "float",
    8: "double",
}



def c_declaration(name, py_type, size, default):
    """Return the C declaration line for one field in __fields__."""

    if isinstance(default, Enum):
        return f"{type(default).__name__} {name};"

    if py_type is str:
        return f"char {name}[{size}];"

    if py_type is float:
        return f"{FLOAT_MAP[size]} {name};"

    if py_type is int:
        if size in INT_MAP:
            return f"{INT_MAP[size]} {name};"
        return f"uint8_t {name}[{size}];"

    raise TypeError(f"{name}: unsupported type {py_type!r}")



enums:list[Enum] = []
message_classes:list[object] = []



for name, cls in inspect.getmembers(protocol, inspect.isclass):
    if cls.__module__ != protocol.__name__:
        continue

    if cls is protocol.MessageBase:
        continue

    if (
        is_dataclass(cls)
        and issubclass(cls, protocol.MessageBase)
    ):
        message_classes.append(cls)

    elif issubclass(cls, Enum):
        enums.append(cls)


def to_snake(name):
    out = ""
    for i, ch in enumerate(name):
        if ch.isupper() and i:
            out += "_"
        out += ch
    return out

def generate_enum(enum):
    prefix = to_snake(enum.__name__).upper()
    lines = [f"typedef enum {enum.__name__} {{"]

    for name, member in enum.__members__.items():
        lines.append(f"    {prefix}_{name} = {member.value},")

    lines.append(f"}} {enum.__name__};")
    return "\n".join(lines) + "\n"







def generate_struct(_class):
    lines = [
        "typedef struct{",
    ]

    for f in fields(_class):
        default = f.default if f.default is not MISSING else None

        if f.name in _class.__fields__:
            py_type, size = _class.__fields__[f.name]
            lines.append("    " + c_declaration(f.name, py_type, size, default))
        else:
            lines.append(f"    void *{f.name};")

    lines.append(f"}} {_class.__name__};")
    return "\n".join(lines)


def _defaults(_class):
    return {
        f.name: (f.default if f.default is not MISSING else None)
        for f in fields(_class)
    }


def _encode_field(name, py_type, size, default):
    if isinstance(default, Enum):
        return [f"buffer[offset++] = (uint8_t)msg->{name};"]

    if py_type is str:
        return [
            f"memset(&buffer[offset], 0, {size});",
            f"strncpy((char *)&buffer[offset], msg->{name}, {size - 1});",
            f"offset += {size};",
        ]

    if py_type is int and size == 1:
        return [f"buffer[offset++] = msg->{name};"]

    if py_type is int and size not in INT_MAP:
        # byte array in the struct (e.g. mac_address[6]), so no &
        return [
            f"memcpy(&buffer[offset], msg->{name}, {size});",
            f"offset += {size};",
        ]

    # float, or int of size 2/4/8
    return [
        f"memcpy(&buffer[offset], &msg->{name}, {size});",
        f"offset += {size};",
    ]


def _decode_field(name, py_type, size, default):
    if isinstance(default, Enum):
        max_value = max(m.value for m in type(default))
        return [
            f"if (buffer[offset] > {max_value})",
            "    return NULL;",
            f"msg->{name} = ({type(default).__name__})buffer[offset++];",
        ]

    if py_type is str:
        return [
            f"memcpy(msg->{name}, &buffer[offset], {size});",
            f"msg->{name}[{size - 1}] = '\\0';",
            f"offset += {size};",
        ]

    if py_type is int and size == 1:
        return [f"msg->{name} = buffer[offset++];"]

    if py_type is int and size not in INT_MAP:
        return [
            f"memcpy(msg->{name}, &buffer[offset], {size});",
            f"offset += {size};",
        ]

    return [
        f"memcpy(&msg->{name}, &buffer[offset], {size});",
        f"offset += {size};",
    ]


def generate_encoder(_class):
    name = _class.__name__
    defaults = _defaults(_class)

    lines = [
        f"uint8_t *{to_snake(name).lower()}_encode(const {name} *msg, uint8_t *buffer, size_t *buffer_size)",
        "{",
        f"    if (*buffer_size < {_class.__size__})",
        "        return NULL;",
        "",
        "    size_t offset = 0;",
        "",
    ]

    for field_name, (py_type, size) in _class.__fields__.items():
        for line in _encode_field(field_name, py_type, size, defaults[field_name]):
            lines.append("    " + line)
        lines.append("")

    lines += [
        "    *buffer_size -= offset;",
        "    return buffer + offset;",
        "}",
    ]
    return "\n".join(lines)


def generate_decoder(_class):
    name = _class.__name__
    defaults = _defaults(_class)

    lines = [
        f"const uint8_t *{to_snake(name).lower()}_decode({name} *msg, const uint8_t *buffer, size_t *buffer_size)",
        "{",
        f"    if (*buffer_size < {_class.__size__})",
        "        return NULL;",
        "",
        "    size_t offset = 0;",
        "",
    ]

    for field_name, (py_type, size) in _class.__fields__.items():
        for line in _decode_field(field_name, py_type, size, defaults[field_name]):
            lines.append("    " + line)
        lines.append("")

    lines += [
        "    *buffer_size -= offset;",
        "    return buffer + offset;",
        "}",
    ]
    return "\n".join(lines)


generated_h = """/*
    THIS CODE IS GENERATED. DO NOT CHANGE.
    IF YOU WANT TO CHANGE IT, MODIFY THE PYTHON PROTOCOL
    AND REGENERATE THIS CODE.
*/

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stddef.h>

"""


# Generate enums and structs first
for enum in enums:
    generated_h += generate_enum(enum)
    generated_h += "\n\n"


for _class in message_classes:
    generated_h += generate_struct(_class)
    generated_h += "\n\n"


# Use C linkage for function declarations when included from C++
generated_h += """
#ifdef __cplusplus
extern "C" {
#endif

"""


for _class in message_classes:
    name = _class.__name__
    function_name = to_snake(name).lower()

    generated_h += (
        f"uint8_t *{function_name}_encode(\n"
        f"    const {name} *msg,\n"
        f"    uint8_t *buffer,\n"
        f"    size_t *buffer_size\n"
        ");\n\n"
    )

    generated_h += (
        f"const uint8_t *{function_name}_decode(\n"
        f"    {name} *msg,\n"
        f"    const uint8_t *buffer,\n"
        f"    size_t *buffer_size\n"
        ");\n\n"
    )


generated_h += """
#ifdef __cplusplus
}
#endif

#endif /* PROTOCOL_H */
"""



generated_c = """
/*
    THIS CODE IS GENEARTED DO NOT CHANGE, IF YOU WANT TO CHANGE LOOK THROUGH THE
    PYTHON MIRROR OF THE PROTOCAL AND REGENERATE THE CODE WITH THE GENERATOR
*/

#include "protocol.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <stdio.h>



"""







for _class in message_classes:
    generated_c += generate_encoder(_class) + "\n" * 2
    generated_c += generate_decoder(_class) + "\n" * 2




if __name__ == "__main__":

    folder = "generated"
    os.makedirs(folder, exist_ok=True)

    header_file_path = os.path.join(folder, "protocol.h")
    implementation_file_path = os.path.join(folder, "protocol.c")


    with open(header_file_path, "w") as f:
        f.write(generated_h)

    with open(implementation_file_path, "w") as f:
        f.write(generated_c)

    protocol_python_path = os.path.join(BASE_DIR, "protocol.py")
    protocol_python_copy_path = os.path.join(folder, "protocol.py")

    shutil.copy(protocol_python_path, protocol_python_copy_path)


