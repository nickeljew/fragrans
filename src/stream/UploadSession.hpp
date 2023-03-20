#ifndef FRG_UploadSession_hpp
#define FRG_UploadSession_hpp

#include <cstdio>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "oatpp/core/data/stream/Stream.hpp"


class UploadSession {
public:
  static oatpp::data::stream::DefaultInitializedContext DEFAULT_CONTEXT;

public:
    class UploadPart : public oatpp::data::stream::WriteCallback {
    protected:
        uint32_t m_seq;
        v_buff_size m_count;

    public:
        UploadPart(uint32_t seq, v_buff_size count)
            : m_seq(seq)
            , m_count(count)
        {};
        ~UploadPart() = default;

        uint32_t seq() const { return m_seq; }
        v_buff_size count() const { return m_count; }
        virtual const std::string & tag() const = 0;

    };

public:
    virtual const std::string& id() const = 0;
    virtual const std::string& key() const = 0;

    virtual std::shared_ptr<UploadPart> add(uint32_t seq, v_buff_size count) = 0;
    virtual void fulfill(const oatpp::List<oatpp::String> & tags) = 0;

};

#endif /* FRG_UploadSession_hpp */