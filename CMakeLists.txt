cmake_minimum_required(VERSION 3.15)
project(DaemonMonitoringProject LANGUAGES CXX)

cmake_policy(SET CMP0167 NEW)
cmake_policy(SET CMP0144 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB COMMON_SOURCES src/common/*.cpp)

include(FetchContent)
FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.18.7
)
FetchContent_MakeAvailable(httplib)

include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(json)

set(PROGRAMS client daemon storage)

foreach(prog ${PROGRAMS})
  file(GLOB PROGRAM_SOURCES src/${prog}/src/*.cpp)
  add_executable(${prog} ${PROGRAM_SOURCES} ${COMMON_SOURCES})
  target_include_directories(${prog} PRIVATE
    "src/${prog}/include"
    "src"
  )
  target_link_libraries(${prog} PRIVATE httplib::httplib PRIVATE nlohmann_json::nlohmann_json)
endforeach()

option(BUILD_TESTS "build unit tests" OFF)

if(BUILD_TESTS)
  enable_testing()
  find_package(Boost REQUIRED)

  if(Boost_FOUND)
    foreach(prog ${PROGRAMS})
      file(GLOB PROG_SRC_NO_MAIN src/${prog}/src/*.cpp)
      list(FILTER PROG_SRC_NO_MAIN EXCLUDE REGEX "main\\.cpp$")

      file(GLOB TEST_SOURCES tests/unit/${prog}/*.cpp)

      if(TEST_SOURCES)
        add_executable(test-${prog}
          ${COMMON_SOURCES}
          ${PROG_SRC_NO_MAIN}
          ${TEST_SOURCES}
        )
        target_include_directories(test-${prog} PRIVATE
          src
          src/${prog}/include
          ${Boost_INCLUDE_DIRS}
        )
        target_compile_definitions(test-${prog} PRIVATE BOOST_TEST_HEADER_ONLY)
        add_test(NAME test-${prog} COMMAND test-${prog})
      endif()
    endforeach()
  endif()
endif()
