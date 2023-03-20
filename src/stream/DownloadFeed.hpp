#ifndef FRG_DownloadFeed_hpp
#define FRG_DownloadFeed_hpp

#include <cstdio>
#include <string>
#include <list>
#include <iostream>
#include <fstream>

#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"


class  DownloadFeed {

public:
    virtual const std::shared_ptr<oatpp::web::protocol::http::outgoing::StreamingBody> body() = 0;
    virtual bool feedable() const = 0;

};

#endif /* FRG_DownloadFeed_hpp */