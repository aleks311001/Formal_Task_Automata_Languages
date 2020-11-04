cmake_minimum_required(VERSION 3.17)
project(FormalPractice1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

add_executable(FormalPractice1 main.cpp NKA.cpp NKA.h)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(Test Tests.cpp Tests.h NKA.h NKA.cpp RunTests.cpp task14.h)
target_link_libraries(Test ${GTEST_LIBRARIES} Threads::Threads)
