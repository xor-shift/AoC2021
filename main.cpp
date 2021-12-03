#include <memory>

#include <common/solution.hpp>
#include <solutions/2021/3/main.hpp>

int main() {
    auto today = std::make_unique<AOC::solution_t<2021, 3>>();
    today->SolveFile("inputs/2021/3/in.in");
    //4797452
    return 0;
}
