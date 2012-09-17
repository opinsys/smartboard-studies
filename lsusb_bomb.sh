#!/bin/bash

set -eu

lsusb_loop() {
    for i in {1..1000}
    do
	lsusb -v > /dev/null
    done
}

for i in {1..10}
do
    lsusb_loop&
done