cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(lab_01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_INSTALL_RPATH "/usr/local/lib")
#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(AUTHORIZATION_SERVICE ${PROJECT_NAME}_authorization_service)

set(A_HEADERS
  service/authorization_service/authorization_handler.h
  service/authorization_service/http_authorization_request_factory.h
)

add_executable(${AUTHORIZATION_SERVICE} 
  ${A_HEADERS} 
  service/authorization_service/authorization_service.cpp
)

target_link_libraries(${AUTHORIZATION_SERVICE} PRIVATE HLMAI)

set(GROUP_CHAT_SERVICE ${PROJECT_NAME}_group_chat_service)

set(G_HEADERS
  service/group_chat_service/group_chat_handler.h
  service/group_chat_service/http_group_chat_request_factory.h
)

add_executable(${GROUP_CHAT_SERVICE} 
  ${G_HEADERS} 
  service/group_chat_service/group_chat_service.cpp
)

target_link_libraries(${GROUP_CHAT_SERVICE} PRIVATE HLMAI)

set(P2P_CHAT_SERVICE ${PROJECT_NAME}_p2p_chat_service)

set(P_HEADERS
  service/p2p_chat_service/p2p_chat_handler.h
  service/p2p_chat_service/http_p2p_chat_request_factory.h
)

add_executable(${P2P_CHAT_SERVICE}
  ${P_HEADERS}
  service/p2p_chat_service/p2p_chat_service.cpp
)

target_link_libraries(${P2P_CHAT_SERVICE} PRIVATE HLMAI)

#set(WEB_SERVICE ${PROJECT_NAME}_web_service)
#set(OTHER_SERVICE ${PROJECT_NAME}_other_service)

#find_package(OpenSSL)
#find_package(Threads)
#find_package(ZLIB)

#include_directories("/usr/include/mysql")
#include_directories("/usr/local/include/mysql")
#link_directories("/usr/local/lib")

#set(W_HEADERS 
#    web_server/handlers/user_handler.h
#    web_server/http_request_factory.h
#    web_server/http_web_server.h
#)

#set(W_SOURCES web_server/main.cpp)

#add_executable(${WEB_SERVICE} ${W_HEADERS} ${W_SOURCES})

#target_link_libraries(${WEB_SERVICE} PRIVATE HLMAI)

#set(O_HEADERS 
#  other_server/handlers/other_handler.h
#  other_server/http_other_request_factory.h
#  other_server/http_other_server.h
#)

#set(O_SOURCES other_server/main.cpp)
