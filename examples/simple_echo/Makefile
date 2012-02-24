
all: client server

client: client.cpp
		g++ -Wall -Wextra -Weffc++ client.cpp -o client -g -std=c++0x -pthread -lhpcl-communication -lhpcl-common -I /home/santanu/tmp/include -L /home/santanu/tmp/lib -Wl,-rpath -Wl,/home/santanu/tmp/lib

server: server.cpp
		g++ -Wall -Wextra -Weffc++ server.cpp -o server -g -std=c++0x -pthread -lhpcl-communication -lhpcl-common -I /home/santanu/tmp/include -L /home/santanu/tmp/lib -Wl,-rpath -Wl,/home/santanu/tmp/lib

.PHONY: clean

clean:
	-rm -f client server

