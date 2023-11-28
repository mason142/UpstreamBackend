cmake_minimum_required(VERSION 3.21)

project(PocoHTTP)
set(Poco_DIR "home/simon/libs/lib/cmake/Poco/") 
set(Poco_INCLUDE_DIRS "home/simon/libs/include/Poco/Net" "home/simon/libs/include/Poco")

find_package(Poco REQUIRED COMPONENTS Foundation Net XML Util)

include_directories( ${Poco_INCLUDE_DIRS}) 

add_executable(main main.cpp)
target_link_libraries(main ${Poco_LIBRARIES}) 
target_link_libraries(PocoNet PocoFoundation)
target_link_libraries(PocoNetSSL PocoCrypto PocoUtil PocoNet)
target_link_libraries(PocoCrypto PocoFoundation Ssl Crypto)
target_link_libraries(PocoUtil PocoFoundation)
target_link_libraries(PocoJSON PocoFoundation)
target_link_libraries(PocoXML PocoFoundation)