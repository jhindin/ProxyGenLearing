
PROGRAMS=EchoServer MyFirstServer MySecondServer MyThirdServer SimpleReverseProxy

all: ${PROGRAMS}

CXXFLAGS=   -std=c++14 -g -Wall `pkg-config --cflags proxygen` `pkg-config --cflags libfolly` `pkg-config --cflags libglog` `pkg-config --cflags libgflags` 
LDLIBS=`pkg-config --libs proxygen` `pkg-config --libs libfolly` `pkg-config --libs libglog` `pkg-config --libs libgflags` -lpthread

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
