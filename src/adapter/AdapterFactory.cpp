#include <filesystem>

#include "AdapterFactory.hpp"
#include "simple/FillingSession.hpp"
#include "simple/Assembler.hpp"

namespace fs = std::filesystem;


std::shared_ptr<AdapterFactory>
AdapterFactory::createShared()
{
    return std::make_shared<AdapterFactory>(fs::current_path().string());
}

std::shared_ptr<AdapterFactory>
AdapterFactory::createShared(const std::string& dir)
{
    return std::make_shared<AdapterFactory>(dir);
}

std::shared_ptr<UploadSession>
AdapterFactory::createUploadSession(const char* filename)
{
    return FillingSession::createShared(m_dir, filename);
}

std::shared_ptr<DownloadFeed>
AdapterFactory::createDownloadFeed(const std::string& key)
{
    return Assembler::createShared(m_dir, key);
}
