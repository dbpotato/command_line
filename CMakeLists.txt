#[[
Copyright (c) 2018 Adam Kaniewski

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
]]

cmake_minimum_required(VERSION 3.2)

set(CMAKE_SYSTEM_NAME Linux)

set(COMMON_DIR "${PROJECT_SOURCE_DIR}/common")
set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(CMAKE_CXX_FLAGS "-fPIC -O2 -std=c++11")

set(DEFAULT_CXX "g++")

if(DEFINED ENV{CXX})
  message("Using user's compiler : " $ENV{CXX})
  set(CMAKE_CXX_COMPILER $ENV{CXX})
else(DEFINED ENV{CXX})
  message("Using default compiler : " ${DEFAULT_CXX})
  set(CMAKE_CXX_COMPILER ${DEFAULT_CXX})
endif(DEFINED ENV{CXX})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_${CMAKE_CXX_COMPILER})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_${CMAKE_CXX_COMPILER})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build_${CMAKE_CXX_COMPILER})


include_directories(
  "${COMMON_DIR}/third_party/spdlog/include"
  "${COMMON_DIR}/tools/thread"
  "${COMMON_DIR}/tools/logger"
  "${COMMON_DIR}/tools/net")


set(COMMON
  ${COMMON_DIR}/tools/net/SocketObject.cpp
  ${COMMON_DIR}/tools/net/Client.cpp
  ${COMMON_DIR}/tools/net/Server.cpp
  ${COMMON_DIR}/tools/net/Connection.cpp
  ${COMMON_DIR}/tools/net/Message.cpp
  ${COMMON_DIR}/tools/net/MessageBuilder.cpp
  ${COMMON_DIR}/tools/thread/PosixThread.cpp
)

set(CMDLLIB
  ${SRC_DIR}/CmdServer.cpp
  ${COMMON_DIR}/tools/net/ServerImpl.cpp
  ${SRC_DIR}/CommandLine.cpp
  ${SRC_DIR}/lib.cpp
  ${COMMON}
)

set(EXAMPLE
  ${SRC_DIR}/example.cpp
)

set(CLIENT
  ${SRC_DIR}/CmdClient.cpp
  ${COMMON_DIR}/tools/net/ConnectionChecker.cpp
  ${COMMON}
)

add_library(cmdline SHARED ${CMDLLIB})
target_link_libraries(cmdline pthread)

add_executable(example ${EXAMPLE})
target_link_libraries(example pthread dl)

add_executable(client ${CLIENT})
target_link_libraries(client pthread)
