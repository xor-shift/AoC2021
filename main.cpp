#include <memory>

#include <common/solution.hpp>
#include <solutions/2021/0/main.hpp>
#include <solutions/2021/1/main.hpp>

int main() {
    auto today = std::make_unique<AOC::solution_t<2021, 0>>();
    today->SolveFile("inputs/2021/1/test.in");
    return 0;
}
