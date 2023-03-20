cmake_minimum_required(VERSION 3.13)

set(project_name fragrans)

project(${project_name} VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_library(${project_name}-lib
    src/AppComponent.hpp
    src/plugin/ReqInterceptor.cpp
    src/plugin/ReqInterceptor.hpp
    src/plugin/ResInterceptor.cpp
    src/plugin/ResInterceptor.hpp
    src/controller/Uploader.cpp
    src/controller/Uploader.hpp
    src/controller/Downloader.cpp
    src/controller/Downloader.hpp
    src/dto/UploadDTO.hpp
    #src/stream/UploadSession.cpp
    src/stream/UploadSession.hpp
    #src/stream/DownloadFeed.cpp
    src/stream/DownloadFeed.hpp
    src/adapter/AdapterFactory.cpp
    src/adapter/AdapterFactory.hpp
    src/adapter/simple/FillingSession.cpp
    src/adapter/simple/FillingSession.hpp
    src/adapter/simple/Assembler.cpp
    src/adapter/simple/Assembler.hpp
)

find_package(oatpp 1.3.0 REQUIRED)

target_link_libraries(${project_name}-lib
    PUBLIC oatpp::oatpp
    #PUBLIC oatpp::oatpp-test
)

add_executable(${project_name}
    #main.cpp
    src/App.cpp
)
target_link_libraries(${project_name} ${project_name}-lib)
add_dependencies(${project_name} ${project_name}-lib)

set_target_properties(${project_name}-lib ${prjoect_name} PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
    LINKER_LANGUAGE CXX
)

