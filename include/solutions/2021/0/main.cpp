#include "main.hpp"

namespace AOC::Y2021::D0 {

void Sol::Solve(std::string_view data) {
    Utils::ProcessLines(data, "\n", [](std::string_view line, std::size_t n) {
        fmt::print("{}: {}\n", n, line);
    });
}

}