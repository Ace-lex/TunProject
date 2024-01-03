APPS = TunTest UDPServer
all : $(APPS)

clean:
	$(foreach fname,${APPS}, rm -f ${fname})

TunTest:clean
		g++ TunTest.cpp -o TunTest -L. -lTun
UDPServer:clean
		g++ UDPServer.cpp -o UDPServer -L. -lTun

.PHONY: all clean