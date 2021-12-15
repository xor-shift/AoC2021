#include <memory>

#include <common/solution.hpp>
#include <solutions/2021/15/main.hpp>

int main() {
    auto today = std::make_unique<AOC::solution_t<2021, 15>>();
    today->SolveFile("inputs/2021/15/in.in");
    return 0;
}
