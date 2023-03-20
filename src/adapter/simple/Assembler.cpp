#include <filesystem>
#include <iostream>
#include <fstream>

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/core/data/stream/Stream.hpp"

#include "const.hpp"
#include "Assembler.hpp"

namespace fs = std::filesystem;
using std::string;


// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}


class PartReader : public oatpp::data::stream::ReadCallback {
private:
    std::string m_dir;
    std::list<std::string> m_tags;
    std::list<std::string>::iterator m_itr;
    std::ifstream* m_file;
    uint64_t m_partCount;
    uint16_t m_segCountInPart;

public:
    PartReader(const string& dir, const string& metaFile);
    ~PartReader();

public:
    static std::shared_ptr<PartReader> createShared(const string& dir, const string& metaFile);

public:
    /**
     * Proxy method to readCallback::read().
     * @param buffer - pointer to buffer.
     * @param count - size of the buffer in bytes.
     * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
     * caller MUST return this action on coroutine iteration.
     * @return - actual number of bytes written to buffer. 0 - to indicate end-of-file.
     */
    oatpp::v_io_size read(void *buffer, v_buff_size count, oatpp::async::Action& action) override;

private:
    bool readTags(const std::string& tagsPath);
};


PartReader::PartReader(const string& dir, const string& metaFile)
    : m_dir(dir)
    , m_file(nullptr)
    , m_partCount(0)
    , m_segCountInPart(0)
    , m_itr(m_tags.end())
{
    auto metaFilePath = m_dir + metaFile;
    //OATPP_LOGD("PartReader::PartReader", "reading meta at: '%s'", metaFilePath.c_str());
    if (!readTags(metaFilePath)) {
        OATPP_LOGE("PartReader::PartReader", "file not found '%s'", metaFilePath.c_str());
    }
    else {
        m_itr = m_tags.begin();
    }
}

PartReader::~PartReader()
{
   if (m_file != nullptr) {
        m_file->close();
        m_file = nullptr;
    }
}


std::shared_ptr<PartReader>
PartReader::createShared(const string& dir, const string& metaFile)
{
    OATPP_LOGD("PartReader::createShared", "about to read %s at %s", metaFile.c_str(), dir.c_str());
    return std::make_shared<PartReader>(dir, metaFile);
}

bool
PartReader::readTags(const std::string& tagsPath)
{
    std::ifstream file(tagsPath);
    if (!file.is_open()) {
        return false;
    }
    string line;
    while (getline(file, line)) {
        trim(line);
        if (line.length() > 0) {
            OATPP_LOGD("PartReader::readTags", "read a tag: %s", line.c_str());
            m_tags.push_back(line);
        }
    }
    file.close();
    OATPP_LOGD("PartReader::readTags", "read %d tags", m_tags.size());
    return true;
}


oatpp::v_io_size
PartReader::read(void *buffer, v_buff_size count, oatpp::async::Action& action)
{
    oatpp::v_io_size readCount = 0;
    do
    {
        if (m_file == nullptr) {
            if (m_itr == m_tags.end()) {
                return readCount;
            }

            auto path = m_dir + "/" + *m_itr;
            m_file = new std::ifstream(path, std::ios::binary);
            if (!m_file->good()) {
                OATPP_LOGE("PartReader::read", "file not found '%s'", path.c_str());
                return 0;
            }
            m_partCount = 0;
            m_segCountInPart = 0;
        }

        m_file->read((char*)buffer, count);
        readCount = m_file->gcount();
        if (readCount > 0) {
            m_partCount += readCount;
            m_segCountInPart++;
        }

        if (m_file->eof()) {
            OATPP_LOGD("PartReader::read", "read %d byte(s) from %s (%d, %d)", m_partCount, m_itr->c_str(), readCount, m_segCountInPart);
            m_file->close();
            m_file = nullptr;
            m_itr++;
        }
    }
    while (m_file == nullptr);
    return readCount;
}




Assembler::Assembler(const std::string & dir, const std::string & key)
    : m_dir(dir + STORAGE_DIR + key)
    , m_feedable(false)
    , m_body(nullptr)
{
    if (fs::is_directory(fs::status(m_dir))) {
        m_feedable = true;
        m_body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>(PartReader::createShared(m_dir, TAGS_DB_FILE));
    }
}

std::shared_ptr<Assembler>
Assembler::createShared(const std::string & dir, const std::string & key)
{
    return std::make_shared<Assembler>(dir, key);
}
