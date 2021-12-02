#include "main.hpp"

#include <algorithm>
#include <numeric>

namespace AOC::Y2021::D1 {

void FirstSolution(std::string_view data) {
    auto vec = Utils::GetNumbersFromLines<std::size_t, 1>(data, "\n");

    std::size_t numIncreases = 0;
    auto prev = *vec.begin();

    for (auto i: vec) {
        if (i > prev) ++numIncreases;
        prev = i;
    }

    fmt::print("{}\n", numIncreases);

    numIncreases = 0;
    prev = *vec.begin();
    bool hadPrev = false;
    for (std::size_t i = 2; i < vec.size(); i++) {
        std::size_t n = 0;
        for (std::size_t j = (i >= 2 ? i - 2 : 0); j <= std::min(i, vec.size() - 1); j++) {
            n += vec[j];
        }

        if (hadPrev && n > prev) ++numIncreases;
        prev = n;
        hadPrev = true;
    }

    fmt::print("{}\n", numIncreases);
}

void Refactor(std::string_view data) {
    std::size_t part1 = 0, part2 = 0;
    constexpr std::size_t windowSize = 3;
    std::array<std::size_t, windowSize + 1> window{0};

    Utils::ProcessLines(data, "\n", [&part1, &part2, &window](std::string_view line, std::size_t n) {
        for (std::size_t i = windowSize; i > 0; i--) window[i] = window[i - 1];
        window[0] = Utils::Convert<std::size_t>(line, n);
        part1 += n && window[1] < window[0];
        part2 += (n > windowSize - 1) && (window[0] > window[windowSize]);
    });

    fmt::print("{}, {}\n", part1, part2);
}

void Sol::Solve(std::string_view data) {
    return Refactor(data);

    //1195
    //1235
}

}