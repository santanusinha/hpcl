
CPPFLAGS += -I $(HPCL_BASE)/include
CXXFLAGS += -Wall -Wextra -Weffc++ -std=c++0x -pthread -g3

LDFLAGS += -L $(HPCL_BASE)/lib \
           -lhpcl-communication \
           -lhpcl-common \
           -lboost_serialization \
           -lrt \
           -Wl,-rpath -Wl,$(HPCL_BASE)/lib

all: client server

client: client.o
		${CXX} $< ${CXXFLAGS} ${LDFLAGS} -o $@

server: server.o
		${CXX} $< ${CXXFLAGS} ${LDFLAGS} -o $@

.PHONY: clean

clean:
	-rm -f client server *.o

