#include "main.hpp"

#include <unordered_set>

namespace AOC::Y2021::D8 {

void Sol::Solve(std::string_view data) {
    using boost::multiprecision::cpp_int;
    using namespace Utils;

    auto fl = GetLines(data, "\n");

    using ss_t = std::array<bool, 7>;
    auto PrintSST = [](const ss_t &ss) {
        for (auto b: ss) {
            if (b) fmt::print("1");
            else fmt::print("0");
        }
    };

    auto CountSS = [](const ss_t &ss) -> u64 {
        return std::count(ss.cbegin(), ss.cend(), true);
    };

    auto GetSSIndices = [](const ss_t &ss) -> std::vector<std::size_t> {
        std::vector<std::size_t> ret;
        for (std::size_t i = 0; i < 7; i++) if (ss[i]) ret.push_back(i);
        return ret;
    };

    auto SSToNum = [](const ss_t &ss) -> u64 {
        u16 n = 0;
        for (std::size_t i = 0; i < 7; i++) {
            n |= static_cast<u16>(ss[i]) << (7 - i - 1);
        }

        switch (n) {
            case 0b1110111:
                return 0;
            case 0b0010010:
                return 1;
            case 0b1011101:
                return 2;
            case 0b1011011:
                return 3;
            case 0b0111010:
                return 4;
            case 0b1101011:
                return 5;
            case 0b1101111:
                return 6;
            case 0b1010010:
                return 7;
            case 0b1111111:
                return 8;
            case 0b1111011:
                return 9;
            default:
                return 10;
        }
    };

    std::vector<std::pair<std::array<ss_t, 10>, std::array<ss_t, 4>>> decode;

    for (auto l: fl) {
        auto[seg0, seg1] = VecToTuple<2>(GetLines(l, " | "));
        auto seg0v = GetLines(seg0, " ");
        auto seg1v = GetLines(seg1, " ");

        std::array<ss_t, 10> lhs{};
        std::array<ss_t, 4> rhs{};

        for (std::size_t i = 0; i < 10; i++) {
            for (auto c: seg0v[i]) lhs[i][c - 'a'] = true;
        }

        for (std::size_t i = 0; i < 4; i++) {
            for (auto c: seg1v[i]) rhs[i][c - 'a'] = true;
        }

        decode.emplace_back(lhs, rhs);
    }

    /*for (const auto &[lhs, rhs]: decode) {
        for (const auto &l: lhs) {
            PrintSST(l);
            fmt::print(" ");
        }
        fmt::print(" | ");
        for (const auto &r: rhs) {
            PrintSST(r);
            fmt::print(" ");
        }
        fmt::print("\n");
    }*/

    using map_t = std::array<u64, 7>;

    map_t defmap{};
    for (std::size_t i = 0; i < 7; i++) defmap[i] = i;

    auto Eq = [](map_t l, map_t r) {
        for (std::size_t i = 0; i < 7; i++) if (l[i] != r[i]) return false;
        return true;
    };

    auto Permute = [](map_t &remap) {
        std::next_permutation(remap.begin(), remap.end());
    };

    auto GetMapped = [](ss_t v, map_t m) {
        ss_t r;
        for (std::size_t i = 0; i < 7; i++)
            r[m[i]] = v[i];
        return r;
    };

    u64 sum = 0;

    for (auto &[lhs, rhs]: decode) {
        /*for (const auto &r: rhs) {
            if (auto c = std::count(r.cbegin(), r.cend(), true); c == 2 || c == 3 || c == 4 || c == 7) {
                ++sum;
            }
        }*/

        //1 2
        //4 4
        //7 3
        //8 7

/*        using remap_t = std::pair<u32, u32>;
        std::unordered_set<remap_t> possibleRemaps{};

        for (const auto &l: lhs) {
            auto indices = GetSSIndices(l);
            std::vector<std::size_t> candidates{};
            auto s = indices.size();

            switch (s) {
                case 2:
                    candidates = {2, 5};
                    break;
                case 3:
                    candidates = {0, 2, 5};
                    break;
                case 4:
                    candidates = {1, 2, 3, 5};
                    break;
                case 7:
                    candidates = {0, 1, 2, 3, 4, 5, 6};
                    break;
                default:
                    break;
            }

            //cartesian mult
            if (!candidates.empty())
                for (std::size_t i = 0; i < s; i++)
                    for (std::size_t j = 0; j < s; j++)
                        possibleRemaps.insert(remap_t{indices[i], candidates[j]});

            //remove inconsistent remaps

        }*/
        bool first = true;
        map_t mapping = defmap;
        for (;;) {
            if (!first) {
                if (Eq(mapping, defmap)) throw std::runtime_error("no valid remap");
            } else first = false;

            bool valid = true;
            for (const auto &l: lhs) {
                std::unordered_map<std::size_t, u64> iCountToNum{{2, 1},
                                                                 {4, 4},
                                                                 {3, 7},
                                                                 {7, 8}};
                if (auto it = iCountToNum.find(GetSSIndices(l).size());
                  it != iCountToNum.end()) {

                    if (auto n = SSToNum(GetMapped(l, mapping)); n == 10 || n != it->second) {
                        valid = false;
                        break;
                    }
                }
            }

            if (valid) {
                for (const auto &l: lhs) {
                    if (SSToNum(GetMapped(l, mapping)) == 10) {
                        valid = false;
                        break;
                    }
                }
            }

            if (!valid) {
                Permute(mapping);
                continue;
            }

            //throw std::runtime_error("asdasd");

            /*for (auto &r: rhs) {
                fmt::print("{}", SSToNum(GetMapped(r, mapping)));
            }
            fmt::print("\n");
            break;*/

            u64 temp = 0;
            for (u64 i = 0; i < 4; i++) {
                temp += SSToNum(GetMapped(rhs[i], mapping)) * std::pow<u64>(10, 4 - i - 1);
            }
            //PrintShit(temp);
            sum += temp;

            break;
        }
    }
    PrintShit(sum);

}

}