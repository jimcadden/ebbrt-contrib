project("iperf-ebbrt")
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g3")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -flto -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -fpermissive -DHAVE_CONFIG_H")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_CONFIG_H")

find_package(EbbRTSocket REQUIRED)
include_directories(${EBBRT-SOCKET_INCLUDE_DIRS})
link_libraries(${EBBRT-SOCKET_LIBRARIES})

set(sources
  # ebbrt
  ${CMAKE_SOURCE_DIR}/iperf.cpp
  # iperf-2.0.9
  #${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/Client.cpp
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/Launch.cpp
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/List.cpp
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/Listener.cpp
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/main.cpp
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/PerfSocket.cpp
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/Server.cpp
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/Settings.cpp
  #${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/checkdelay.cc
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/Extractor.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/gnu_getopt.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/gnu_getopt_long.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/Locale.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/ReportCSV.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/ReportDefault.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/Reporter.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/service.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/SocketAddr.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/sockets.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/stdio.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/src/tcp_window_size.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/compat/delay.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/compat/error.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/compat/gettimeofday.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/compat/inet_ntop.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/compat/inet_pton.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/compat/signal.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/compat/snprintf.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/compat/string.c
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/compat/Thread.c
)

set( dirs
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/include
  ${CMAKE_SOURCE_DIR}/iperf-2.0.9/compat
)

include_directories(${dirs})
add_executable(iperf.elf ${sources})

add_custom_command(TARGET iperf.elf POST_BUILD 
  COMMAND objcopy -O elf32-i386 iperf.elf iperf.elf32 )
