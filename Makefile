
PROGRAMS=EchoServer MyFirstServer MySecondServer MyThirdServer SimpleReverseProxy

all: ${PROGRAMS}

CXXFLAGS=-std=c++11 -g -Wall
LDLIBS=-lproxygenhttpserver -lproxygencurl -lfolly -lglog -lgflags -pthread

EchoServer.SRCS=EchoServer.cc EchoHandler.cc
MyFirstServer.SRCS=MyFirstServer.cc
MySecondServer.SRCS=MySecondServer.cc
MyThirdServer.SRCS=MyThirdServer.cc
SimpleReverseProxy.SRCS=SimpleReverseProxy.cc ReverseProxyHandler.cc ReverseProxyHandlerFactory.cc


.SECONDEXPANSION:
${PROGRAMS}: % : $${$$@.SRCS}
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDLIBS}

.PHONY: all

clean:
	rm -f ${PROGRAMS} *.o *~
