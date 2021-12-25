#include <memory>

#include <common/solution.hpp>
#include <solutions/2021/25/main.hpp>

int main() {
    auto today = std::make_unique<AOC::solution_t<2021, 25>>();
    today->SolveFile("inputs/2021/25/test.in");
    return 0;
}
