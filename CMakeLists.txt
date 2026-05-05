cmake_minimum_required(VERSION 3.30)
project(DaemonMonitoringProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROGRAMS client daemon storage)

foreach(prog ${PROGRAMS})
  file(GLOB PROGRAM_SOURCES ${CMAKE_SOURCE_DIR}/${prog}/src/*.cpp)

  if(PROGRAM_SOURCES)
    add_executable(${prog} ${PROGRAM_SOURCES})
    target_include_directories(${prog} PRIVATE
      ${CMAKE_SOURCE_DIR}/${prog}/include
      ${CMAKE_SOURCE_DIR}/libs
    )
  endif()
endforeach()

add_custom_command(
  TARGET client PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/client/config
  ${CMAKE_BINARY_DIR}/client/config
)

option(BUILD_TESTS "build unit tests" OFF)

if(BUILD_TESTS)
  enable_testing()
  find_package(Boost REQUIRED)

  if(Boost_FOUND)
    foreach(prog ${PROGRAMS})
      file(GLOB PROGRAM_SOURCES_NO_MAIN ${CMAKE_SOURCE_DIR}/${prog}/src/*.cpp)
      list(FILTER PROGRAM_SOURCES_NO_MAIN EXCLUDE REGEX "main\\.cpp$")

      file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/${prog}/tests/*.cpp)

      if(TEST_SOURCES AND PROGRAM_SOURCES_NO_MAIN)
        add_executable(test-${prog}
          ${PROGRAM_SOURCES_NO_MAIN}
          ${TEST_SOURCES}
        )
        target_include_directories(test-${prog} PRIVATE
          ${CMAKE_SOURCE_DIR}/${prog}/include
          ${CMAKE_SOURCE_DIR}/libs
          ${Boost_INCLUDE_DIRS}
        )
        add_test(NAME test-${prog} COMMAND test-${prog})
      endif()
    endforeach()
  endif()
endif()
