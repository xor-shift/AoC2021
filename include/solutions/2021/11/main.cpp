#include "main.hpp"

namespace AOC::Y2021::D11 {

u64 Tick(std::vector<std::vector<u64>> &v) {
    std::vector<std::vector<int_fast8_t>> flashed(v.size(), std::vector<int_fast8_t>(v.back().size(), 0));

    for (u64 y = 0; y < v.size(); y++) {
        for (u64 x = 0; x < v.back().size(); x++) {
            const auto val = v[y][x];
            v[y][x] = val + 1;
        }
    }

    //process flashes

    u64 flashes = 0, lastFlashes = 0;
    bool p = false;
    while (!p || lastFlashes) {
        lastFlashes = 0;
        p = true;

        for (u64 y = 0; y < v.size(); y++) {
            for (u64 x = 0; x < v.back().size(); x++) {
                if (flashed[y][x]) continue;
                if (v[y][x] <= 9) continue;
                v[y][x] = 0;
                ++lastFlashes;
                flashed[y][x] = 1;

                for (i64 yy = -1; yy <= 1; yy++) {
                    for (i64 xx = -1; xx <= 1; xx++) {
                        if (x + xx >= v.back().size()) continue;
                        if (y + yy >= v.size()) continue;
                        if (xx == 0 && yy == 0) continue;
                        if (flashed[y + yy][x + xx]) continue;
                        v[y + yy][x + xx] += 1;
                    }
                }
            }
        }

        flashes += lastFlashes;
    }

    return flashes;
}

void Print(const std::vector<std::vector<u64>> &v) {
    for (u64 y = 0; y < v.size(); y++) {
        for (u64 x = 0; x < v.back().size(); x++) {
            fmt::print("{}", v[y][x]);
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

void FirstSolution(std::string_view data) {
    using namespace Utils::UseThisNamespace;

    auto lines = GetLines(data, "\n");

    std::vector<std::vector<u64>> v;

    for (auto line: lines) {
        v.emplace_back();
        for (char c: line) v.back().push_back(c - '0');
    }

    u64 f = 0;
    for (u64 i = 0; i < 1000000; i++) {
        u64 t;
        f += (t = Tick(v));

        if (t == v.size() * v.back().size()) {
            PrintShit("a", i); // 350, 349
            std::abort();
        }

        if (i == 99) PrintShit(f);
    }
}

void Sol::Solve(std::string_view data) {
    FirstSolution(data);
}

}