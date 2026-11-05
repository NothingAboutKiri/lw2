cmake_minimum_required(VERSION 3.16)
project(lab2 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lab2_core STATIC
    structures/singly_list.cpp structures/doubly_list.cpp structures/stack.cpp
    structures/json.cpp structures/hash_set.cpp structures/open_hash.cpp
    algorithms/xml.cpp algorithms/pattern.cpp algorithms/huffman.cpp algorithms/genealogy.cpp algorithms/lru.cpp
    tasks/io.cpp tasks/task1_xml.cpp tasks/task2_set.cpp tasks/task4_pattern.cpp tasks/task5_huffman.cpp
    tasks/task6_hash.cpp tasks/task6_genealogy.cpp tasks/task7_lru.cpp)
target_compile_options(lab2_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(lab2 main.cpp)
target_link_libraries(lab2 lab2_core)
target_compile_options(lab2 PRIVATE -Wall -Wextra -Wpedantic)

include(CTest)
if(BUILD_TESTING)
    add_executable(lab2_tests tests/tests.cpp)
    target_link_libraries(lab2_tests PRIVATE lab2_core)
    target_compile_options(lab2_tests PRIVATE -Wall -Wextra -Wpedantic)
    add_test(NAME lab2_tests COMMAND lab2_tests)
endif()
