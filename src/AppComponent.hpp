#ifndef FRG_AppComponent_hpp
#define FRG_AppComponent_hpp

#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"

#include "./plugin/ReqInterceptor.hpp"
#include "./plugin/ResInterceptor.hpp"
#include "adapter/AdapterFactory.hpp"

class AppComponent {
public:
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)([] {
        return std::make_shared<oatpp::async::Executor>(
            4 /* Data-Processing threads */,
            1 /* I/O threads */,
            1 /* Timer threads */
        );
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([] {
        return oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", 8000});
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
        return oatpp::web::server::HttpRouter::createShared();
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
        OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);

        auto handler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);
        auto reqInter = std::make_shared<ReqInterceptor>();
        handler->addRequestInterceptor(reqInter);
        auto resInter = std::make_shared<ResInterceptor>();
        handler->addResponseInterceptor(resInter);
        return handler;
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([] {
        return oatpp::parser::json::mapping::ObjectMapper::createShared();
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<AdapterFactory>, adapterFactory)([] {
        return AdapterFactory::createShared();
    }());
};

#endif /* FRG_AppComponent_hpp */