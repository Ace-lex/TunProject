APPS = RoceSend UDPServer
all : $(APPS)

clean:
	$(foreach fname,${APPS}, rm -f ${fname})

RoceSend:clean
		g++ RoceSend.cpp -o RoceSend -L. -lTun
UDPServer:clean
		g++ UDPServer.cpp -o UDPServer -L. -lTun

.PHONY: all clean