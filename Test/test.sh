#!/bin/bash
modprobe tun
echo test begin
dst_ip=`ifconfig -a|grep inet|grep -v 127.0.0.1|grep -v inet6|awk '{print $2}'|tr -d "addr:"`
LD_LIBRARY_PATH=. ./UDPServer &
LD_LIBRARY_PATH=. ./TunTest $dst_ip
echo test end