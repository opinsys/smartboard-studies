#!/bin/sh

set -eu

lsusb_loop() {
    for _ in $(seq 1 10000)
    do
	lsusb -v >/dev/null 2>&1
    done
}

for _ in $(seq 1 15)
do
    lsusb_loop&
done