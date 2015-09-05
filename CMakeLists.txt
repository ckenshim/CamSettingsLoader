cmake_minimum_required(VERSION 3.2)
project(CamSettingsDownloader)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

link_directories("/usr/local/lib")
set (EXTRA_LIBS ${EXTRA_LIBS} PocoFoundation PocoNetSSL PocoNet)
set(SOURCE_FILES main.cpp CarWashApiClient.cpp CarWashApiClient.h)
add_executable(CamSettingsDownloader ${SOURCE_FILES})
target_link_libraries (CamSettingsDownloader ${EXTRA_LIBS})