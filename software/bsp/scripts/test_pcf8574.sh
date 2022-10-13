#!/bin/bash

while true; do
	i2ctransfer -f -y 1 w1@0x27 0x00
	sleep 0.01
	i2ctransfer -f -y 1 w1@0x27 0x80
	sleep 0.01
done
