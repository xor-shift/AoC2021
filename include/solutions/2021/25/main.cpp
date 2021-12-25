#include "main.hpp"

namespace AOC::Y2021::D25 {
using namespace Utils::UseThisNamespace;

enum class ECell {
    Free,
    South,
    East,
};

using Grid = std::vector<std::vector<ECell>>;

std::pair<std::size_t, std::size_t> get_grid_dims(const Grid &grid) {
    std::size_t h = grid.size();
    std::size_t w = grid.back().size();

    return {w, h};
}

std::pair<u64, u64> get_next_coord(const Grid &grid, std::size_t x, std::size_t y, bool south) {
    auto[w, h] = get_grid_dims(grid);
    return {(x + (south ? 0 : 1)) % w, (y + (south ? 1 : 0)) % h};
}

/// returns the new position
std::optional<std::pair<u64, u64>> get_movement(const Grid &grid, std::size_t x, std::size_t y) {
    const auto &cell = grid[y][x];
    if (cell == ECell::Free) return std::nullopt;

    auto[xx, yy] = get_next_coord(grid, x, y, cell == ECell::South);
    const auto &target_cell = grid[yy][xx];
    if (target_cell != ECell::Free) return std::nullopt;

    return std::pair<u64, u64>{xx, yy};
}

Grid tick_grid(const Grid &grid, ECell type_to_tick) {
    auto[w, h] = get_grid_dims(grid);

    Grid next_state{h, std::vector<ECell>{w, ECell::Free}};

    std::unordered_set<std::tuple<std::size_t, std::size_t>, Utils::TupleHasher<std::size_t, std::size_t>> skip;

    for (std::size_t y = 0; y < h; y++) {
        for (std::size_t x = 0; x < w; x++) {
            if (skip.contains({x, y})) continue;

            if (grid[y][x] != type_to_tick) {
                next_state[y][x] = grid[y][x];
                continue;
            }

            auto movement = get_movement(grid, x, y);
            if (!movement) {
                next_state[y][x] = grid[y][x];
            } else {
                auto[xx, yy] = *movement;

                next_state[y][x] = ECell::Free;
                next_state[yy][xx] = grid[y][x];
                skip.insert({xx, yy});
            }
        }
    }

    return next_state;
}

void print_grid(const Grid &grid) {
    auto[w, h] = get_grid_dims(grid);

    for (std::size_t y = 0; y < h; y++) {
        for (std::size_t x = 0; x < w; x++) {
            switch (grid[y][x]) {
                case ECell::Free:
                    std::putchar('.');
                    break;
                case ECell::South:
                    std::putchar('v');
                    break;
                case ECell::East:
                    std::putchar('>');
                    break;
                default:
                    std::abort();
            }
        }
        std::putchar('\n');
    }
    std::putchar('\n');
}

bool grids_equal(const Grid &lhs, const Grid &rhs) {
    auto[w, h] = get_grid_dims(lhs);

    for (std::size_t i = 0; i < h; i++)
        if (lhs[i] != rhs[i]) return false;

    return true;
}

void Sol::Solve(std::string_view data) {
    Grid grid;
    ProcessLines(data, "\n", [&grid](std::string_view line, std::size_t) {
        grid.emplace_back();

        for (char c: line) {
            grid.back().push_back(c == '.' ? ECell::Free : c == 'v' ? ECell::South : ECell::East);
        }
    });

    print_grid(grid);
    for (std::size_t i = 1;; i++) {
        auto prev = grid;
        grid = tick_grid(grid, ECell::East);
        grid = tick_grid(grid, ECell::South);

        if (grids_equal(prev, grid)) {
            fmt::print("{}\n", i);
            break;
        }
    }
    print_grid(grid);
}

}
