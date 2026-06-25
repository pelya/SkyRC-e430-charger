#!/bin/sh

make || exit 1

[ -n "$1" ] && ./write-flash.sh
