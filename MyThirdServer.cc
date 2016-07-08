#include <gflags/gflags.h>
#include <folly/Memory.h>
#include <folly/Function.h>
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

DEFINE_int32(http_port, 80, "Port to listen on with HTTP protocol");

class MyHandler : public RequestHandler {
public:
    MyHandler(folly::EventBase *evb) :
        m_body0(new folly::IOBuf(folly::IOBuf::CopyBufferOp::COPY_BUFFER, s_defaultBody0.data(), s_defaultBody0.size())),
        m_body1(new folly::IOBuf(folly::IOBuf::CopyBufferOp::COPY_BUFFER, s_defaultBody1.data(), s_defaultBody1.size())),
        m_evb(evb)
    {
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
            .header("Content-length", to_string(s_defaultBody0.size() + s_defaultBody1.size()))
            .body(move(m_body0))
            .send();


        m_evb->runAfterDelay([this]() { this->sendTail(); }, 1000);

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

    void sendTail() {
        ResponseBuilder(downstream_)
            .body(move(m_body1))
            .sendWithEOM();
    }
protected:
    unique_ptr<folly::IOBuf> m_body0, m_body1;

    folly::EventBase *m_evb;

    static string s_defaultBody0,s_defaultBody1;

};

string MyHandler::s_defaultBody0("It works!\n");
string MyHandler::s_defaultBody1("It really works!\n");

class MyHandlerFactory : public RequestHandlerFactory {
public:
    void onServerStart(folly::EventBase* evb) noexcept override : m_evb(evb) { }

    void onServerStop() noexcept override {
    }

    RequestHandler* onRequest(RequestHandler*, HTTPMessage*) noexcept override {
        return new MyHandler(m_evb);
    }

private:
    folly::EventBase *m_evb;

    HHWheelTimer

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
