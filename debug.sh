#!/bin/bash

set -e
qemu-system-x86_64 -snapshot -hda loader.img -s&
QEMU="$!"
sleep 8 # sleep enough time to get QEMU to x64 mode
ps --pid $QEMU >/dev/null
kill_gently() {
	kill $1 && return
	sleep 0.1
	kill -9 $1
}
trap "kill_gently $QEMU" exit
gdb -quiet -ex 'set arch i386:x64-32' \
	-ex 'target remote localhost:1234' \
	-ex 'set $eax = 0xfeed'
