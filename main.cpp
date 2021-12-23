#include <memory>

#include <common/solution.hpp>
#include <solutions/2021/19/main.hpp>
#include <solutions/2021/22/main.hpp>

int main() {
    //auto today = std::make_unique<AOC::solution_t<2021, 22>>();
    //today->SolveFile("inputs/2021/22/test.in");
    auto d19 = std::make_unique<AOC::solution_t<2021, 19>>();
    d19->SolveFile("inputs/2021/19/in.in");
    return 0;
}
