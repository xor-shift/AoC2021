#include "main.hpp"

namespace AOC::Y2021::D19 {
using namespace Utils::UseThisNamespace;

using Coord = arr<i64, 3>;
using Rotation = arr<int, 3>;

struct coord_hash {
    std::size_t operator()(const Coord &coord) const {
        auto str = fmt::format("{}{}{}", coord[0], coord[1], coord[2]);
        return std::hash<std::string>{}(str);
    }
};

using Scanner = uset<Coord, coord_hash>;

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
    for (auto &v : scanner) {
        //v = apply_rotation(v, rotation);
    }
}

Rotation get_rotation(int rot_id) {
    static const vec<Rotation> rots{
      {2,  1,  -3},
      {2,  3,  1},
      {2,  -1, 3},
      {2,  -3, -1},
      {-3, 2,  1},
      {1,  2,  3},
      {3,  2,  -1},
      {-1, 2,  -3},
      {3,  1,  2},
      {-1, 3,  2},
      {-3, -1, 2},
      {1,  -3, 2},
      {-1, 3,  -2},
      {-3, -1, -2},
      {1,  -3, -2},
      {3,  1,  -2},
      {-2, 1,  3},
      {-2, -3, 1},
      {-2, -1, -3},
      {-2, 3,  -1},
      {3,  -2, 1},
      {1,  -2, -3},
      {-3, -2, -1},
      {-1, -2, 3},
    };

    return rots[rot_id];
}

std::optional<int> try_resolve(const Scanner &reference, Scanner &scanner) {

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

    uset<std::size_t> resolved_scanners{0};

    /*vec<vec<arr<long double, 3>>> centers;
    for (auto &scanner: scanners) {
        centers.emplace_back();

        auto num_elems = scanner.size();
        std::vector<u64> indices(num_elems, 0);
        std::iota(indices.begin(), indices.end(), 1);

        u64 perm = choose(num_elems, 12);
        for (u64 i = 0; i < perm; i++) {
            arr<long double, 3> sum{0,0,0};
            for (u64 j = 0; j < 12; j++) {
                sum[0] += scanner[indices[j]][0];
                sum[1] += scanner[indices[j]][1];
                sum[2] += scanner[indices[j]][2];
            }
        }
    }*/
}

}
