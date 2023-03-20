#include "ResInterceptor.hpp"


typedef oatpp::web::protocol::http::incoming::Request IncomingRequest;
typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;

std::shared_ptr<OutgoingResponse> 
ResInterceptor::intercept(const std::shared_ptr<IncomingRequest>& request,
    const std::shared_ptr<OutgoingResponse>& response)
{
    OATPP_LOGV("ResInterceptor::intercept", "status: %d", response->getStatus().code);
    return response;
}
