#include "main.hpp"

namespace AOC::Y2021::D9 {

bool IsLow(const std::vector<std::vector<u64>> &m, u64 oy, u64 ox) {
    u64 val = m[oy][ox];
    for (i64 y = -1; y < 2; y++) {
        for (i64 x = -1; x < 2; x++) {
            if (oy == 0 && y == -1) continue;
            if (ox == 0 && x == -1) continue;
            if (ox + x >= m.back().size()) continue;
            if (oy + y >= m.size()) continue;
            if (x == 0 && y == 0) continue;
            if (x == y) continue;

            if (m[oy + y][ox + x] <= val) return false;
        }
    }
    return true;
}

u64 Basin(const std::vector<std::vector<u64>> &m, u64 oy, u64 ox) {
    std::unordered_map<u64, std::unordered_map<u64, bool>> traversed;
    traversed[oy][ox] = false;

    std::list<std::tuple<u64, u64, u64>> toTraverse;
    toTraverse.emplace_back(oy, ox, m[oy][ox]);

    u64 sum = 0;

    while (!toTraverse.empty()) {
        auto[y, x, prevSize] = toTraverse.front();
        toTraverse.pop_front();
        if (traversed[y][x]) continue;
        else traversed[y][x] = true;
        //fmt::print("traversing {}, {}\n", y, x);
        //sum += prevSize;
        ++sum;

        for (i64 yy = -1; yy <= 1; yy++) {
            for (i64 xx = -1; xx <= 1; xx++) {
                if (y == 0 && yy == -1) continue;
                if (x == 0 && xx == -1) continue;
                if (x + xx >= m.back().size()) continue;
                if (y + yy >= m.size()) continue;
                if (xx == 0 && yy == 0) continue;
                //if (xx == yy) continue;

                if (!traversed[y + yy][x + xx] && m[y + yy][x + xx] == prevSize + 1 && m[y + yy][x + xx] != 9) {
                    toTraverse.emplace_back(y + yy, x + xx, m[y + yy][x + xx]);
                    traversed[y + yy][x + xx] = false;
                }
            }
        }
    }

    return sum;
}

void Sol::Solve(std::string_view data) {
    using boost::multiprecision::cpp_int;
    using namespace Utils;

    auto lines = GetLines(data, "\n");

    std::vector<std::vector<u64>> m;

    for (auto line: lines) {
        m.push_back({});

        for (auto c: line) {
            m.back().push_back(c - '0');
        }
    }

    u64 sum = 0;

    std::array<std::pair<u64, u64>, 3> largest{{{0, 0}, {0, 0}, {0, 0}}};

    for (u64 y = 0; y < m.size(); y++) {
        for (u64 x = 0; x < m.back().size(); x++) {
            if (IsLow(m, y, x)) {
                sum += (1 + m[y][x]);
                if (u64 b = Basin(m, y, x); b > largest[0].first) largest[0] = {b, m[y][x]};
            }

            std::ranges::sort(largest, [](auto lhs, auto rhs) { return lhs.first < rhs.first; });
            //PrintShit(largest[0], largest[1], largest[2]);
            //            if (u64 b = Basin(m, y, x); b > largest[0].first) largest[0] = {b, m[y][x]};
        }
    }

    PrintShit(sum); //208, 503, 513

    PrintShit(largest[0].first, largest[1].first, largest[2].first); //1550670 110*111*127
    PrintShit(largest[0].first * largest[1].first * largest[2].first);
    PrintShit(largest[0].second, largest[1].second, largest[2].second);
}

}