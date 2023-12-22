APPS = TunRecv UDPSend
all : $(APPS)

clean:
	$(foreach fname,${APPS}, rm -f ${fname})

TunRecv:clean
		g++ TunRecv.cpp -o TunRecv
UDPSend:clean
		g++ UDPSend.cpp -o UDPSend

.PHONY: all clean