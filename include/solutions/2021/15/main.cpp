#include "main.hpp"

#include <queue>

namespace AOC::Y2021::D15 {

void Sol::Solve(std::string_view data) {
    using namespace Utils::UseThisNamespace;

    auto lines = GetLines(data, "\n");

    vec<vec<u64>> grid;
    u64 orig_w = 0, orig_h = lines.size();
    for (auto line: lines) {
        grid.emplace_back();
        transform(line, std::back_inserter(grid.back()), [](char c) -> u64 { return c - '0'; });
        if (!orig_w) orig_w = grid.back().size();
        grid.back().reserve(orig_w * 5);

        for (std::size_t i = 0; i < 4; i++)
            std::transform(grid.back().cbegin() + orig_w * i, grid.back().cbegin() + orig_w * (i + 1),
                           std::back_inserter(grid.back()),
                           [](auto i) { return ++i >= 10 ? 1 : i; });
    }

    grid.reserve(orig_h * 5);

    for (std::size_t i = 0; i < 4; i++) {
        std::transform(grid.cbegin() + orig_h * i, grid.cbegin() + orig_h * (i + 1), std::back_inserter(grid),
                       [](const auto &v) {
                           vec<u64> ret;
                           ret.reserve(v.size());
                           transform(v, std::back_inserter(ret), [](auto i) { return ++i >= 10 ? 1 : i; });
                           return ret;
                       });
    }

    /*for (u64 y = 0; auto &row: grid) {
        if (!(y++ % orig_h)) fmt::print("\n");
        for (u64 x = 0; auto &v: row) {
            if (!(x++ % orig_w)) fmt::print(" ");
            fmt::print("{}", v);
        }
        fmt::print("\n");
    }
    fmt::print("\n");*/

    using coordinate = std::pair<u64, u64>;
    using queue_elem = std::pair<coordinate, u64>;
    using queue_cont = std::vector<queue_elem>;
    auto queue_compare = [](const queue_elem &lhs, const queue_elem &rhs) -> bool {
        return lhs.second > rhs.second;
    };
    std::priority_queue<queue_elem, queue_cont, decltype(queue_compare)> prio_queue{};

    umap<u64, umap<u64, opt<coordinate>>> cameFrom;
    umap<u64, umap<u64, u64>> costs;

    prio_queue.emplace(coordinate{0, 0}, grid[0][0]);
    while (!prio_queue.empty()) {
        auto[current_coord, _] = prio_queue.top();
        prio_queue.pop();
        auto[x, y] = current_coord;

        for (i64 yy = -1; yy <= 1; yy++) {
            for (i64 xx = -1; xx <= 1; xx++) {
                //if (yy == -1 || xx == -1) continue;
                if (y == 0 && yy == -1) continue;
                if (x == 0 && xx == -1) continue;
                if (x + xx >= grid.back().size()) continue;
                if (y + yy >= grid.size()) continue;
                if (xx == 0 && yy == 0) continue;
                if (std::abs(xx) == std::abs(yy)) continue;

                u64 newCost = costs[y][x] + grid[y + yy][x + xx];

                if (!cameFrom[y + yy][x + xx] || newCost < costs[y + yy][x + xx]) {
                    costs[y + yy][x + xx] = newCost;
                    cameFrom[y + yy][x + xx] = coordinate{x, y};
                    prio_queue.emplace(coordinate{x + xx, y + yy}, grid[y + yy][x + xx]);
                }
            }
        }
    }

    coordinate current{grid.size() - 1, grid.size() - 1};
    u64 sum = 0;
    do {
        sum += grid[current.second][current.first];
        current = *cameFrom[current.second][current.first];
        //PrintShit(current.first, current.second);
    } while (current.first != 0 || current.second != 0);

    PrintShit(sum);
}

}
