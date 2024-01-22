#!/bin/bash
modprobe tun
echo test begin

# Get host ip
dst_ip=`ifconfig -a|grep inet|grep -v 127.0.0.1|grep -v inet6|awk '{print $2}'|tr -d "addr:"`

# Run UDPServer and save the pid
./udpserver &
udpserver_pid=$!

# Run TunTest
./tuntest $dst_ip

# Check the exit status of UDPServer
wait $udpserver_pid
if [ $? -ne 0 ];then
    exit 1
fi
echo test end