#include <proxygen/lib/http/HTTPHeaders.h>
#include <proxygen/lib/http/session/HTTPUpstreamSession.h>
#include <folly/io/async/AsyncSocket.h>

#include "ReverseProxyHandler.h"
#include "ReverseProxyHandlerFactory.h"
#include <iostream>

using namespace std;
using namespace proxygen;
using namespace folly;

string ReverseProxyHandler::s_body400string("No Host header\n");

ReverseProxyHandler::ReverseProxyHandler(ReverseProxyHandlerFactory *factory) :
    m_factory(factory),
    m_httpConnector(this, factory->wheelTimer()),
    m_body400(new folly::IOBuf(folly::IOBuf::CopyBufferOp::COPY_BUFFER, s_body400string.data(), s_body400string.size())),
    m_upstreamHandler(this)
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::onRequest(std::unique_ptr<HTTPMessage> message) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;

    HTTPHeaders &headers = message->getHeaders();

    if (headers.getNumberOfValues(HTTP_HEADER_HOST) != 1) {
        ResponseBuilder(downstream_)
            .status(400, "Bad request")
            .header("Content-type", "text/plain")
            .body(move(m_body400))
            .sendWithEOM();
    }

    std::string host = headers.getSingleOrEmpty(HTTP_HEADER_HOST);

    // TODO - parse host header value as <host>:<port>
    // TODO - implement configurable remapping
    SocketAddress addr(host, 8895, true);

    static const AsyncSocket::OptionMap opts{{{SOL_SOCKET, SO_REUSEADDR}, 1}};

    m_httpConnector.connect
        (m_factory->evb(),
         addr,
         std::chrono::milliseconds(1000),
         opts);
}

void ReverseProxyHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::onEOM() noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::onUpgrade(UpgradeProtocol proto) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::requestComplete() noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::onError(ProxygenError err) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}


// HTTPConnector::Callback methods
void ReverseProxyHandler::connectSuccess(HTTPUpstreamSession* session) noexcept
{
    m_upstreamTransaction = session->newTransaction(&m_upstreamHandler);
}

void ReverseProxyHandler::connectError(const folly::AsyncSocketException& ex) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}

// HTTPTransactionHandler methods
void ReverseProxyHandler::upstreamSetTransaction(HTTPTransaction* txn) noexcept
{

}

void ReverseProxyHandler::upstreamDetachTransaction() noexcept
{

}

void ReverseProxyHandler::upstreamOnHeadersComplete(std::unique_ptr<HTTPMessage> msg)
{

}

void ReverseProxyHandler::upstreamOnBody(std::unique_ptr<folly::IOBuf> chain) noexcept
{

}

void ReverseProxyHandler::upstreamOnChunkHeader(size_t length) noexcept
{

}

void ReverseProxyHandler::upstreamOnChunkComplete() noexcept
{

}

void ReverseProxyHandler::upstreamOnTrailers(std::unique_ptr<HTTPHeaders> trailers) noexcept
{

}

void ReverseProxyHandler::upstreamOnEOM() noexcept
{

}

void ReverseProxyHandler::upstreamOnUpgrade(UpgradeProtocol protocol) noexcept
{

}

void ReverseProxyHandler::upstreamOnError(const HTTPException& error) noexcept
{

}

void ReverseProxyHandler::upstreamOnEgressPaused() noexcept
{

}

void ReverseProxyHandler::upstreamOnEgressResumed() noexcept
{

}

void ReverseProxyHandler::upstreamOnPushedTransaction(HTTPTransaction* txn) noexcept
{

}
