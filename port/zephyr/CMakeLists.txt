# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(crosspoint)

target_include_directories(app PRIVATE src)
target_sources(app PRIVATE
  src/main.cpp
  src/hal/HalGpio.cpp
  src/hal/HalDisplay.cpp
  src/hal/HalStorage.cpp
  src/hal/HalPower.cpp
  src/hal/HalSystem.cpp
)
