#ifndef FRG_Assembler_hpp
#define FRG_Assembler_hpp

#include <cstdio>
#include <string>
#include <list>
#include <iostream>
#include <fstream>

#include "stream/DownloadFeed.hpp"


class Assembler : public DownloadFeed {
private:
    std::string m_dir;
    bool m_feedable;
    std::shared_ptr<oatpp::web::protocol::http::outgoing::StreamingBody> m_body;

public:
    Assembler(const std::string & dir, const std::string & key);
    ~Assembler() = default;

public:
    static std::shared_ptr<Assembler> createShared(const std::string & dir, const std::string & key);

public:
    const std::shared_ptr<oatpp::web::protocol::http::outgoing::StreamingBody> body() override {
        return m_body;
    }

    bool feedable() const override {
        return m_feedable;
    }

};

#endif /* FRG_Assembler_hpp */