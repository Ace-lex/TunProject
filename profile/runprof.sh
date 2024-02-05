#!/bin/bash
modprobe tun
 
echo =====1kb payloads=====
./profile prof_1kb

echo =====4kb payloads=====
./profile prof_4kb

echo =====8kb payloads=====
./profile prof_8kb

echo =====16kb payloads=====
./profile prof_16kb