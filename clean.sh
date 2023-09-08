#!/bin/bash

# cmake
rm -fr generated
rm -fr cmake_install.cmake
rm -fr Makefile
rm -fr CMakeFiles
rm -fr CMakeCache.txt

# Pico C SDK
rm -fr *.uf2 *.hex *.bin *.dis *.elf *.elf.map
rm -fr pioasm
rm -fr pico-sdk
rm -fr elf2uf2

