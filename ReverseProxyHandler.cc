#include <proxygen/lib/http/HTTPHeaders.h>

#include "ReverseProxyHandler.h"
#include <iostream>

using namespace std;
using namespace proxygen;

string ReverseProxyHandler::s_body400string("No Host header\n");

ReverseProxyHandler::ReverseProxyHandler(ReverseProxyHandlerFactory *factory) :
    m_factory(factory),
    m_body400(new folly::IOBuf(folly::IOBuf::CopyBufferOp::COPY_BUFFER, s_body400string.data(), s_body400string.size()))
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::onRequest(std::unique_ptr<HTTPMessage> message) noexcept
{
    HTTPHeaders &headers = message->getHeaders();

    if (!headers.exists(HTTP_HEADER_HOST)) {
        ResponseBuilder(downstream_)
            .status(400, "Bad request")
            .header("Content-type", "text/plain")
            .body(move(m_body400))
            .sendWithEOM();
    }

    cout << __PRETTY_FUNCTION__ << endl;
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
