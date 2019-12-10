#!/usr/bin/python3

import usb.core
import usb.util

dev = usb.core.find(idVendor=0x1d50, idProduct=0xffff)

protobuf_iface = next(x for x in dev.get_active_configuration().interfaces() if x.bInterfaceClass == 0xFF)
protobuf_ep = protobuf_iface.endpoints()[1]

outfile = open("out.bin", "wb")

while True:
    data = protobuf_ep.read(16384)
    print("Got %d bytes" % len(data))
    outfile.write(bytes(data))
    
