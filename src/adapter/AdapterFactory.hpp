#ifndef FRG_AdapterFactory_hpp
#define FRG_AdapterFactory_hpp

#include <cstdio>
#include <string>
#include <list>
#include <iostream>
#include <fstream>

#include "stream/UploadSession.hpp"
#include "stream/DownloadFeed.hpp"


class AdapterFactory {
private:
    std::string m_dir;

public:
    AdapterFactory() = default;
    AdapterFactory(const std::string& dir)
    : m_dir(dir)
    {};
    ~AdapterFactory() = default;

public:
    static std::shared_ptr<AdapterFactory> createShared();
    static std::shared_ptr<AdapterFactory> createShared(const std::string& dir);

public:
    virtual std::shared_ptr<UploadSession> createUploadSession(const char* filename);
    virtual std::shared_ptr<DownloadFeed> createDownloadFeed(const std::string& key);

};

#endif /* FRG_AdapterFactory_hpp */