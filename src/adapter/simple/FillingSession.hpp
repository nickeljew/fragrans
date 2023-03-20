#ifndef FRG_FillingSession_hpp
#define FRG_FillingSession_hpp

#include <cstdio>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "oatpp/core/data/stream/Stream.hpp"

#include "stream/UploadSession.hpp"


class FillingSession : public UploadSession {
public:
  static oatpp::data::stream::DefaultInitializedContext DEFAULT_CONTEXT;

public:
    class FillingPart : public UploadSession::UploadPart {
    private:
        std::string m_dir;
        std::string m_tag;
        std::string m_filepath;
        std::ofstream* m_file;
        v_buff_size m_written;

    public:
        FillingPart(uint32_t seq, v_buff_size count, const std::string & dir);
        ~FillingPart();

    public:
        const std::string & tag() const override { return m_tag; }
        v_buff_size written() const { return m_written; }
        bool check() const;

        oatpp::v_io_size write(const void *data, v_buff_size count, oatpp::async::Action& action) override;
    };

private:
    std::string m_id;
    std::string m_filename;
    std::string m_key;

    std::string m_dir;
    std::map<std::string, std::shared_ptr<FillingPart>> m_parts;

public:
    FillingSession(const std::string & dir, const char* filename);
    ~FillingSession();

    /**
     * Move constructor.
     * @param other
     */
    FillingSession(FillingSession&& other);
    FillingSession& operator=(FillingSession&& other);

public:
    static std::shared_ptr<FillingSession> createShared(const std::string & dir, const char* filename);

public:
    const std::string& id() const override { return m_id; };
    const std::string& key() const override { return m_key; };

    const std::string& filename() const { return m_filename; };

    /**
     * Get stream context.
     * @return
     */
    //oatpp::data::stream::Context& getContext() override;

    std::shared_ptr<UploadSession::UploadPart> add(uint32_t seq, v_buff_size count) override;

    void fulfill(const oatpp::List<oatpp::String> & tags) override;

};

#endif /* FRG_FillingSession_hpp */