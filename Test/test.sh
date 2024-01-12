#!/bin/bash
modprobe tun
echo test begin
LD_LIBRARY_PATH=. ./UDPServer &
LD_LIBRARY_PATH=. ./TunTest
echo test end