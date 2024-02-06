

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

## memcpy+write vs writev

操作系统: Debian 11.1.0 64bit 

Linux kernel: 5.10.0-23-amd64

UDPTunSend使用memcpy将报文头部和载荷拼装起来，然后使用write系统调用写入虚拟网卡；UDPTunSendv2由writev拼装头部和载荷并写入网卡，下面对两种实现的性能进行对比

使用1kb, 4kb, 8kb大小的载荷进行测试，结合perf工具测得结果如下：

（1）使用UDPTunSend和UDPTunSendv2发送20个载荷为1kb的数据包，耗时与CPU占用如下所示

|                       | UDPTunSend | UDPTunSendv2 |
| --------------------- | ---------- | ------------ |
| **耗时（微秒）** | 232        | 185          |
| **CPU占用**           | 4.97%      | 2.12%        |

（2）使用UDPTunSend和UDPTunSendv2发送20个载荷为4kb的数据包，耗时与CPU占用如下所示

|                       | **UDPTunSend** | **UDPTunSendv2** |
| --------------------- | -------------- | ---------------- |
| **耗时（微秒）** | 512            | 326              |
| **CPU占用**           | 6.30%          | 3.61%            |

（3）使用UDPTunSend和UDPTunSendv2发送20个载荷为8kb的数据包，耗时与CPU占用如下所示

|                       | UDPTunSend | UDPTunSendv2 |
| --------------------- | ---------- | ------------ |
| **耗时（微秒）** | 602        | 447          |
| **CPU占用**           | 8.44%      | 6.40%        |

（4）使用UDPTunSend和UDPTunSendv2发送20个载荷为16kb的数据包，耗时与CPU占用如下所示

|                  | UDPTunSend | UDPTunSendv2 |
| ---------------- | ---------- | ------------ |
| **耗时（微秒）** | 927        | 792          |
| **CPU占用**      | 11.20%     | 9.68%        |

（5）使用UDPTunSend和UDPTunSendv2发送20个载荷为32kb的数据包，耗时与CPU占用如下所示

|                  | UDPTunSend | UDPTunSendv2 |
| ---------------- | ---------- | ------------ |
| **耗时（微秒）** | 1543       | 1607         |
| **CPU占用**      | 12.91%     | 13.39%       |

发现当数据包较小（小于16kb）时，writev的时间性能好于memcpy+write的时间性能，在数据包较大（大于等于16kb）时，writev与memcpy+write性能相近，在不同版本的操作系统和内核中会出现memcpy+write的性能好于writev的情况。
