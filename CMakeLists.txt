cmake_minimum_required(VERSION 3.30)
project(data_forge)

set(CMAKE_CXX_STANDARD 20)
set(BOOST_ROOT "/opt/homebrew/Cellar/boost/1.87.0")

find_package(Boost REQUIRED COMPONENTS system json)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(data_forge src/main.cpp
        src/generator/generator.cpp
        src/generator/generator.h
        src/composer/composer.cpp
        src/composer/composer.h
        src/writer/csv_writer.cpp
        src/writer/csv_writer.h
        src/server/request/request_params.h
        src/server/request/request_params.cpp
        src/server/request/request_handler.h
        src/server/request/request_handler.cpp
        src/server/server.cpp
        src/server/server.h
)
target_link_libraries(data_forge ${Boost_LIBRARIES})

add_executable(data_forge_tests
        tests/main.cpp
        tests/generator_tests.cpp
        tests/composer.cpp
        tests/csv_writer_tests.cpp
        src/generator/generator.cpp
        src/generator/generator.h
        src/composer/composer.cpp
        src/composer/composer.h
        src/writer/csv_writer.cpp
        src/writer/csv_writer.h
        src/server/request/request_params.h
        src/server/request/request_handler.h
        src/server/request/request_handler.cpp
        src/server/server.cpp
        src/server/server.h
)
target_link_libraries(data_forge_tests ${Boost_LIBRARIES} GTest::GTest GTest::Main)