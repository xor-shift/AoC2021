#include "main.hpp"

#include <stack>

namespace AOC::Y2021::D10 {

void Sol::Solve(std::string_view data) {
    using boost::multiprecision::cpp_int;
    using namespace Utils;

    auto lines = GetLines(data, "\n");

    std::unordered_map<char, char> complement{
      {'(', ')'},
      {'[', ']'},
      {'<', '>'},
      {'{', '}'},
      {')', '('},
      {']', '['},
      {'>', '<'},
      {'}', '{'},
    };
    std::unordered_map<char, u64> pts{
      {')', 3},
      {']', 57},
      {'>', 25137},
      {'}', 1197},
    };
    std::unordered_map<char, u64> pts2{
      {')', 1},
      {']', 2},
      {'>', 4},
      {'}', 3},
    };

    u64 sum = 0, sum2 = 0;
    std::vector<u64> pt2;
    for (auto line: lines) {
        std::list<char> asd;

        auto Pop = [&](char c) {
            if (asd.back() != complement.at(c)) return false;
            asd.pop_back();
            return true;
        };

        bool incomp = false;
        for (char c: line) {
            if (!pts.contains(c)) asd.push_back(c);
            else {
                if (!Pop(c)) {
                    const auto p = pts.at(c);
                    //PrintShit(line, c, p);
                    sum += p;
                    incomp = true;
                    break;
                }
            }
        }

        if (!incomp) {
            u64 temp = 0;
            while (asd.size()) {
                auto c = asd.back();
                auto cmp = complement.at(c);
                //fmt::print("{}", cmp);
                asd.pop_back();

                temp *= 5;
                temp += pts2.at(cmp);
            }
            //fmt::print("\n");
            PrintShit(temp);
            pt2.push_back(temp);
        }
    }

    std::ranges::sort(pt2);
    PrintShit(sum, sum2, pt2.at(pt2.size() / 2));
}

}