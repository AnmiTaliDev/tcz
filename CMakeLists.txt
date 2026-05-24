cmake_minimum_required(VERSION 3.16)
project(tcz VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0167 NEW)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost 1.65 REQUIRED)
find_package(CURL REQUIRED)

find_path(TGBOT_INCLUDE_DIR tgbot/tgbot.h)
find_library(TGBOT_LIBRARY NAMES TgBot)

if(NOT TGBOT_INCLUDE_DIR OR NOT TGBOT_LIBRARY)
    message(FATAL_ERROR "TgBot-cpp not found")
endif()

find_package(date CONFIG QUIET)
if(NOT date_FOUND)
    find_path(DATE_INCLUDE_DIR date/tz.h)
    find_library(DATE_LIBRARY NAMES date-tz)
    if(NOT DATE_INCLUDE_DIR OR NOT DATE_LIBRARY)
        message(FATAL_ERROR "Howard Hinnant date library not found")
    endif()
endif()

add_executable(tcz
    src/main.cpp
    src/aliases.cpp
    src/timezone.cpp
    src/formatter.cpp
    src/handlers.cpp
)

target_include_directories(tcz PRIVATE ${TGBOT_INCLUDE_DIR})

if(date_FOUND)
    target_link_libraries(tcz date::date-tz)
else()
    target_include_directories(tcz PRIVATE ${DATE_INCLUDE_DIR})
    target_link_libraries(tcz ${DATE_LIBRARY})
endif()

target_link_libraries(tcz
    ${TGBOT_LIBRARY}
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
    Boost::headers
    CURL::libcurl
)

target_compile_options(tcz PRIVATE -Wall -Wextra -O2)
