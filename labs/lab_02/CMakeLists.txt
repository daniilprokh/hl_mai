cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(lab_02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_INSTALL_RPATH "/usr/local/lib")
#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(AUTHORIZATION_SERVICE ${PROJECT_NAME}_authorization_service)

set(A_HEADERS
  authorization_service/authorization_handler.h
  authorization_service/http_authorization_request_factory.h
)

add_executable(${AUTHORIZATION_SERVICE} 
  ${A_HEADERS} 
  authorization_service/authorization_service.cpp
)

target_link_libraries(${AUTHORIZATION_SERVICE} PRIVATE HLMAI)
