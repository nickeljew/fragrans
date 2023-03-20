#ifndef FRG_Uploader_hpp
#define FRG_Uploader_hpp

#include <map>
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "dto/UploadDTO.hpp"
#include "stream/UploadSession.hpp"
#include "adapter/AdapterFactory.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class Uploader : public oatpp::web::server::api::ApiController {
private:
    typedef Uploader __ControllerType;
    std::shared_ptr<AdapterFactory> m_factory;
    std::map<std::string, std::shared_ptr<UploadSession>> m_sessions;

public:
    Uploader(
        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper),
        OATPP_COMPONENT(std::shared_ptr<AdapterFactory>, factory)
    )
        : oatpp::web::server::api::ApiController(objectMapper)
        , m_factory(factory)
    {}
    ~Uploader();
    

public:
    ENDPOINT_INFO(Startup)
    {
        info->summary = "Start up a new uploading session";
        info->addConsumes<oatpp::Object<SessionInit>>("application/json");
        info->addResponse<oatpp::Object<SessionStart>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_500, "text/plain");
    }
    ENDPOINT_ASYNC("POST", "/upload/create-session", Startup)
    {
        ENDPOINT_ASYNC_INIT(Startup)

        Action act() override;
        Action createUploadSessionInAutoMode(const oatpp::Object<SessionInit>& body);
    };

    ENDPOINT_INFO(Endup)
    {
        info->summary = "End up the uploading session";
        info->addConsumes<oatpp::Object<SessionToFinish>>("application/json");
        info->addResponse<oatpp::Object<SessionEnd>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_400, "text/plain");
    }
    ENDPOINT_ASYNC("POST", "/upload/close-session", Endup)
    {
        ENDPOINT_ASYNC_INIT(Endup)

        Action act() override;
        Action closeUploadSession(const oatpp::Object<SessionToFinish>& body);
    };

    ENDPOINT_INFO(Uploading)
    {
        info->summary = "Upload one part of a file";
        //info->headers["userId"].description = "User Identifier";
        //info->pathParams["partNumber"].description = "Number of the part";
        info->pathParams.add<UInt32>("partNumber").description = "Number of the part";
        info->queryParams["sessionId"].description = "Session ID";
        info->addConsumes<oatpp::Void>("application/octet-stream");
        info->addResponse<oatpp::Object<SessionProgress>>(Status::CODE_200, "application/json");
        info->addResponse<String>(Status::CODE_400, "text/plain");
    }
    ENDPOINT_ASYNC("PUT", "/upload/part/{partNumber}", Uploading)
    {
        ENDPOINT_ASYNC_INIT(Uploading)

        std::shared_ptr<UploadSession> m_session;
        std::shared_ptr<UploadSession::UploadPart> m_part;

        Action act() override;
        Action handleUploadPart();
    };
    
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* FRG_Uploader_hpp */