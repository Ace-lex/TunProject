APPS = TunRecv UDPSend
all : $(APPS)

TunRecv:
		g++ TunRecv.cpp -o TunRecv
UDPSend:
		g++ UDPSend.cpp -o UDPSend