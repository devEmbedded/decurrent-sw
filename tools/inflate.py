#!/usr/bin/python3
import sys
import zlib

data = sys.stdin.read()
data = zlib.decompress(data, -15)
sys.stdout.write(data)
