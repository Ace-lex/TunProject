#!/bin/bash
modprobe tun
 
echo =====small payloads=====
./profile small

echo =====medium payloads=====
./profile medium

echo =====large payloads=====
./profile large