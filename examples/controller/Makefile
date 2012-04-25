
CPPFLAGS += -I $(HPCL_BASE)/include
CXXFLAGS += -Wall -Wextra -Weffc++ -std=c++0x -pthread -g3 -O0

LDFLAGS += -L $(HPCL_BASE)/lib \
           -lhpcl-messaging \
           -lhpcl-logging \
           -lhpcl-remote-exec \
           -lhpcl-communication \
           -lhpcl-common \
           -Wl,-rpath -Wl,$(HPCL_BASE)/lib -lrt

all: logd execd agent controller

logd: logd.o
		${CXX} $< ${CXXFLAGS} ${LDFLAGS} -o $@

execd: execd.o
		${CXX} $< ${CXXFLAGS} ${LDFLAGS} -o $@

agent: agent.o
		${CXX} $< ${CXXFLAGS} ${LDFLAGS} -o $@

controller: controller.o
		${CXX} $< ${CXXFLAGS} ${LDFLAGS} -o $@

.PHONY: clean

clean:
	-rm -f logd execd agent controller *.o

