#include <memory>

#include <common/solution.hpp>
#include <solutions/2021/21/main.hpp>

int main() {
    auto today = std::make_unique<AOC::solution_t<2021, 21>>();
    today->SolveFile("inputs/2021/21/in.in");
    return 0;
}
