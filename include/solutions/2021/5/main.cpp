#include "main.hpp"

#include <execution>
#include <mutex>

namespace AOC::Y2021::D5 {

void FirstSolution(std::string_view data) {
    auto file = Utils::GetLines(data, "\n");

    typedef std::pair<std::ptrdiff_t, std::ptrdiff_t> Coordinate;
    typedef std::pair<Coordinate, Coordinate> Line;
    std::vector<Line> lines{};

    for (std::ptrdiff_t i = 0; auto line: file) {
        ++i;
        auto[s1, s2] = Utils::VecToTuple<2>(Utils::GetLines(line, " -> "));
        auto[x0, y0] = Utils::VecToTuple<2>(Utils::GetLines(s1, ','));
        auto[x1, y1] = Utils::VecToTuple<2>(Utils::GetLines(s2, ','));

        lines.push_back({{Utils::Convert<std::ptrdiff_t>(x0),
                           Utils::Convert<std::ptrdiff_t>(y0)},
                         {Utils::Convert<std::ptrdiff_t>(x1),
                           Utils::Convert<std::ptrdiff_t>(y1)}});
    }

    Coordinate max{0, 0};
    for (auto l: lines) {
        auto[c0, c1] = l;
        max.first = std::max(max.first, std::max(c0.first, c1.first));
        max.second = std::max(max.second, std::max(c0.second, c1.second));
        //Utils::PrintShit(c0.first, c0.second, c1.first, c1.second);
    }

    const auto cols = max.first + 1, rows = max.second + 1;
    Utils::PrintShit("dims", cols, rows);
    std::vector<std::ptrdiff_t> map(rows * cols, 0);
    for (auto l: lines) {
        if (l.first.first == l.second.first) {
            //Utils::PrintShit(l.first.first, l.second.first, l.first.second, l.second.second);
            bool direction = l.second.second - l.first.second > 0;
            for (std::ptrdiff_t y = 0; y <= std::abs(l.second.second - l.first.second); y++) {
                auto o = direction ? y : -y;
                ++map[l.first.first + (l.first.second + o) * cols];
            }
        } else if (l.first.second == l.second.second) {
            //Utils::PrintShit(l.first.first, l.second.first, l.first.second, l.second.second);
            bool direction = l.second.first - l.first.first > 0;
            for (std::ptrdiff_t x = 0; x <= std::abs(l.second.first - l.first.first); x++) {
                auto o = direction ? x : -x;
                ++map[l.first.first + o + l.first.second * cols];
            }
        } else {
            //Utils::PrintShit(l.first.first, l.first.second, l.second.first, l.second.second);
            bool xdir = l.first.first < l.second.first;
            bool ydir = l.first.second < l.second.second;
            auto start = l.first;
            auto length = std::abs(l.first.first - l.second.first);
            //Utils::PrintShit(length, xdir, ydir, start.first, start.second);

            for (auto i = 0; i <= length; i++) {
                auto xo = xdir ? i : -i;
                auto yo = ydir ? i : -i;
                //Utils::PrintShit(start.first + xo, start.second + yo);
                ++map[(start.first + xo) + (start.second + yo) * cols];
            }
        }
    }

    Utils::PrintShit(std::count_if(map.cbegin(), map.cend(), [](auto v) { return v > 1; }));

    /*for (auto row = 0; row < rows; row++) {
        for (auto col = 0; col < cols; col++) {
            fmt::print("{}", map[col + row * cols]);
        }
        fmt::print("\n");
    }*/
}

void Refactor(std::string_view data) {
    constexpr bool parallel = false;
    using execution_t = std::conditional_t<parallel, std::execution::parallel_unsequenced_policy, std::execution::sequenced_policy>;
    const auto execution = execution_t{};

    std::mutex spotsMutex{};
    std::unordered_map<u64, std::pair<u64, u64>> occupiedSpots;

    auto ToIndex = [](u32 x, u32 y) -> u64 { return (static_cast<u64>(y) << 32) | x; };

    auto ParseLine = [ToIndex](std::string_view l) {
        auto[start, end] = Utils::Map(Utils::Map(
          Utils::ToPair(Utils::GetLines(l, " -> ")),
          [](const auto &v) { return Utils::ToPair(Utils::GetLines(v, ',')); }
        ), [](const auto &v) { return Utils::Convert<u32>(v); });

        std::array<std::array<i32, 2>, 2> line{{{static_cast<int>(start.first), static_cast<int>(start.second)},
                                                {static_cast<int>(end.first), static_cast<int>(end.second)}}};

        u64 startIdx, step, count;
        bool diag;

        if (diag = !(line[0][0] == line[1][0] || line[0][1] == line[1][1]); !diag) {
            const bool xAxis = line[0][1] == line[1][1];
            const auto s = line[1][!xAxis] > line[0][!xAxis] ? start : end;
            startIdx = ToIndex(s.first, s.second);
            step = xAxis ? 1 : (1ull << 32);
            count = std::abs(line[0][!xAxis] - line[1][!xAxis]);
        } else {
            count = std::abs(line[0][0] - line[1][0]);
            if (line[0][1] > line[1][1]) std::swap(line[0], line[1]); //always going downwards
            bool right = line[0][0] < line[1][0];
            step = (1ll << 32) + (right ? 1ll : -1ll);
            startIdx = ToIndex(line[0][0], line[0][1]);
        }

        return std::make_tuple(startIdx, step, count, diag);
    };

    auto lines = Utils::GetLines(data, "\n");
    std::for_each(execution, lines.cbegin(), lines.cend(), [ParseLine, &occupiedSpots, &spotsMutex](auto l) {
        auto [startIdx, step, count, diag] = ParseLine(l);

        for (u64 i = 0; i <= count; i++) {
            std::unique_lock<std::mutex> lock;
            if constexpr(parallel) {
                std::unique_lock temp(spotsMutex);
                lock.swap(temp);
            }
            auto &t = occupiedSpots[startIdx];
            if (!diag) ++t.first;
            ++t.second;
            startIdx += step;
        }
    });

    auto[p1, p2] = std::reduce(execution, occupiedSpots.cbegin(), occupiedSpots.cend(), std::pair<u64, u64>{0, 0}, [](auto sum, auto it) {
        sum.first += it.second.first > 1;
        sum.second += it.second.second > 1;
        return sum;
    });

    fmt::print("{}, {}", p1, p2);
}

void Sol::Solve(std::string_view data) {
    Refactor(data);
}

}