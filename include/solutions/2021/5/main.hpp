#pragma once

#include <common/solution.hpp>

namespace AOC::Y2021::D5 {

class Sol final : public SolutionBase {
  public:
    ~Sol() noexcept override = default;

    void Solve(std::string_view data) override;
};

}

namespace AOC {
SolutionFactory(2021, 5);
}
