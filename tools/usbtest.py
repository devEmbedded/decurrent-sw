#!/usr/bin/python3

import usb.core
import usb.util
import time

dev = usb.core.find(idVendor=0x1d50, idProduct=0xffff)

protobuf_iface = next(x for x in dev.get_active_configuration().interfaces() if x.bInterfaceClass == 0xFF)
protobuf_ep = protobuf_iface.endpoints()[1]

outfile = open("out.bin", "wb")

prevtime = time.time()
totallen = 0
while True:
    data = protobuf_ep.read(4096 * 1024)
    
    timenow = time.time()
    delta = timenow - prevtime
    prevtime = timenow

    totallen += len(data)
    print("Got %d bytes, %0.3f MB/s, total %d MB" % (len(data), len(data) / 1024.0 / 1024.0 / delta, totallen // 1024 // 1024))
    outfile.write(bytes(data))
    
