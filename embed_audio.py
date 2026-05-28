#!/usr/bin/env python3
import os
import sys


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input_file> <output_header>", file=sys.stderr)
        sys.exit(1)

    input_file = sys.argv[1]
    output_header = sys.argv[2]

    with open(input_file, "rb") as f:
        data = f.read()

    var_name = os.path.basename(input_file).replace(".", "_").replace("-", "_")

    with open(output_header, "w") as f:
        f.write("#pragma once\n\n")
        f.write(f"static const unsigned char {var_name}[] = {{\n")

        for i in range(0, len(data), 12):
            chunk = data[i : i + 12]
            hex_vals = ", ".join(f"0x{b:02x}" for b in chunk)
            f.write(f"    {hex_vals},\n")

        f.write("}};\n\n")
        f.write(f"static const unsigned int {var_name}_len = {len(data)};\n")

    print(f"Generated {output_header}: {len(data)} bytes")


if __name__ == "__main__":
    main()
