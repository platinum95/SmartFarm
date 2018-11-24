cmake_minimum_required(VERSION 3.12)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
list(APPEND CMAKE_CXX_FLAGS
      -fpermissive
)

project(NONE)

FILE(GLOB app_sources src/*.c*)
target_sources(app PRIVATE ${app_sources})
