#include "main.hpp"

namespace AOC::Y2021::D13 {

void Sol::Solve(std::string_view data) {
    using namespace Utils::UseThisNamespace;

    auto lines = GetLines(data, "\n");

    auto it = find_if(lines, [](auto line) { return line.starts_with("fold"); });

    auto dotsL = std::span{lines.begin(), it};
    auto foldsL = std::span{it, lines.end()};

    vec<pair<u64, u64>> dots;
    for (auto dotL: dotsL) {
        dots.push_back(Map(ToPair(VecToTuple<2>(GetLines(dotL, ","))), [](auto v) { return Convert<u64>(v, 10); }));
    }

    vec<pair<bool, u64>> folds;

    for (auto foldL: foldsL) {
        auto it = find(foldL, '=');
        folds.emplace_back(*(it - 1) == 'y', Convert<u64>({it + 1, foldL.end()}, 10));
    }

    umap<u64, umap<u64, bool>> grid;

    auto Dims = [&grid]() {
        u64 width = 0, height = 0;
        for (auto &[y, row]: grid) {
            for (auto &[x, val]: row) {
                if (!val) continue;
                width = std::max(x + 1, width);
                height = std::max(y + 1, height);
            }
        }
        return std::make_pair(width, height);
    };

    for (auto[x, y]: dots) {
        grid[y][x] = true;
    }

    auto Fold = [&](bool axis, u64 at) {
        //up if axis, left if not
        vec<pair<u64, u64>> toSetToTrue{};

        for (auto &[y, row]: grid) {
            for (auto &[x, val]: row) {
                bool b = false;
                b |= axis && y > at;
                b |= !axis && x > at;

                if (!b) continue;

                if (axis) {
                    if (val) toSetToTrue.emplace_back(x, at - (y - at));
                } else {
                    if (val) toSetToTrue.emplace_back(at - (x - at), y);
                }

                val = false;
            }
        }

        for (auto[x, y]: toSetToTrue) grid[y][x] = true;
    };

    auto Print = [&]() {
        auto[w, h] = Dims();
        for (u64 y = 0; y < h; y++) {
            for (u64 x = 0; x < w; x++) {
                fmt::print("{}", grid[y][x] ? '#' : '.');
            }
            fmt::print("\n");
        }
        fmt::print("\n");
    };


    for (auto[axis, at]: folds) {
        Fold(axis, at);
    }

    //Fold(folds[0].first, folds[1].second);

    u64 sum = 0;
    for (auto &[y, row]: grid) {
        for (auto &[x, val]: row) {
            sum += val;
        }
    }
    PrintShit(sum);
}

}
