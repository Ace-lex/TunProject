

## libTun

使用TUN虚拟设备发送UDP数据包的相关函数

## example

借助libTun.so发送接收UDP数据包的使用示例

```shell
cd build/example
./TunDevice [filename]
./NormalDevice [filename]
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
./RoceRecv
./RoceSend
```

## compile

```shell
cmake -B build
cmake --build build
```

