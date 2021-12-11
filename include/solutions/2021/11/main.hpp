#pragma once

#include <common/solution.hpp>

#define TheDay 11
#define TheNamespace(d) namespace AOC::Y2021::D##d

TheNamespace(11) {

class Sol final : public SolutionBase {
  public:
    ~Sol() noexcept override = default;

    void Solve(std::string_view data) override;
};

}

namespace AOC {
SolutionFactory(2021, 11);
}

#undef TheNamespace
#undef TheDay
