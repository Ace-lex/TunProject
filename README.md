## TunRecv&UDPSend

用于接收发送UDP包

```shell
make
./TunRecv [file]
./script.sh
./UDPSend [file]
```

## Test

测试发送的UDP包正确

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

