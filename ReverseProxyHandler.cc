#include "ReverseProxyHandler.h"
#include <iostream>

using namespace std;

ReverseProxyHandler::ReverseProxyHandler(ReverseProxyHandlerFactory *factory) : m_factory(factory)
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept
{
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

void ReverseProxyHandler::onUpgrade(proxygen::UpgradeProtocol proto) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::requestComplete() noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}

void ReverseProxyHandler::onError(proxygen::ProxygenError err) noexcept
{
    cout << __PRETTY_FUNCTION__ << endl;
}
