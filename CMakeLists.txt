cmake_minimum_required(VERSION 3.31)
project(untitled2)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled2 main.cpp
        bplustree/BPlusTree.cpp
        bplustree/BPlusTree.h
        bplustree/Node.cpp
        bplustree/Node.h)
