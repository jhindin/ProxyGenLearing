
PROGRAMS=EchoServer MyFirstServer MySecondServer MyThirdServer

all: ${PROGRAMS}

CXXFLAGS=-std=c++11 -g
LDLIBS=-lproxygenhttpserver -lfolly -lglog -lgflags -pthread

EchoServer.SRCS=EchoServer.cc EchoHandler.cc
MyFirstServer.SRCS=MyFirstServer.cc
MySecondServer.SRCS=MySecondServer.cc
MyThirdServer.SRCS=MyThirdServer.cc


.SECONDEXPANSION:
${PROGRAMS}: % : $${$$@.SRCS}
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDLIBS}

.PHONY: all

clean:
	rm -f ${PROGRAMS} *.o *~
