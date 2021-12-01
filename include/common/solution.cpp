#include "solution.hpp"

#include <common/util.hpp>

namespace AOC {

void SolutionBase::SolveFile(const std::string &filename) {
    Utils::MappedStringView mapped(filename);
    Solve(static_cast<std::string_view>(mapped));
}

}
