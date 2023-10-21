cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(lab_01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_RPATH "/usr/local/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(WEB_SERVICE ${PROJECT_NAME}_web_service)
set(OTHER_SERVICE ${PROJECT_NAME}_other_service)

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS XML Util Net )

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

#include_directories("/usr/include/mysql")
#include_directories("/usr/local/include/mysql")
#link_directories("/usr/local/lib")

set(W_HEADERS 
    web_server/handlers/user_handler.h
    web_server/http_request_factory.h
    web_server/http_web_server.h
)

set(W_SOURCES web_server/main.cpp)

add_executable(${WEB_SERVICE} ${W_HEADERS} ${W_SOURCES})

target_include_directories(${WEB_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${WEB_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${WEB_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${WEB_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${WEB_SERVICE} PRIVATE
    HLMAI
    Poco::XML
    Poco::Util
    Poco::Net
)

set(O_HEADERS 
  other_server/handlers/other_handler.h
  other_server/http_other_request_factory.h
  other_server/http_other_server.h
)

set(O_SOURCES other_server/main.cpp)

add_executable(${OTHER_SERVICE} ${O_HEADERS} ${O_SOURCES})
target_include_directories(${OTHER_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${OTHER_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${OTHER_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${OTHER_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${OTHER_SERVICE} PRIVATE
                             HLMAI 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)
