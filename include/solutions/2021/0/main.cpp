#include "main.hpp"

namespace AOC::Y2021::D0 {

void Sol::Solve(std::string_view data) {
    Utils::ProcessLines(data, "\n", [](std::string_view line, std::size_t n) {
        fmt::print("{}: {}\n", n, line);
    });

    std::size_t product = 1;

    for (auto vec = Utils::GetNumbersFromLines<std::size_t, 1>(data, "\n"); auto i: vec) {
        product *= i;
        fmt::print("{}\n", i);
    }

    fmt::print("{}\n", product);
}

}