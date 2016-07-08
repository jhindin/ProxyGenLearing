#include "ReverseProxyHandlerFactory.h"
#include "ReverseProxyHandler.h"

using namespace proxygen;

RequestHandler* ReverseProxyHandlerFactory::onRequest(RequestHandler*, HTTPMessage*) noexcept
{
    return new ReverseProxyHandler(this);
}
