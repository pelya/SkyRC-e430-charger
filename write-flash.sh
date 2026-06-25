#!/bin/sh

stm8flash/stm8flash -c stlinkv2 -p 'stm8s903?3' -w out/main.ihx
