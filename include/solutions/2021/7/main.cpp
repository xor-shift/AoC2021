#include "main.hpp"

namespace AOC::Y2021::D7 {

void Sol::Solve(std::string_view data) {
    using namespace Utils;
    auto nums = GetNumbersFromLines<std::ptrdiff_t>(data, ",");

    auto sum = std::accumulate(nums.cbegin(), nums.cend(), 0ll);
    auto t = static_cast<std::ptrdiff_t>(std::round(static_cast<double>(sum) / static_cast<double>(nums.size())));
    //PrintShit(sum, t);
    std::ptrdiff_t asd = std::numeric_limits<std::ptrdiff_t>::max();
    auto max = std::max_element(nums.cbegin(), nums.cend());
    for (auto d = 0ll; d <= *max; d++) {
        std::ptrdiff_t sum = 0;
        for (auto num: nums) {
            //asd += std::abs(num - asd);
            auto temp =  std::abs(num - d);
            sum += (temp * (temp + 1)) / 2;
        }
        asd = std::min<std::ptrdiff_t>(sum, asd);
        //PrintShit(asd, d);
    }

    PrintShit(asd);
    //1397447
}

}