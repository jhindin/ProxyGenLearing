#pragma once

#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <proxygen/lib/http/HTTPConnector.h>
#include <proxygen/lib/http/session/HTTPTransaction.h>

class ReverseProxyHandlerFactory;

class ReverseProxyHandler : public proxygen::RequestHandler,
    public proxygen::HTTPConnector::Callback
{
public:
    ReverseProxyHandler(ReverseProxyHandlerFactory *factory);

    // RequestHandler methods
    void onRequest(std::unique_ptr<proxygen::HTTPMessage> message) noexcept override;
    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;
    void onEOM() noexcept override;
    void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;
    void requestComplete() noexcept override;
    void onError(proxygen::ProxygenError err) noexcept override;
    void onEgressPaused() noexcept override;
    void onEgressResumed() noexcept override;

    // HTTPConnector::Callback methods
    void connectSuccess(proxygen::HTTPUpstreamSession* session) noexcept override;
    void connectError(const folly::AsyncSocketException& ex) noexcept override;

    // HTTPTransactionHandler delegated methods

    void upstreamSetTransaction(proxygen::HTTPTransaction* txn) noexcept;
    void upstreamDetachTransaction() noexcept;
    void upstreamOnHeadersComplete(std::unique_ptr<proxygen::HTTPMessage> msg);
    void upstreamOnBody(std::unique_ptr<folly::IOBuf> chain) noexcept;
    void upstreamOnChunkHeader(size_t length) noexcept;
    void upstreamOnChunkComplete() noexcept;
    void upstreamOnTrailers(std::unique_ptr<proxygen::HTTPHeaders> trailers) noexcept;
    void upstreamOnEOM() noexcept;
    void upstreamOnUpgrade(proxygen::UpgradeProtocol protocol) noexcept;
    void upstreamOnError(const proxygen::HTTPException& error) noexcept;
    void upstreamOnEgressPaused() noexcept;
    void upstreamOnEgressResumed() noexcept;
    void upstreamOnPushedTransaction(proxygen::HTTPTransaction *txn) noexcept;
protected:
    ReverseProxyHandlerFactory *m_factory;
    proxygen::HTTPConnector m_httpConnector;

    std::unique_ptr<folly::IOBuf> m_body400;
    static std::string s_body400string;

    std::unique_ptr<folly::IOBuf> m_body502;
    static std::string s_body502string;

    proxygen::HTTPTransaction* m_upstreamTransaction = nullptr;
    std::unique_ptr<proxygen::HTTPMessage> m_message;
    std::unique_ptr<folly::IOBuf> m_requestBody;
    bool m_requestEOM = false;

    class UpstreamTransactionHandler :  public proxygen::HTTPTransactionHandler {
    public:
        UpstreamTransactionHandler(ReverseProxyHandler *outerClass) : m_outerClass(outerClass) {}
        // HTTPTransactionHandler methods

        void setTransaction(proxygen::HTTPTransaction* txn) noexcept {
            m_outerClass->upstreamSetTransaction(txn);
        }

        void detachTransaction() noexcept {
            m_outerClass->upstreamDetachTransaction();
        }

        void onHeadersComplete(std::unique_ptr<proxygen::HTTPMessage> msg) noexcept {
            m_outerClass->upstreamOnHeadersComplete(std::move(msg));
        }

        void onBody(std::unique_ptr<folly::IOBuf> chain) noexcept {
            m_outerClass->upstreamOnBody(std::move(chain));
        }

        void onChunkHeader(size_t length) noexcept {
            m_outerClass->upstreamOnChunkHeader(length);
        }

        void onChunkComplete() noexcept {
            m_outerClass->upstreamOnChunkComplete();
        }

        void onTrailers(std::unique_ptr<proxygen::HTTPHeaders> trailers) noexcept {
            m_outerClass->upstreamOnTrailers(std::move(trailers));
        }

        void onEOM() noexcept {
            m_outerClass->upstreamOnEOM();
        }

        void onUpgrade(proxygen::UpgradeProtocol protocol) noexcept {
            m_outerClass->upstreamOnUpgrade(protocol);
        }

        void onError(const proxygen::HTTPException& error) noexcept {
            m_outerClass->upstreamOnError(error);
        }

        void onEgressPaused() noexcept {
            m_outerClass->upstreamOnEgressPaused();
        }

        void onEgressResumed() noexcept {
            m_outerClass->upstreamOnEgressResumed();
        }

        void onPushedTransaction(proxygen::HTTPTransaction *txn) noexcept {
            m_outerClass->upstreamOnPushedTransaction(txn);
        }

        ReverseProxyHandler *m_outerClass;

    };

    UpstreamTransactionHandler m_upstreamHandler;

};
