#pragma once

#include <string>

#include "util.hpp"

namespace AOC {

class SolutionBase {
  public:
    virtual ~SolutionBase() noexcept = default;

    virtual void SolveFile(const std::string &filename);

    virtual void Solve(std::string_view data) = 0;
};

template<std::size_t year, std::size_t day>
struct SolutionFor { typedef void solution_t; };

template<std::size_t year, std::size_t day>
using solution_t = typename SolutionFor<year, day>::solution_t;

#define SolutionFactory(year, day) \
template<>\
struct SolutionFor<2021, day> { typedef AOC::Y##year::D##day::Sol solution_t; };

}
