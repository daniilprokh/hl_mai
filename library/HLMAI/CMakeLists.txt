cmake_minimum_required(VERSION 3.16)

project(HLMAI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(REDISCPP_FLAGS "-DREDISCPP_HEADER_ONLY=ON")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -std=c++17 ${REDISCPP_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -DNDEBUG ${REDISCPP_FLAGS}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(CppKafka REQUIRED)
find_package(MySQL REQUIRED)
find_package(Poco REQUIRED COMPONENTS 
    Data
    DataMySQL
    JSON
    XML
    Util
    Net
)

add_library(${PROJECT_NAME} STATIC)

set(PUBLIC_HEADERS
  include/HLMAI/database/chat_user/chat_user_feature.h
  include/HLMAI/database/chat_user/chat_user_converter.h
  include/HLMAI/database/chat_user/chat_user_table.h
  include/HLMAI/database/chat_user/chat_user.h
  include/HLMAI/database/group_chat/group_chat_feature.h
  include/HLMAI/database/group_chat/group_chat_converter.h
  include/HLMAI/database/group_chat/group_chat_message_table.h
  include/HLMAI/database/group_chat/group_chat_table.h
  include/HLMAI/database/group_chat/group_chat.h
  include/HLMAI/database/message/message_feature.h
  include/HLMAI/database/message/message_converter.h
  include/HLMAI/database/message/message_table.h
  include/HLMAI/database/message/message.h
  include/HLMAI/database/p2p_chat/p2p_chat_feature.h
  include/HLMAI/database/p2p_chat/p2p_chat_converter.h
  include/HLMAI/database/p2p_chat/p2p_chat_message_table.h
  include/HLMAI/database/p2p_chat/p2p_chat_table.h
  include/HLMAI/database/p2p_chat/p2p_chat.h
  include/HLMAI/database/user/user.h
  include/HLMAI/database/user/user_feature.h
  include/HLMAI/database/user/user_converter.h
  include/HLMAI/database/user/user_table.h
  include/HLMAI/database/user/user.h
  include/HLMAI/database/database.h
  include/HLMAI/database/row_converter.h
  include/HLMAI/database/table.h
  include/HLMAI/cache.h
  include/HLMAI/check_html_form.h
  include/HLMAI/config.h
  include/HLMAI/contains_substr.h
  include/HLMAI/http_web_server.h
  include/HLMAI/identity.h
  include/HLMAI/singleton.h
)

set(SOURCES ${PUBLIC_HEADERS}
  src/database/chat_user/chat_user_converter.cpp
  src/database/chat_user/chat_user_table.cpp
  src/database/group_chat/group_chat_converter.cpp
  src/database/group_chat/group_chat_table.cpp
  src/database/message/message_converter.cpp
  src/database/message/message_table.cpp
  src/database/p2p_chat/p2p_chat_converter.cpp
  src/database//p2p_chat/p2p_chat_table.cpp
  src/database/user/user_converter.cpp
  src/database/user/user_table.cpp
  src/database/database.cpp
  src/cache.cpp
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

target_include_directories(${PROJECT_NAME} 
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    ${Poco_INCLUDE_DIRS}
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

set_target_properties(${PROJECT_NAME} PROPERTIES
  PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

target_link_libraries(${PROJECT_NAME}
    CppKafka::cppkafka
    Poco::Data
    Poco::DataMySQL
    Poco::JSON
    Poco::Util
    Poco::Net
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        # If the debug configuration pass the DEBUG define to the compiler 
        $<$<CONFIG:Debug>:DEBUG>
)
