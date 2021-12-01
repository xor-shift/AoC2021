cmake_minimum_required(VERSION 3.21)
project(aoc)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mtune=native -funsafe-math-optimizations")
if ((CMAKE_BUILD_TYPE MATCHES Debug) OR (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fsanitize=address -fsanitize=undefined")
endif ()

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
