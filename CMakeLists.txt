cmake_minimum_required(VERSION 3.10)
project(CLCMiner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(clcminer src/main.cpp src/lib/uint256.cpp src/config.cpp)
target_link_libraries(clcminer OpenSSL::Crypto Threads::Threads secp256k1 curl config)
