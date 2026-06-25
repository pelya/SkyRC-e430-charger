#!/bin/sh

stm8flash/stm8flash -c stlinkv2 -p 'stm8s903?3' -r original-fw.bin
stm8flash/stm8flash -c stlinkv2 -p 'stm8s903?3' -s eeprom -r original-eeprom.bin
stm8flash/stm8flash -c stlinkv2 -p 'stm8s903?3' -s opt -r original-opt.bin

