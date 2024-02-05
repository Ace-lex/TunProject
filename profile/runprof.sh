#!/bin/bash
modprobe tun

for folder in prof_*/; do
    echo "=====$(basename "$folder")====="
    ./profile $folder
done