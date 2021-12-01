cmake_minimum_required(VERSION 3.21)
project(aoc)

set(CMAKE_CXX_STANDARD 23)

add_executable(aoc
        main.cpp

        include/common/solution.hpp
        include/common/util.hpp

        include/common/solution.cpp
        include/common/util.cpp
        )
target_include_directories(aoc PRIVATE
        solutions
        include)
target_link_libraries(aoc fmt)

foreach (year 2021)
    foreach (day RANGE 0 25 1)
        set(base ${CMAKE_SOURCE_DIR}/include/solutions/${year}/${day}/main)
        if (EXISTS ${base}.hpp)
            target_sources(aoc PUBLIC ${base}.hpp ${base}.cpp)
        endif ()
    endforeach ()
endforeach ()
