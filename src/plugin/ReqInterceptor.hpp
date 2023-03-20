#ifndef FRG_ReqInterceptor_hpp
#define FRG_ReqInterceptor_hpp

#include "oatpp/web/server/interceptor/RequestInterceptor.hpp"


class ReqInterceptor : public oatpp::web::server::interceptor::RequestInterceptor {
    typedef oatpp::web::protocol::http::incoming::Request IncomingRequest;
    typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;
    
public:
    virtual std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request) override;
    
};

#endif /* FRG_ReqInterceptor_hpp */