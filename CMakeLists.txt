project(leveldb.json CXX C)
cmake_minimum_required(VERSION 2.6)

execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
if (NOT (GCC_VERSION VERSION_GREATER 4.5 OR GCC_VERSION VERSION_EQUAL 4.5))
	message(FATAL_ERROR "GCC 4.5+ Required")
endif()

SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -fno-inline ")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -ggdb ")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb ")
SET(CMAKE_C_FLAGS_RELEASE "-O2 -ggdb ")
SET(CMAKE_CXX_FLAGS "-Wall ${FAS_FLAGS} --std=c++0x")
SET(CMAKE_C_FLAGS "-Wall")

SET(SRC_PATH ".")
include_directories( ${SRC_PATH} ${SRC_PATH}/src)
add_executable(leveldb.json ${SRC_PATH}/src/daemon.cpp ${SRC_PATH}/common/params.cpp)
target_link_libraries(leveldb.json pthread boost_program_options z boost_thread-mt leveldb tbb)

