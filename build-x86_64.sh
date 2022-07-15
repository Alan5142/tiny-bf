#!/bin/bash
clang -nostdlib -ffast-math -fno-threadsafe-statics -fdelete-null-pointer-checks -static -Oz -march=native \
 -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-stack-protector \
 -Wl,--gc-sections -s -Wl,-z -Wl,noseparate-code -Wl,--build-id=none main.c -o main -I /usr/include/x86_64-linux-gnu
strip main -s -R .comment -R .gnu.version -x -R .shstrtab --strip-debug -R NULL
./sstrip main
ls -l main