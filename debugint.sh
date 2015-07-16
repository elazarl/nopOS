#!/bin/bash
gcc -std=gnu99 -I. -Iinclude -isystem include/glibc-compat -isystem include/api -isystem include/api/x64 -c -o /dev/null libc/internal/intscan.c  
