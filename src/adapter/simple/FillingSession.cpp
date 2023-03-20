#include <random>
#include <sys/stat.h>
#include <filesystem>

#include "const.hpp"
#include "FillingSession.hpp"

namespace fs = std::filesystem;

using std::string;
using std::random_device;
using std::default_random_engine;

oatpp::data::stream::DefaultInitializedContext FillingSession::DEFAULT_CONTEXT(oatpp::data::stream::StreamType::STREAM_FINITE);

std::string randomStr(u_int16_t len);


FillingSession::FillingPart::FillingPart(uint32_t seq, v_buff_size count, const std::string & dir)
    : UploadSession::UploadPart(seq, count)
    , m_dir(dir)
    , m_tag(randomStr(16))
    , m_filepath(m_dir + "/" + m_tag)
    , m_file(nullptr)
    , m_written(0)
{
    m_file = new std::ofstream(m_filepath, std::ios::binary | std::ios::trunc);
}

FillingSession::FillingPart::~FillingPart()
{
    if (m_file != nullptr) {
        m_file->close();
        delete m_file;
        m_file = nullptr;
    }
}



bool
FillingSession::FillingPart::check() const
{
    struct stat results;
    if (stat(m_filepath.c_str(), &results) == 0) {
        OATPP_LOGD("FillingPart::check", "part seq %d | tag %s | written %d, reading from file %d bytes", m_seq, m_tag.c_str(), m_written, results.st_size);
        return results.st_size == m_written;
    }
    return false;
}


oatpp::v_io_size
FillingSession::FillingPart::write(const void *data, v_buff_size count, oatpp::async::Action& action)
{
    //(void) action;
    // m_data = malloc(count);
    // std::memcpy(m_data, data, count);
    //return std::fwrite(data, 1, count, m_data);

    OATPP_LOGD("FillingPart::write", "part seq %d | tag %s | writing binaries %d bytes", m_seq, m_tag.c_str(), count);
    
    if (m_file->is_open())
    {
        m_file->write((const char*)data, count);
        m_written += count;
        if (m_written >= m_count) {
            m_file->close();
            delete m_file;
            m_file = nullptr;
        }
    }
    return count;
}




FillingSession::FillingSession(const std::string& dir, const char* filename)
    : m_id(randomStr(48))
    , m_filename(filename)
    , m_key(m_id + m_filename.substr(m_filename.find_last_of('.')))
    , m_dir(dir + STORAGE_DIR + m_key)
{
    fs::create_directory(m_dir);
    //fs::permissions(m_dir, fs::perms::others_all, fs::perm_options::remove);
}

FillingSession::~FillingSession() {
    m_parts.clear();
}

FillingSession::FillingSession(FillingSession&& other)
    : m_id(std::move(other.m_id))
    , m_filename(std::move(other.m_filename))
    , m_key(std::move(other.m_key))
    , m_dir(std::move(other.m_dir))
{
    //other.m_filename.clear();
}

FillingSession&
FillingSession::operator=(FillingSession&& other)
{
    m_id = std::move(other.m_id);
    m_filename = std::move(other.m_filename);
    m_key = std::move(other.m_key);
    m_dir = std::move(other.m_dir);

    return *this;
}

std::shared_ptr<FillingSession>
FillingSession::createShared(const std::string& dir, const char* filename)
{
    return std::make_shared<FillingSession>(dir, filename);
}

// oatpp::data::stream::Context& FillingSession::getContext() {
//     return DEFAULT_CONTEXT;
// }

std::shared_ptr<UploadSession::UploadPart>
FillingSession::add(uint32_t seq, v_buff_size count)
{
    auto part = std::make_shared<FillingSession::FillingPart>(seq, count, m_dir);
    m_parts[ part->tag() ] = part;
    //OATPP_LOGD("FillingSession::add", "part %d with tag %s", part->seq(), part->tag().c_str());
    return part;
}

void
FillingSession::fulfill(const oatpp::List<oatpp::String> & tags)
{
    std::ofstream file(m_dir + TAGS_DB_FILE, std::ios::trunc);

    for (auto& tag: *tags) {
        auto ite = m_parts.find(*tag);
        if (ite == m_parts.end()) {
            OATPP_LOGE("FillingSession::fulfill", "unknown tag '%s'", tag->c_str());
        }
        else {
            OATPP_LOGD("FillingSession::fulfill", "input tag '%s' with data %d", tag->c_str(), ite->second->count());
            if (ite->second->check()) {
                file << tag->c_str() << std::endl;
            }
            else {
                OATPP_LOGE("FillingSession::fulfill", "file not found with tag '%s'", tag->c_str());
            }
        }
    }

    file.close();
}



std::string
randomStr(u_int16_t len)
{
    std::string buffer;
    
    random_device rd; // 产生一个 std::random_device 对象 rd
    default_random_engine random(rd()); // 用 rd 初始化一个随机数发生器 random
    
    for (u_int16_t i = 0; i < len; i++) {
        char tmp = random() % 36; // 随机一个小于 36 的整数，0-9、A-Z 共 36 种字符
        if (tmp < 10) {
    		// 如果随机数小于 10，变换成一个阿拉伯数字的 ASCII
            tmp += '0';
        } else {
    		// 否则，变换成一个大写字母的 ASCII
            tmp -= 10;
            tmp += 'a';
        }
        buffer += tmp;
    }
    return buffer;
}
