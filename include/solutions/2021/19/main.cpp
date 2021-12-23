#include "main.hpp"

namespace AOC::Y2021::D19 {
using namespace Utils::UseThisNamespace;

using Coord = arr<i64, 3>;
using Rotation = arr<int, 3>;

using Scanner = uset<Coord, ArrayHasher<i64, 3>>;

u64 fac(u64 v) {
    u64 p = 1;
    while (v > 1) p *= v--;
    return p;
}

u64 choose(u64 a, u64 b) {
    return fac(a) / (fac(b) * fac(a - b));
}

template<typename T, typename U>
T pairwise_op(const T &lhs, const U &rhs, auto &&op) {
    T ret;
    for (u64 i = 0; i < lhs.size(); i++) ret[i] = std::invoke(op, lhs[i], rhs[i]);
    return ret;
}

Coord apply_rotation(Coord coordinate, arr<int, 3> rotation) {
    Coord ret{
      coordinate[std::abs(rotation[0]) - 1],
      coordinate[std::abs(rotation[1]) - 1],
      coordinate[std::abs(rotation[2]) - 1],
    };

    return pairwise_op(ret, rotation, [](auto l, auto r) { return l * (r < 0 ? -1 : 1); });
}

Scanner apply_rotation(const Scanner &scanner, Rotation rotation) {
    Scanner ret;

    for (const auto &v: scanner) {
        ret.insert(apply_rotation(v, rotation));
    }

    return ret;
}

Rotation get_rotation(int rot_id) {
    static const vec<Rotation> rots{
      {1,  2,  3},
      {1,  -3, 2},
      {1,  -2, -3},
      {1,  3,  -2},
      {-2, 1,  3},
      {3,  1,  2},
      {2,  1,  -3},
      {-3, 1,  -2},
      {-1, -2, 3},
      {-1, -3, -2},
      {-1, 2,  -3},
      {-1, 3,  2},
      {2,  -1, 3},
      {3,  -1, -2},
      {-2, -1, -3},
      {-3, -1, 2},
      {-3, 2,  1},
      {2,  3,  1},
      {3,  -2, 1},
      {-2, -3, 1},
      {-3, -2, -1},
      {-2, 3,  -1},
      {3,  2,  -1},
      {2,  -3, -1}
    };

    return rots[rot_id];
}

/**
 *
 * @param reference
 * @param scanner
 * @return the offset of scanner in relation to reference and the offset-corrected scanner
 */
std::optional<std::pair<Coord, Scanner>> try_resolve(const Scanner &reference, const Scanner &scanner) {
    std::unordered_map<Coord, u64, ArrayHasher<i64, 3>> deltas_list{};

    for (const auto &r_p: reference) {
        for (const auto &s_p: scanner) {
            Coord delta = pairwise_op(r_p, s_p, std::minus{});
            deltas_list[delta]++;
        }
    }

    std::vector<std::pair<Coord, u64>> extracted{deltas_list.cbegin(), deltas_list.cend()};
    std::ranges::sort(extracted, [](auto lhs, auto rhs) { return lhs.second < rhs.second; });

    for (const auto &[offset, offset_count]: deltas_list) {
        if (offset_count < 12) continue;

        fmt::print("{},{},{}\n", offset[0], offset[1], offset[2]);

        Scanner offset_scanner{};
        for (const auto &point: scanner) {
            offset_scanner.insert(pairwise_op(point, offset, std::plus{}));
        }

        for (const auto &p: offset_scanner) {
            //fmt::print("{},{},{}\n", p[0], p[1], p[2]);
        }

        Scanner union_scanner = reference;
        std::copy(offset_scanner.cbegin(), offset_scanner.cend(), std::inserter(union_scanner, union_scanner.begin()));

        if ((union_scanner.size() + 12) <= (reference.size() + scanner.size())) {
            return std::make_pair(offset, offset_scanner);
        }
    }

    return std::nullopt;
}

std::optional<std::tuple<int, Coord, Scanner>> try_resolve_rot(const Scanner &reference, const Scanner &scanner) {
    for (int rot_id = 0; rot_id < 24; rot_id++) {
        Scanner cur_scanner = apply_rotation(scanner, get_rotation(rot_id));

        const auto resolution = try_resolve(reference, cur_scanner);
        if (!resolution) continue;

        const auto[offset, corrected] = *resolution;

        return std::make_tuple(rot_id, offset, corrected);
    }
    return std::nullopt;
}

void Sol::Solve(std::string_view data) {
    auto lines = GetLines(data, "\n");
    vec<Scanner> scanners;
    for (auto line: lines) {
        if (line.starts_with("---")) {
            scanners.emplace_back();
            continue;
        }

        auto[x, y, z] = Map(VecToTuple<3>(GetLines(line, ",")), [](auto l) { return Convert<i64>(l, 10); });
        scanners.back().insert({x, y, z});
    }

    uset<std::size_t> resolved_scanner_indices{0};
    vec<Scanner> resolved_scanners{scanners[0]};

    std::vector<Coord> offsets;

    while (resolved_scanner_indices.size() != scanners.size())
        for (std::size_t i = 0; i < scanners.size(); i++) {
            if (resolved_scanner_indices.contains(i)) continue;
            fmt::print("testing scanner {}\n", i);
            for (const auto &reference_scanner: resolved_scanners) {
                const auto resolution = try_resolve_rot(reference_scanner, scanners[i]);
                if (!resolution) continue;

                const auto[res_rot, res_offset, res_scanner] = *resolution;
                resolved_scanner_indices.insert(i);
                resolved_scanners.push_back(std::move(res_scanner));
                fmt::print("resolved scanner {} with offset={},{},{} rot_id={}\n",
                           i,
                           res_offset[0], res_offset[1], res_offset[2],
                           res_rot);
                offsets.push_back(res_offset);
                break;
            }
        }

    fmt::print("{}\n", resolved_scanners.size());

    Scanner union_scanner;
    for (const auto &s: resolved_scanners) {
        std::copy(s.cbegin(), s.cend(), std::inserter(union_scanner, union_scanner.begin()));
    }

    for (auto &p: union_scanner) {
        fmt::print("{}, {}, {}\n", p[0], p[1], p[2]);
    }

    std::vector<i64> distances{};
    for (std::size_t i = 0; i < offsets.size(); i++) {
        for (std::size_t j = i + 1; j < offsets.size(); j++) {
            i64 distance = 0;
            for (std::size_t dim = 0; dim < 3; dim++)
                distance += std::abs(offsets[j][dim] - offsets[i][dim]);
            distances.push_back(distance);
        }
    }
    sort(distances);

    fmt::print("part 1: {}\n", union_scanner.size());
    fmt::print("part 2: {}\n", distances.back());
}

}
