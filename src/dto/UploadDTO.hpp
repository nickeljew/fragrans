#ifndef FRG_UploadDTO_hpp
#define FRG_UploadDTO_hpp

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class SessionInit: public oatpp::DTO {

    DTO_INIT(SessionInit, DTO)

    DTO_FIELD(String, filename);
};

class SessionStart: public oatpp::DTO {

    DTO_INIT(SessionStart, DTO)

    DTO_FIELD(String, sessionId);
};


class SessionToFinish: public oatpp::DTO {

    DTO_INIT(SessionToFinish, DTO)

    DTO_FIELD(String, sessionId);
    DTO_FIELD(List<String>, results);
};

class SessionEnd: public oatpp::DTO {

    DTO_INIT(SessionEnd, DTO)

    DTO_FIELD(String, key);
};


class SessionProgress: public oatpp::DTO {

    DTO_INIT(SessionProgress, DTO)

    DTO_FIELD(String, resultTag);
};

#include OATPP_CODEGEN_END(DTO)

#endif /* FRG_UploadDTO_hpp */