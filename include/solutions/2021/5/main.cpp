#include "main.hpp"

namespace AOC::Y2021::D5 {

void Sol::Solve(std::string_view data) {
    auto file = Utils::GetLines(data, "\n");

    typedef std::pair<std::ptrdiff_t, std::ptrdiff_t> Coordinate;
    typedef std::pair<Coordinate, Coordinate> Line;
    std::vector<Line> lines{};

    for (std::ptrdiff_t i = 0; auto line: file) {
        ++i;
        auto[s1, s2] = Utils::GetNLines<2>(line, " -> ");
        auto[x0, y0] = Utils::GetNLines<2>(s1, ',');
        auto[x1, y1] = Utils::GetNLines<2>(s2, ',');

        lines.push_back({{Utils::Convert<std::ptrdiff_t>(x0, 0),
                           Utils::Convert<std::ptrdiff_t>(y0, 0)},
                         {Utils::Convert<std::ptrdiff_t>(x1, 0),
                           Utils::Convert<std::ptrdiff_t>(y1, 0)}});
    }

    Coordinate max{0, 0};
    for (auto l : lines) {
        auto[c0, c1] = l;
        max.first = std::max(max.first, std::max(c0.first, c1.first));
        max.second = std::max(max.second, std::max(c0.second, c1.second));
        //Utils::PrintShit(c0.first, c0.second, c1.first, c1.second);
    }

    const auto cols = max.first + 1, rows = max.second + 1;
    Utils::PrintShit("dims", cols, rows);
    std::vector<std::ptrdiff_t> map(rows * cols, 0);
    for (auto l : lines) {
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

    Utils::PrintShit(std::count_if(map.cbegin(),  map.cend(), [](auto v) { return v > 1; }));

    /*for (auto row = 0; row < rows; row++) {
        for (auto col = 0; col < cols; col++) {
            fmt::print("{}", map[col + row * cols]);
        }
        fmt::print("\n");
    }*/
}

}