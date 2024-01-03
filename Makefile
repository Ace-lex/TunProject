APPS = TunRecvSend UDPSend
all : $(APPS)

clean:
	$(foreach fname,${APPS}, rm -f ${fname})

TunRecvSend:clean
		g++ TunRecvSend.cpp -o TunRecvSend -L. -lTun
UDPSend:clean
		g++ UDPSend.cpp -o UDPSend

.PHONY: all clean