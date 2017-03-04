cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project("weaksym-ebbrt" C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g3")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -flto -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -Wall -Werror")

set(HOSTED_SOURCES
      src/hosted/weaksym.cc
      )

set(BAREMETAL_SOURCES
      src/baremetal/weaksym.cc
      )


# Baremetal  ========================================================
if( ${CMAKE_SYSTEM_NAME} STREQUAL "EbbRT")
find_package(EbbRTZooKeeper REQUIRED)
  find_package(EbbRTSocket REQUIRED)
  include_directories(${EBBRT-SOCKET_INCLUDE_DIRS}
    ${EBBRT-ZOOKEEPER_INCLUDE_DIRS})
  add_executable(weaksym.elf ${BAREMETAL_SOURCES})
  target_link_libraries(weaksym.elf ${EBBRT-ZOOKEEPER_LIBRARIES} ${EBBRT-SOCKET_LIBRARIES})
  add_custom_command(TARGET weaksym.elf POST_BUILD
    COMMAND objcopy -O elf32-i386 weaksym.elf weaksym.elf32 )

# Hosted  ===========================================================
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
find_package(EbbRTZooKeeper REQUIRED)
  find_package(EbbRT REQUIRED)
  find_package(Boost 1.53.0 REQUIRED COMPONENTS
    filesystem system coroutine context )
  find_package(Capnp REQUIRED)
  find_package(TBB REQUIRED)
  find_package(Threads REQUIRED)

  include_directories(${EBBRT_INCLUDE_DIRS}
    ${EBBRT-ZOOKEEPER_INCLUDE_DIRS})
  add_executable(weaksym ${HOSTED_SOURCES})
  target_link_libraries(weaksym ${EBBRT_LIBRARIES}
    ${CAPNP_LIBRARIES_LITE} ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES} ${TBB_LIBRARIES} ${EBBRT-ZOOKEEPER_LIBRARIES}
    ${EBBRT_LIBRARIES})
else()
  message(FATAL_ERROR "System name unsupported: ${CMAKE_SYSTEM_NAME}")
endif()
