#!/bin/bash

i2ctransfer -f -y 1 w2@0x70 0x01 0x02 r1
dmesg | tail -50
