#pragma once

#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <proxygen/lib/http/HTTPConnector.h>

class ReverseProxyHandlerFactory;

class ReverseProxyHandler : public proxygen::RequestHandler, public proxygen::HTTPConnector::Callback {
public:
    ReverseProxyHandler(ReverseProxyHandlerFactory *factory);

    // RequestHandler methods
    void onRequest(std::unique_ptr<proxygen::HTTPMessage> message) noexcept override;
    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;
    void onEOM() noexcept override;
    void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;
    void requestComplete() noexcept override;
    void onError(proxygen::ProxygenError err) noexcept override;

    // HTTPConnector::Callback methods
    void connectSuccess(proxygen::HTTPUpstreamSession* session) noexcept override;
    void connectError(const folly::AsyncSocketException& ex) noexcept override;


protected:
    ReverseProxyHandlerFactory *m_factory;
    proxygen::HTTPConnector m_httpConnector;

    std::unique_ptr<folly::IOBuf> m_body400;
    static std::string s_body400string;

};
