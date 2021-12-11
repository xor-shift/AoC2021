#pragma once

#include <common/solution.hpp>

#define TheDay 0
#define TheNamespace namespace AOC::Y2021::D##TheDay

TheNamespace {

class Sol final : public SolutionBase {
  public:
    ~Sol() noexcept override = default;

    void Solve(std::string_view data) override;
};

}

namespace AOC {
SolutionFactory(2021, TheDay);
}

#undef TheDay
