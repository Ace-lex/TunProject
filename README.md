

## libTun

使用TUN虚拟设备发送UDP数据包的相关函数

## example

借助libTun.so发送接收UDP数据包的使用示例

```shell
cd build/example
./tundevice [filename]
./normaldevice [filename]
```

## test

测试发送的UDP包格式正确

```shell
cd build/test
./test.sh
```

## tunroce

组装最基础的RoCEv2数据包

```shell
cd build/tunroce
./rocerecv
./rocesend
```

## compile

```shell
cmake -B build
cmake --build build
```

## 关于UDPTunSend的设计

目前的API如下，用户需要传入tun设备的文件描述符，源目地址和端口信息以及载荷，这种设计方便用户调用，但每调用一次函数就需要在函数内开辟一块内存空间存放报文：

```C++
int UDPTunSend(int tun, const char *src_ip, const char *dest_ip, int src_port,
               int dest_port, const u_int8_t *message, int payload_length);
```

该API也有其他的设计方式，比如只传入存放报文的缓冲区指针buf，这种方法在一定程度上节省了空间(不需要为载荷开辟空间，buf只需在程序起始处开辟一个即可重复使用)，但不方便用户调用，用户需要自行将载荷放到buf的合适位置

```C++
int UDPTunSend(int tun, const char *src_ip, const char *dest_ip, int src_port,
               int dest_port, const u_int8_t *buf, int payload_length);
```

最初的设计是用户需要传入buf指针和载荷指针，这种设计虽然不需要在函数内开辟内存空间，但是对于用户来说不如直接传入载荷方便。
