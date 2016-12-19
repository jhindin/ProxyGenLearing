#include <proxygen/lib/http/HTTPHeaders.h>
#include <proxygen/lib/http/session/HTTPUpstreamSession.h>
#include <folly/io/async/AsyncSocket.h>

#include "ReverseProxyHandler.h"
#include "ReverseProxyHandlerFactory.h"
#include <iostream>

using namespace std;
using namespace proxygen;
using namespace folly;

string ReverseProxyHandler::s_body400stringNoHostHeader("No Host header\n");
string ReverseProxyHandler::s_body400stringBadHostHeader("Bad Host header\n");
string ReverseProxyHandler::s_body502string("Server doesn't response\n");

ReverseProxyHandler::ReverseProxyHandler(ReverseProxyHandlerFactory *factory) :
    m_factory(factory),
    m_httpConnector(this, factory->wheelTimer()),
    m_body400NoHostHeader(new folly::IOBuf(folly::IOBuf::CopyBufferOp::COPY_BUFFER, s_body400stringNoHostHeader.data(), s_body400stringNoHostHeader.size())),
    m_body400BadHostHeader(new folly::IOBuf(folly::IOBuf::CopyBufferOp::COPY_BUFFER, s_body400stringBadHostHeader.data(), s_body400stringBadHostHeader.size())),
    m_body502(new folly::IOBuf(folly::IOBuf::CopyBufferOp::COPY_BUFFER, s_body502string.data(), s_body502string.size())),
    m_upstreamHandler(this)
{
    cout << __PRETTY_FUNCTION__ << endl;
}

// RequestHandler methods
void ReverseProxyHandler::onRequest(std::unique_ptr<HTTPMessage> message) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;

    m_message = std::move(message);

    HTTPHeaders &headers = m_message->getHeaders();

    if (headers.getNumberOfValues(HTTP_HEADER_HOST) != 1) {
        ResponseBuilder(downstream_)
            .status(400, "Bad request")
            .header("Content-type", "text/plain")
            .body(move(m_body400NoHostHeader))
            .sendWithEOM();
    }

    std::string host = headers.getSingleOrEmpty(HTTP_HEADER_HOST);

    // TODO - implement configurable remapping and host name resolution

    try {
        SocketAddress addr;

        // TODO: port hardcoded for testing!!
        if (host.find(':') == string::npos)
            addr.setFromHostPort(host, 8895);
        else
            addr.setFromHostPort(host);

        static const AsyncSocket::OptionMap opts{{{SOL_SOCKET, SO_REUSEADDR}, 1}};

        m_httpConnector.connect
            (m_factory->evb(),
             addr,
             std::chrono::milliseconds(1000),
             opts);
    } catch (system_error &er) {
        ResponseBuilder(downstream_)
            .status(400, "Bad request")
            .header("Content-type", "text/plain")
            .body(move(m_body400BadHostHeader))
            .sendWithEOM();
        return;
    }
}

void ReverseProxyHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
    if (m_upstreamTransaction)
        m_upstreamTransaction->sendBody(std::move(body));
    else if (m_requestBody)
        m_requestBody->prependChain(std::move(body));
    else
        m_requestBody = std::move(body);
}

void ReverseProxyHandler::onEOM() noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
    if (m_upstreamTransaction)
        m_upstreamTransaction->sendEOM();
    else
        m_requestEOM = true;
}

void ReverseProxyHandler::onUpgrade(UpgradeProtocol proto) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::requestComplete() noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
    // TODO - make the instance delete itself only when both upstream and downstream
    // transactions are closed
}

void ReverseProxyHandler::onError(ProxygenError err) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::onEgressPaused() noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::onEgressResumed() noexcept
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
    ResponseBuilder(downstream_)
        .status(502, "Bad gateway")
        .header("Content-type", "text/plain")
        .body(move(m_body502))
        .sendWithEOM();
}

// HTTPTransactionHandler methods
void ReverseProxyHandler::upstreamSetTransaction(HTTPTransaction* txn) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;

    m_upstreamTransaction = txn;

    m_upstreamTransaction->setIdleTimeout(chrono::milliseconds(60000));

    HTTPHeaderSize headersSize;
    HTTPMessage *message = m_message.get();
    m_upstreamTransaction->sendHeaders(*message);

    if (m_requestBody)
        m_upstreamTransaction->sendBody(std::move(m_requestBody));

    if (m_requestEOM)
        m_upstreamTransaction->sendEOM();
}

void ReverseProxyHandler::upstreamDetachTransaction() noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
    // TODO - make the instance delete itself only when both upstream and downstream
    // transactions are closed
}

void ReverseProxyHandler::upstreamOnHeadersComplete(std::unique_ptr<HTTPMessage> msg)
{
    cout << __PRETTY_FUNCTION__ << endl;
    downstream_->sendHeaders(*(msg.get()));
}

void ReverseProxyHandler::upstreamOnBody(std::unique_ptr<folly::IOBuf> chain) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;

    downstream_->sendBody(std::move(chain));
}

void ReverseProxyHandler::upstreamOnChunkHeader(size_t length) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
    downstream_->sendChunkHeader(length);
}

void ReverseProxyHandler::upstreamOnChunkComplete() noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
    downstream_->sendChunkTerminator();
}

void ReverseProxyHandler::upstreamOnTrailers(std::unique_ptr<HTTPHeaders> trailers) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::upstreamOnEOM() noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
    downstream_->sendEOM();
}

void ReverseProxyHandler::upstreamOnUpgrade(UpgradeProtocol protocol) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;

    if (protocol == proxygen::UpgradeProtocol::TCP) {

        ResponseBuilder(downstream_).acceptUpgradeRequest(ResponseBuilder::UpgradeType::HTTP_UPGRADE);

        cout << "upstream upgrade to TCP" << endl;
        m_upgradedToTcp = true;
    }
}

void ReverseProxyHandler::upstreamOnError(const HTTPException& error) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
    cout << "  Direction: " << static_cast<int>(error.getDirection()) <<
        ", message: "  << error.describe()  << endl;
}

void ReverseProxyHandler::upstreamOnEgressPaused() noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
    m_upstreamTransaction->pauseIngress();
}

void ReverseProxyHandler::upstreamOnEgressResumed() noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
    m_upstreamTransaction->resumeIngress();
}

void ReverseProxyHandler::upstreamOnPushedTransaction(HTTPTransaction* txn) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}
