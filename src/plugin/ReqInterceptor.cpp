#include "ReqInterceptor.hpp"


typedef oatpp::web::protocol::http::incoming::Request IncomingRequest;
typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;

std::shared_ptr<OutgoingResponse> 
ReqInterceptor::intercept(const std::shared_ptr<IncomingRequest>& request) {
    OATPP_LOGV("ReqInterceptor::intercept", "path: %s", request->getStartingLine().path.std_str().c_str());
    auto headers = request->getHeaders().getAll();
    for (auto& pair : headers) {
        OATPP_LOGV("ReqInterceptor::intercept", "%s: %s", pair.first.std_str().c_str(), pair.second.std_str().c_str());
    }
    return nullptr;
}
