#pragma once

#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <proxygen/httpserver/ResponseBuilder.h>

class ReverseProxyHandlerFactory : public proxygen::RequestHandlerFactory {
public:
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

private:
    folly::EventBase *m_evb = nullptr;
    folly::HHWheelTimer::UniquePtr m_timer;
};
