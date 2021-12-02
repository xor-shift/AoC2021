#include "main.hpp"

namespace AOC::Y2021::D2 {

void FirstSolution(std::string_view data) {
    /*Utils::ProcessLines(data, "\n", [](std::string_view line, std::size_t n) {
    fmt::print("{}: {}\n", n, line);
});

Utils::ProcessNumbersFromLines<std::size_t, 2>(data, [](std::size_t v, std::string_view, std::size_t row, std::size_t col) {
    fmt::print("{}, {}: {}\n", row, col, v);
});*/

    //auto vec = Utils::GetNumbersFromLines<std::size_t, 1>(data, "\n");

    std::ptrdiff_t depth = 0, horiz = 0, aim = 0;

    Utils::ProcessLines(data, "\n", [&depth, &horiz, &aim](std::string_view line, std::size_t n) {
        auto it = line.find_first_of(" ");

        auto lhs = line.substr(0, it);
        auto rhs = line.substr(it + 1);
        //auto rhsv = Utils::ConvertUsing(rhs, 0, [](const char *s, char **e, int b) { return std::strtoll(s, e, b); }, 10);
        //note: changed from the original which returned a std::size_t (due to bad writing), hence the use of std::size_t over std::ptrdiff_t
        auto rhsv = Utils::Convert<std::size_t>(rhs, n);

        std::unordered_map<std::string_view, std::size_t> asd{
          {"forward", 1,},
          {"down",    2},
          {"up",      3},
        };

        switch (asd[lhs]) {
            case 1:
                horiz += rhsv;
                depth += aim * static_cast<std::ptrdiff_t>(rhsv);
                break;
            case 2:
                //depth += rhsv;
                aim += rhsv;
                break;
            case 3:
                //depth -= rhsv;
                aim -= rhsv;
                break;
        }
        fmt::print("{} * {} = {} (aim={})\n", horiz, depth, horiz * depth, aim);
    });

    fmt::print("{} * {} = {} (aim={})\n", horiz, depth, horiz * depth, aim);
}

void Sol::Solve(std::string_view data) {
    FirstSolution(data);
}

}
