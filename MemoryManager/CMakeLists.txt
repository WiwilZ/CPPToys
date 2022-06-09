cmake_minimum_required(VERSION 3.21)
project(MemoryManager)

set(CMAKE_CXX_STANDARD 20)

add_executable(MemoryManager test.cpp Allocator.h MemoryPool.h)
