cmake_minimum_required(VERSION 3.15)
project(DaemonMonitoringProject LANGUAGES CXX)

cmake_policy(SET CMP0167 NEW)
cmake_policy(SET CMP0144 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
  file(GLOB PROGRAM_SOURCES ${prog}/src/*.cpp)

  if(PROGRAM_SOURCES)
    add_executable(${prog} ${PROGRAM_SOURCES})
    target_include_directories(${prog} PRIVATE
      ${prog}/include
    )
    target_link_libraries(${prog} PRIVATE httplib::httplib PRIVATE nlohmann_json::nlohmann_json)
  endif()
endforeach()

option(BUILD_TESTS "build unit tests" OFF)

if(BUILD_TESTS)
  enable_testing()
  find_package(Boost REQUIRED)

  if(Boost_FOUND)
    foreach(prog ${PROGRAMS})
      file(GLOB PROGRAM_SOURCES_NO_MAIN ${prog}/src/*.cpp)
      list(FILTER PROGRAM_SOURCES_NO_MAIN EXCLUDE REGEX "main\\.cpp$")

      file(GLOB TEST_SOURCES ${prog}/tests/*.cpp)

      if(TEST_SOURCES AND PROGRAM_SOURCES_NO_MAIN)
        add_executable(test-${prog}
          ${PROGRAM_SOURCES_NO_MAIN}
          ${TEST_SOURCES}
        )
        target_include_directories(test-${prog} PRIVATE
          ${prog}/include
          ${Boost_INCLUDE_DIRS}
        )
        target_compile_definitions(test-${prog} PRIVATE BOOST_TEST_HEADER_ONLY)
        add_test(NAME test-${prog} COMMAND test-${prog})
      endif()
    endforeach()
  endif()
endif()
