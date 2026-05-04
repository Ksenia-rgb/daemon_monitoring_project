cmake_minimum_required(VERSION 3.30)
project(DaemonMonitoringProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROGRAMS client daemon storage)

foreach(prog ${PROGRAMS})
  file(GLOB PROGRAM_SOURCES ${prog}/src/*.cpp)

  if(PROGRAM_SOURCES)
    add_executable(${prog} ${PROGRAM_SOURCES})
    target_include_directories(${prog} PRIVATE
      ${prog}/include
      libs
    )
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
          libs
          ${Boost_INCLUDE_DIRS}
        )
        add_test(NAME test-${prog} COMMAND test-${prog})
      endif()
    endforeach()
  endif()
endif()
