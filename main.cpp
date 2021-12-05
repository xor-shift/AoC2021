#include <memory>

#include <common/solution.hpp>
#include <solutions/2021/5/main.hpp>

int main() {
    auto today = std::make_unique<AOC::solution_t<2021, 5>>();
    today->SolveFile("inputs/2021/5/5-20000-6400-fixed.in");
    return 0;
}
