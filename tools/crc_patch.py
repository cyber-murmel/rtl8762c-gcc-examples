#! /usr/bin/env python

from sys import argv
from struct import pack, unpack

from crccheck.crc import CrcArc

if __name__ == "__main__":
    with open(argv[1], "rb+") as file:
        file.seek(0x4)
        id = unpack("<H", file.read(2))[0]

        start = 0
        end = 0

        # determin start and end points of data
        if 0x278D == id:
            # oem config
            file.seek(0x8)
            data_len = unpack("<I", file.read(4))[0]

            start = 0x10C
            end = 0x400 + data_len + -(data_len % -4)
        elif 0x2790 == id:
            # ota bank header
            start = 0xC
            end = 0x300

        # padd file end if required
        file.seek(0, 2)
        file.write(b"\xff" * max(0, end - file.tell()))

        # calculate and write crc
        file.seek(start)
        crc = CrcArc.calc(file.read(end - start))
        file.seek(0x6)
        file.write(pack("<H", crc))

        # pad to 4kiB
        file.seek(0, 2)
        file.write(b"\xff" * -(file.tell() % -0x1000))
