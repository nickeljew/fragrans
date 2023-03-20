#include "Downloader.hpp"

#include <stdio.h>
#include <string>

#include "oatpp/web/protocol/http/outgoing/Response.hpp"


using Action = oatpp::async::AbstractCoroutine::Action;
typedef oatpp::web::protocol::http::outgoing::Response Response;


Action
Downloader::GetFile::act()
{
    auto key = request->getPathVariable("key");
    
    auto feed = controller->m_factory->createDownloadFeed(key);
    if (!feed->feedable()) {
        return _return(controller->createResponse(Status::CODE_404, "Key not found"));
    }

    auto response = Response::createShared(Status::CODE_200, feed->body());
    response->putHeader(Header::CONNECTION, Header::Value::CONNECTION_KEEP_ALIVE);
    response->putHeader(Header::CONTENT_TYPE, "application/octet-stream");
    //response->putHeader("Content-Transfer-Encoding", "binary");
    response->putHeader("Content-Disposition", oatpp::String("attachment;filename=") + key);
    //response->putHeader(Header::CONTENT_LENGTH, "...");
    return _return(response);
}
