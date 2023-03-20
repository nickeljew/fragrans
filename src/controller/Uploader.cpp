#include "Uploader.hpp"

#include <stdio.h>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

using Action = oatpp::async::AbstractCoroutine::Action;
typedef oatpp::web::protocol::http::incoming::Request IncomingRequest;


Uploader::~Uploader()
{
    m_sessions.clear();
}

bool
requestWithJsonBody(oatpp::String contentType)
{
    std::string jsonType = "application/json";
    int i = contentType->compare(0, jsonType.size(), jsonType);
    return i == 0;
}

Action
Uploader::Startup::act()
{
    //printf("incoming request to startup");
    auto startingLine = request->getStartingLine();
    auto contentType = request->getHeader("Content-Type");
    bool isJson = requestWithJsonBody(contentType);
    OATPP_LOGI("Uploader::Startup", "incoming request %s [%s] %d", startingLine.path.toString().get(), contentType->c_str(), isJson);
    // return _return(controller->createResponse(Status::CODE_400, "wrong request body"));

    // auto res = request->readBodyToDtoAsync<oatpp::Object<SessionInit>>(
    //     controller->getDefaultObjectMapper()
    // );
    return request->readBodyToDtoAsync<oatpp::Object<SessionInit>>(
        controller->getDefaultObjectMapper()
    ).callbackTo(&Startup::createUploadSessionInAutoMode);
}

Action
Uploader::Startup::createUploadSessionInAutoMode(const oatpp::Object<SessionInit>& body)
{
    auto filename = body->filename->c_str();
    OATPP_LOGI("Uploader::Startup", "creating session with filename '%s'", filename);
    
    auto session = controller->m_factory->createUploadSession(filename);
    if (controller->m_sessions.find(session->id()) != controller->m_sessions.end()) {
        // response with 500, or generated another session ID
    }

    auto dto = SessionStart::createShared();
    dto->sessionId = session->id();
    OATPP_LOGI("Uploader::Startup", "creating session '%s' with key '%s'", dto->sessionId->c_str(), session->key().c_str());
    controller->m_sessions[ session->id() ] = session;
    return _return(
        controller->createDtoResponse(Status::CODE_200, dto)
    );
}


Action
Uploader::Endup::act()
{
    return request->readBodyToDtoAsync<oatpp::Object<SessionToFinish>>(
        controller->getDefaultObjectMapper()
    ).callbackTo(&Endup::closeUploadSession);
}

Action
Uploader::Endup::closeUploadSession(const oatpp::Object<SessionToFinish>& body)
{
    auto startingLine = request->getStartingLine();
    auto sessionId = body->sessionId;
    auto results = body->results;
    OATPP_LOGI("Uploader::Endup", "incoming request %s with sessionId '%s'", startingLine.path.toString().get(), sessionId->c_str());

    auto ite = controller->m_sessions.find(*sessionId);
    if (ite == controller->m_sessions.end()) {
        // response with 400, unknown session
        return _return(
            controller->createResponse(Status::CODE_400, "unknown session")
        );
    }
    auto session = ite->second;
    session->fulfill(results);
    
    auto dto = SessionEnd::createShared();
    dto->key = session->key();
    return _return(
        controller->createDtoResponse(Status::CODE_200, dto)
    );
}


Action
Uploader::Uploading::act()
{
    // auto pathVariables = request->getPathVariables();
    // auto partNum = pathVariables.getVariable("partNumber");
    auto partNumInput = request->getPathVariable("partNumber");
    uint32_t partNum = 0;
    try {
        partNum = std::stoul(partNumInput);
    }
    catch(std::invalid_argument e) {
        OATPP_LOGE("Uploader::Uploading", "received invalid part number: %s", partNumInput->c_str());
    }
    catch (std::out_of_range e) {
        OATPP_LOGE("Uploader::Uploading", "received out-of-range part number: %s", partNumInput->c_str());
    }
    if (partNum == 0) {
        return _return(
            controller->createResponse(Status::CODE_400, "wrong part number")
        );
    }

    auto sessionId = request->getQueryParameter("sessionId");
    if (sessionId == nullptr) {
        OATPP_LOGE("Uploader::Uploading", "received part %d without sessionId", partNum);
        return _return(
            controller->createResponse(Status::CODE_400, "missing session ID")
        );
    }
    OATPP_LOGI("Uploader::Uploading", "received part %d with sessionId %s", partNum, sessionId->c_str());
    
    auto ite = controller->m_sessions.find(*sessionId);
    if (ite == controller->m_sessions.end()) {
        // response with 400, unknown session
        return _return(
            controller->createResponse(Status::CODE_400, "unknown session")
        );
    }
    //OATPP_LOGI("Uploader::Uploading", "found sesesion with key %s", ite->second->key().c_str());
    m_session = ite->second;

    auto contentLen = request->getHeader("Content-Length");
    v_buff_size dataLen = 0;
    try {
        dataLen = std::stoul(contentLen);
    }
    catch(std::invalid_argument e) {
        OATPP_LOGE("Uploader::Uploading", "received invalid content-length: %s", contentLen->c_str());
    }
    catch (std::out_of_range e) {
        OATPP_LOGE("Uploader::Uploading", "received out-of-range content-length: %s", contentLen->c_str());
    }

    OATPP_LOGD("Uploader::Uploading", "found sesesion with key %s", m_session->key().c_str());
    m_part = m_session->add(partNum, dataLen);
    OATPP_LOGD("Uploader::Uploading", "add new part at %d with data-len %d", m_part->seq(), m_part->count());

    return request->transferBodyAsync(m_part)
        .next(yieldTo(&Uploading::handleUploadPart));
}

Action
Uploader::Uploading::handleUploadPart()
{
    OATPP_LOGD("Uploader::handleUploadPart", "saved part");
    auto dto = SessionProgress::createShared();
    dto->resultTag = m_part->tag();
    return _return(
        controller->createDtoResponse(Status::CODE_200, dto)
    );
}
