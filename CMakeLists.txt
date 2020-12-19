cmake_minimum_required(VERSION 3.16)
project(FormalPractice1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

add_executable(FormalPractice1 main.cpp NKA.cpp NKA.h task14.h Builder.h)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

enable_testing()
set(CMAKE_EXE_LINKER_FLAGS --coverage)

add_executable(Test Tests.cpp Tests.h NKA.h NKA.cpp RunTests.cpp task14.h TestUtils.h Builder.h)
target_link_libraries(Test ${GTEST_LIBRARIES} Threads::Threads)

add_definitions(-fprofile-arcs -ftest-coverage)
target_link_libraries(Test -fprofile-arcs)
