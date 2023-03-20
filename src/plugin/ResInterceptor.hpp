#ifndef FRG_ResInterceptor_hpp
#define FRG_ResInterceptor_hpp

#include "oatpp/web/server/interceptor/ResponseInterceptor.hpp"


class ResInterceptor : public oatpp::web::server::interceptor::ResponseInterceptor {
    typedef oatpp::web::protocol::http::incoming::Request IncomingRequest;
    typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;
    
public:
    virtual std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request,
                                                      const std::shared_ptr<OutgoingResponse>& response) override;
    
};

#endif /* FRG_ResInterceptor_hpp */