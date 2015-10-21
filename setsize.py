#!/usr/bin/python
import io
import os
import sys

if __name__ == '__main__':
    img = sys.argv[1]
    elf = sys.argv[2]
    block_size = 32*1024
    elf_size_in_bytes = os.stat(elf).st_size
    img_size = os.stat(img).st_size
    # round up
    elf_size_in_blocks = (elf_size_in_bytes-1)//block_size+1
    print('elf_size_in_blocks', sys.argv[2], elf_size_in_blocks)
    elf_data = bytearray(img_size)
    with io.FileIO(img, 'r') as fp:
        n = fp.readinto(elf_data)
        elf_data = elf_data[:n]
    elf_data[0x11] = (elf_size_in_blocks & 0xff00) >> 8
    elf_data[0x10] = elf_size_in_blocks & 0xff
    with open(img, 'w') as fp:
        fp.write(elf_data)
