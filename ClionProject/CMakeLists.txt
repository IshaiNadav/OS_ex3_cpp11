cmake_minimum_required(VERSION 3.10)
project(ClionProject)

set(CMAKE_CXX_STANDARD 11)

add_executable(ClionProject Factory.cxx Factory.h Product.h test_utilities.h test.cxx )