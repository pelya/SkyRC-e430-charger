#!/bin/sh

make || exit 1

./write-flash.sh
