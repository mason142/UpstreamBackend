cmake_minimum_required(VERSION 3.21)

project(PocoTimer)
set(CMAKE_PREFIX_PATH /home/simon/libs/poco/lib/cmake/Poco)

find_package(Poco REQUIRED Foundation)

add_executable(
    main main.cpp
)

target_link_libraries(
    main
    PRIVATE Poco::Foundation
)