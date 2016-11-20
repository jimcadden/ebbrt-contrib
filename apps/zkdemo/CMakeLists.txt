cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project("zkdemo-ebbrt" C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g3")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -flto -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -Wall -Werror")

set(HOSTED_SOURCES
      src/ZKGlobalIdMap.cc
      src/hosted/zkdemo.cc
      src/hosted/Printer.cc)

set(BAREMETAL_SOURCES
      src/ZKGlobalIdMap.cc
      src/baremetal/zkdemo.cc
      src/baremetal/Printer.cc)

  find_package(EbbRTZooKeeper REQUIRED)

# Baremetal  ========================================================

if( ${CMAKE_SYSTEM_NAME} STREQUAL "EbbRT")

  find_package(EbbRTSocket REQUIRED)
  include_directories(${EBBRT-SOCKET_INCLUDE_DIRS}
    ${EBBRT-ZOOKEEPER_INCLUDE_DIRS})
  add_executable(zkdemo.elf ${BAREMETAL_SOURCES})
  target_link_libraries(zkdemo.elf ${EBBRT-ZOOKEEPER_LIBRARIES} ${EBBRT-SOCKET_LIBRARIES})
  add_custom_command(TARGET zkdemo.elf POST_BUILD
    COMMAND objcopy -O elf32-i386 zkdemo.elf zkdemo.elf32 )

# Hosted  ===========================================================

elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )

  find_package(EbbRT REQUIRED)
  find_package(Boost 1.53.0 REQUIRED COMPONENTS
    filesystem system coroutine context )
  find_package(Capnp REQUIRED)
  find_package(TBB REQUIRED)
  find_package(Threads REQUIRED)
  include_directories(${EBBRT_INCLUDE_DIRS} 
    ${EBBRT-ZOOKEEPER_INCLUDE_DIRS})
  add_executable(zkdemo ${HOSTED_SOURCES})
  target_link_libraries(zkdemo ${EBBRT_LIBRARIES}
    ${CAPNP_LIBRARIES_LITE} ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES} ${TBB_LIBRARIES} ${EBBRT-ZOOKEEPER_LIBRARIES} 
  )
else()
  message(FATAL_ERROR "System name unsupported: ${CMAKE_SYSTEM_NAME}")
endif()
