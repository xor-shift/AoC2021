#include <queue>
#include "main.hpp"

/***
 * GAVE UP AND SOLVED ON PEN AND PAPER AFTER MORE THAN AN HOUR
 */

namespace AOC::Y2021::D23 {
using namespace Utils::UseThisNamespace;

i64 energy_required(char type) {
    return std::pow(10, type - 'A');
}

enum class ETile : i64 {
    Free = 0,
    Wall = 2,
    Amber = 1,
    Bronze = 10,
    Copper = 100,
    Desert = 1000,
};

char tile_to_char(ETile tile) {
    if (tile == ETile::Free) return '.';
    if (tile == ETile::Wall) return '#';
    return 'A' + static_cast<char>(std::log10(static_cast<i64>(tile)));
}

template<typename T>
using GridBase = std::array<std::array<T, 13>, 5>;

using Grid = GridBase<ETile>;

bool is_settled(const Grid &grid, std::size_t x, std::size_t y) {
    const auto &tile = grid[y][x];

    if (tile == ETile::Free || tile == ETile::Wall) return true;

    auto pow = std::log10(static_cast<i64>(tile));
    pow *= 2;

    auto expected_row_x_pos = pow + 3;
    if (x == expected_row_x_pos) {
        bool all_same = true;
        for (std::size_t i = 3; i < grid.size() - 1; i++)
            if (grid[i][x] != tile) all_same = false;
        return all_same;

    }

    return false;
}

bool operator==(const Grid &lhs, const Grid &rhs) {
    for (std::size_t i = 0; i < lhs.size(); i++) {
        if (lhs[i] != rhs[i]) return false;
    }
    return true;
}

void Sol::Solve(std::string_view data) {
    Grid grid;

    ProcessLines(data, "\n", [&grid](std::string_view line, std::size_t line_no) {
        for (std::size_t x = 0; x < grid[line_no].size(); x++) {
            char c = x < line.size() ? line[x] : ' ';
            auto &tile = grid[line_no][x];

            if (c == '#' || c == ' ') tile = ETile::Wall;
            else if (c == '.') tile = ETile::Free;
            else tile = static_cast<ETile>(energy_required(c));
        }
    });

    Grid target = grid;
    target[2][3] = ETile::Amber;
    target[3][3] = ETile::Amber;
    target[2][5] = ETile::Bronze;
    target[3][5] = ETile::Bronze;
    target[2][7] = ETile::Copper;
    target[3][7] = ETile::Copper;
    target[2][9] = ETile::Desert;
    target[3][9] = ETile::Desert;

    /*for (std::size_t y = 0; y < grid.size(); y++) {
        for (std::size_t x = 0; x < grid[y].size(); x++) {
            if (is_settled(grid, x, y)) std::putchar('x');
            else std::putchar(tile_to_char(grid[y][x]));
        }
        std::putchar('\n');
    }*/

    using QueueElement = std::pair<Grid, i64>;
    using QueueContainer = std::vector<QueueElement>;
    auto QueueCompare = [](const auto &lhs, const auto &rhs) {
        return lhs.second > rhs.second;
    };

    struct GridHash {
        std::size_t operator()(const Grid &grid) const {
            std::string res;
            for (const auto &row : grid)
                for (const auto &v : row)
                    res += fmt::to_string(v);
            return std::hash<std::string>{}(res);
        }
    };

    std::priority_queue<QueueElement, QueueContainer, decltype(QueueCompare)> prio_queue{};
    std::unordered_map<Grid, i64, GridHash> costs;
    prio_queue.emplace(grid, 0);

    while (!prio_queue.empty()) {
        auto[current_grid, cost_until_here] = prio_queue.top();
        prio_queue.pop();

        for (i64 y = 0; y < current_grid.size(); y++) {
            for (i64 x = 0; x < current_grid[y].size(); x++) {
                const auto &tile = current_grid[y][x];

                if (is_settled(current_grid, x, y)) continue;

                //look for free spots and swap places
                for (i64 yy = -1; yy <= 1; yy++) {
                    for (i64 xx = -1; xx <= 1; xx++) {
                        if (std::abs(xx) == std::abs(yy)) continue;
                        if (xx == 0 && yy == 0) continue;
                        if ((y == 0 && yy == -1) || (x == 0 && xx == -1)) continue;
                        if (y + yy >= current_grid.size() || x + xx >= current_grid[y].size()) continue;

                        const auto &to_swap_with = current_grid[y+yy][x+xx];
                        if (to_swap_with != ETile::Free) continue;

                        const auto cost_to_swap = static_cast<i64>(tile);
                        const auto total_cost_for_next = cost_to_swap + cost_until_here;

                        Grid current_copy = current_grid;
                        auto temp = current_copy[y][x];
                        current_copy[y][x] = current_copy[y+yy][x+xx];
                        current_copy[y+yy][x+xx] = temp;

                        auto existing_cost_it = costs.find(current_copy);
                        if (existing_cost_it == costs.end() || existing_cost_it->second > total_cost_for_next) {
                            prio_queue.emplace(current_copy, total_cost_for_next);
                        }

                        if (prio_queue.size() > 128ull * 1024ull) std::abort();
                    }
                }
            }
        }
    }

    PrintShit(costs[target]);
}

}
