#pragma once

#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/httpserver/ResponseBuilder.h>

class ReverseProxyHandlerFactory : public proxygen::RequestHandlerFactory {
public:
    ReverseProxyHandlerFactory(const std::string &redirectHost, int redirectPort) : m_redirectHost(redirectHost), m_redirectPort(redirectPort) {}
    void onServerStart(folly::EventBase* evb) noexcept override {
        m_evb = evb;

        m_timer = folly::HHWheelTimer::newTimer(evb,
                                                std::chrono::milliseconds(folly::HHWheelTimer::DEFAULT_TICK_INTERVAL),
                                                folly::AsyncTimeout::InternalEnum::NORMAL,
                                                std::chrono::milliseconds(5000));
    }

    void onServerStop() noexcept override {
    }

    proxygen::RequestHandler* onRequest(proxygen::RequestHandler*, proxygen::HTTPMessage*) noexcept override;

    folly::EventBase *evb() const {  return m_evb; }
    folly::HHWheelTimer *wheelTimer() const { return m_timer.get(); }

    const std::string &redirectHost() const { return m_redirectHost; }
    int redirectPort() const { return m_redirectPort; }

private:
    folly::EventBase *m_evb = nullptr;
    folly::HHWheelTimer::UniquePtr m_timer;

    std::string m_redirectHost;
    int m_redirectPort;
};
