#include "main.hpp"

#include <shared_mutex>

namespace AOC::Y2021::D20 {
using namespace Utils::UseThisNamespace;

u64 get_index(const umap<i64, umap<i64, u8>> &grid, i64 x, i64 y) {
    u64 result = 0;

    for (i64 yy = -1; yy < 2; yy++) {
        for (i64 xx = -1; xx < 2; xx++) {
            result <<= 1;

            auto row = grid.find(y + yy);
            if (row == grid.cend()) continue;

            auto val = row->second.find(x + xx);
            if (val == row->second.cend()) continue;

            result |= val->second & 1;
        }
    }

    return result;
}

std::pair<std::pair<i64, i64>, std::pair<i64, i64>> get_grid_min_max(const umap<i64, umap<i64, u8>> &grid) {
    constexpr const i64 max = std::numeric_limits<i64>::max();
    i64 minX = max, minY = max, maxX = 0, maxY = 0;

    for (const auto &[y, row]: grid) {
        minY = std::min(y, minY);
        maxY = std::max(y, maxY);

        for (const auto &[x, v]: grid) {
            minX = std::min(x, minX);
            maxX = std::max(x, maxX);
        }
    }

    return {{minX, minY},
            {maxX, maxY}};
}

void print_grid(umap<i64, umap<i64, u8>> &grid) {
    auto[min, max] = get_grid_min_max(grid);
    auto[minX, minY] = min;
    auto[maxX, maxY] = max;

    for (i64 y = minY; y <= maxY; y++) {
        for (i64 x = minX; x <= maxX; x++) {
            fmt::print("{}", (grid[y][x] & 1) ? '#' : '.');
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

void Sol::Solve(std::string_view data) {

    auto lines = GetLines(data, "\n");

    auto rest = std::span<std::string_view>{lines.begin() + 1, lines.end()};

    vec<u8> algo;
    transform(lines.front(), std::back_inserter(algo), [](char c) { return c == '#'; });

    umap<i64, umap<i64, u8>> grid;
    for (i64 y = 1; y < lines.size(); y++) {
        for (i64 x = 0; char c: lines[y])
            grid[y - 1][x++] = c == '#';
    }

    //print_grid(grid);

    i64 height = lines.size() - 1;
    i64 width = lines[1].size();

    i64 steps = 50;
    i64 buffer = steps * 3;

    std::array<std::array<i64, 2>, 2> sim_range
      {{
         {-buffer + 1, width + buffer},
         {-buffer + 1, height + buffer},
       }};

    std::array<std::array<i64, 2>, 2> val_range
      {{
         {-steps, width + steps},
         {-steps, height + steps},
       }};

    std::shared_mutex grid_mutex;
    for (i64 i = 0; i < steps; i++) {
        PrintShit(i);
        auto[min, max] = get_grid_min_max(grid);
        /*i64 minX = min.first;
        i64 minY = min.second;
        i64 maxX = max.first;
        i64 maxY = max.second;*/

        auto[minX, minY] = min;
        auto[maxX, maxY] = max;

//#pragma omp parallel for shared(grid_mutex)
        for (i64 y = sim_range[1][0]; y < sim_range[1][1]; y++) {
            for (i64 x = sim_range[0][0]; x < sim_range[0][1]; x++) {
                /*u64 index;
                {
                    std::shared_lock rd_lock(grid_mutex);
                    index = get_index(grid, x, y);
                }
                {
                    std::unique_lock wr_lock(grid_mutex);
                    grid[y][x] |= (algo[index] & 1) << 1;
                }*/

                const auto index = get_index(grid, x, y);
                grid[y][x] |= (algo[index] & 1) << 1;
            }
        }

        for (auto &[y, row]: grid) {
            for (auto &[x, v]: row) {
                v >>= 1;
            }
        }


    }

    //print_grid(grid);

    u64 sum = 0;
    for (i64 y = val_range[1][0]; y < val_range[1][1]; y++) {
        for (i64 x = val_range[0][0]; x < val_range[0][1]; x++) {
            sum += grid[y][x] & 1;
        }
    }
    PrintShit(sum); //20330
}

}
