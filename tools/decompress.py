#!/usr/bin/python3

import struct
import decurrent_pb2
import snappy

infile = open("out.bin", "rb")
outfile = open("raw.bin", "wb")

while True:
    hdr = infile.read(4)
    if len(hdr) != 4: break
    packetlen = struct.unpack("<I", hdr)[0] - 4
    packet = infile.read(packetlen)
    if len(packet) != packetlen: break

    msg = decurrent_pb2.USBResponse.FromString(packet)

    if msg.data:
        digital = snappy.decompress(msg.data)
        outfile.write(digital)