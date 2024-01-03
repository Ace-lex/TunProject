## libTun

tun发送udp报文的动态库，编译后放到其他目录下

## TunRecvSend&UDPSend

TunRecvSend可以接收/发送UDP数据包

```shell
make
./TunRecvSend [file]
./script.sh
./UDPSend [file]
```

## Test

测试发送的UDP包正确（仅测试发送）

```shell
cd Test
make
./UDPServer
./TunTest
```

## Roce

发送Rocev2格式的数据包

```shell
cd TunRoce
make
./UDPServer
./RoceSend
```

