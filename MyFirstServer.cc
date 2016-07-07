#include <gflags/gflags.h>
#include <folly/Memory.h>
#include <folly/Portability.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/httpserver/ResponseHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <unistd.h>
#include <iostream>

using namespace proxygen;

using folly::EventBase;
using folly::EventBaseManager;
using folly::SocketAddress;
using namespace std;

using Protocol = HTTPServer::Protocol;

DEFINE_int32(http_port, 11000, "Port to listen on with HTTP protocol");


class MyHandler : public RequestHandler {
public:
    MyHandler() : m_body(new folly::IOBuf(folly::IOBuf::CopyBufferOp::COPY_BUFFER, s_defaultBody.data(), s_defaultBody.size())) {
        cout << __PRETTY_FUNCTION__ << endl;

    }
    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers)
        noexcept override
    {
        cout << __PRETTY_FUNCTION__ << endl;

    }

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override
    {
        cout << __PRETTY_FUNCTION__ << endl;
    }


    void onEOM() noexcept override
    {
        cout << __PRETTY_FUNCTION__ << endl;

        ResponseBuilder(downstream_)
            .status(200, "OK")
            .header("Content-type", "text/plain")
            .body(move(m_body))
            .sendWithEOM();
    }


    void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override
    {
        cout << __PRETTY_FUNCTION__ << endl;
    }


    void requestComplete() noexcept override
    {
        cout << __PRETTY_FUNCTION__ << endl;
        delete this;
    }

    void onError(proxygen::ProxygenError err) noexcept override
    {
        cout << __PRETTY_FUNCTION__ << endl;
    }
protected:
    unique_ptr<folly::IOBuf> m_body;
    static string s_defaultBody;

};

string MyHandler::s_defaultBody("It works!\n");

class MyHandlerFactory : public RequestHandlerFactory {
public:
    void onServerStart(folly::EventBase* evb) noexcept override {

    }

    void onServerStop() noexcept override {
    }

    RequestHandler* onRequest(RequestHandler*, HTTPMessage*) noexcept override {
        return new MyHandler();
    }

private:

};

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
        .addThen<MyHandlerFactory>()
        .build();


    HTTPServer server(std::move(options));

    server.bind(IPs);

    std::thread t([&] () {
            server.start();
        });

    t.join();

    return 0;
}
