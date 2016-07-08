#include <gflags/gflags.h>
#include <folly/Memory.h>
#include <folly/Function.h>
#include <folly/Portability.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/httpserver/ResponseHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <proxygen/lib/http/HTTPConnector.h>
#include <unistd.h>
#include <iostream>

#include "ReverseProxyHandlerFactory.h"

using namespace proxygen;
using namespace folly;

using folly::EventBase;
using folly::EventBaseManager;
using folly::SocketAddress;
using namespace std;

using Protocol = HTTPServer::Protocol;

DEFINE_int32(http_port, 11000, "Port to listen on with HTTP protocol");


int main(int argc, char *argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    std::vector<HTTPServer::IPConfig> IPs = {
        {SocketAddress("localhost", FLAGS_http_port, true), Protocol::HTTP}
    };

    HTTPServerOptions options;


    options.handlerFactories = RequestHandlerChain()
        .addThen<ReverseProxyHandlerFactory>()
        .build();


    HTTPServer server(std::move(options));

    server.bind(IPs);

    std::thread t([&] () {
            server.start();
        });

    t.join();

    return 0;
}
