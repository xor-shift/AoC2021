#include "main.hpp"

#include <algorithm>
#include <numeric>

namespace AOC::Y2021::D1 {

void Sol::Solve(std::string_view data) {
    auto vec =  Utils::GetNumbersFromLines<std::size_t, 1>(data, "\n");

    std::size_t numIncreases = 0;
    auto prev = *vec.begin();

    for (auto i : vec) {
        if (i > prev) ++ numIncreases;
        prev = i;
    }

    fmt::print("{}\n", numIncreases);

    numIncreases = 0;
    prev = *vec.begin();
    bool hadPrev = false;
    for (std::size_t i = 2; i < vec.size(); i++) {
        std::size_t n = 0;
        for (std::size_t j = (i >=2 ? i - 2 : 0); j <= std::min(i, vec.size() - 1); j++) {
            n += vec[j];
        }

        if (hadPrev && n > prev) ++numIncreases;
        prev = n;
        hadPrev = true;
    }

    fmt::print("{}\n", numIncreases);
}

}