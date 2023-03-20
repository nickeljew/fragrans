#ifndef FRG_Downloader_hpp
#define FRG_Downloader_hpp

#include <map>
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "adapter/AdapterFactory.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class Downloader : public oatpp::web::server::api::ApiController {
private:
    typedef Downloader __ControllerType;
    std::shared_ptr<AdapterFactory> m_factory;

public:
    Downloader(
        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper),
        OATPP_COMPONENT(std::shared_ptr<AdapterFactory>, factory)
    )
        : oatpp::web::server::api::ApiController(objectMapper)
        , m_factory(factory)
    {}
    ~Downloader() = default;
    

public:
    ENDPOINT_INFO(GetFile)
    {
        info->summary = "Start up a new uploading session";
        info->pathParams.add<String>("key").description = "Key of file";
        info->addResponse<oatpp::Void>(Status::CODE_200, "application/octet-stream");
        info->addResponse<String>(Status::CODE_404, "text/plain");
        //info->addResponse<String>(Status::CODE_500, "text/plain");
    }
    ENDPOINT_ASYNC("GET", "/download/{key}", GetFile)
    {
        ENDPOINT_ASYNC_INIT(GetFile)

        Action act() override;
    };
    
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* FRG_Downloader_hpp */