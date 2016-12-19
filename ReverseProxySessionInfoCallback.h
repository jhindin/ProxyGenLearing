#pragma once

#include <proxygen/lib/http/session/HTTPSession.h>

class ReverseProxySessionInfoCallback : public proxygen::HTTPSession::InfoCallback {
public:
    // Note: you must not start any asynchronous work from onCreate()
    void onCreate(const proxygen::HTTPSession&) override {}
    void onIngressError(const proxygen::HTTPSession&, proxygen::ProxygenError) override {}
    void onIngressEOF() override {}
    void onRead(const proxygen::HTTPSession&, size_t bytesRead) override {}
    void onWrite(const proxygen::HTTPSession&, size_t bytesWritten) override {}
    void onRequestBegin(const proxygen::HTTPSession&) override {}
    void onRequestEnd(const proxygen::HTTPSession&,
                      uint32_t maxIngressQueueSize) override {}
    void onActivateConnection(const proxygen::HTTPSession&) override {}
    void onDeactivateConnection(const proxygen::HTTPSession&) override {}
    // Note: you must not start any asynchronous work from onDestroy()
    void onDestroy(const proxygen::HTTPSession&) override {}
    void onIngressMessage(const proxygen::HTTPSession&,
                                  const proxygen::HTTPMessage&) override {}
    void onIngressLimitExceeded(const proxygen::HTTPSession&) override {}
    void onIngressPaused(const proxygen::HTTPSession&) override {}
    void onTransactionDetached(const proxygen::HTTPSession&) override {}
    void onPingReplySent(int64_t latency) override {}
    void onPingReplyReceived() override {}
    void onSettingsOutgoingStreamsFull(const proxygen::HTTPSession&) override {}
    void onSettingsOutgoingStreamsNotFull(const proxygen::HTTPSession&) override {}
    void onFlowControlWindowClosed(const proxygen::HTTPSession&) override {}
    void onEgressBuffered(const proxygen::HTTPSession&) override {}
    void onEgressBufferCleared(const proxygen::HTTPSession&) override {}
};
