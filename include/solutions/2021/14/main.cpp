#include "main.hpp"

namespace AOC::Y2021::D14 {

void Sol::Solve(std::string_view data) {
    using namespace Utils::UseThisNamespace;

    auto lines = GetLines(data, "\n");

    vec<pair<std::string_view, char>> rules;

    for (auto it = lines.begin() + 1; it != lines.end(); it++) {
        auto[lhs, rhs] = VecToTuple<2>(GetLines(*it, " -> "));
        rules.emplace_back(lhs, rhs[0]);
    }

    auto Iterate = [](const std::string &s, pair<std::string_view, char> rule) -> std::string {
        std::string ret{};
        opt<char> temp{std::nullopt};

        for (char c: s) {
            if (temp) {
                ret.push_back(*temp);
                if (c == rule.first[1])
                    ret.push_back(rule.second);

                ret.push_back(c);
                temp = std::nullopt;
            } else {
                if (c == rule.first[0]) temp = c;
                else ret.push_back(c);
            }
        }

        return ret;
    };

    std::string str(lines[0]);
    fmt::print("{}\n", str);

    umap<std::string, mp_int> pairMap;
    umap<char, mp_int> occ;
    for (auto it = str.begin(); it != str.end() - 1; it++) {
        std::string tp{it, it + 2};
        ++pairMap[tp];
    }

    for (u64 i = 1; i <= 40; i++) {
        umap<std::string, mp_int> newPairMap{};
        umap<char, mp_int> newOcc;

        for (auto it = pairMap.begin(); it != pairMap.end(); it++) {
            auto &k = it->first;
            auto &v = it->second;
            auto it2 = find_if(rules, [k = k](auto p) { return p.first == k; });
            if (it2 != rules.end()) {
                std::string lhs{"  "}, rhs{"  "};
                lhs[0] = k[0];
                lhs[1] = it2->second;

                rhs[0] = it2->second;
                rhs[1] = k[1];

                newOcc[k[0]] += v;
                newOcc[it2->second] += v;

                newPairMap[lhs] += v;
                newPairMap[rhs] += v;
            } else {
                newPairMap[k] += v;
            }

            auto it3 = it;
            it3++;

            if (it3 == pairMap.end()) {
                //newOcc[it->first[1]] += it->second;
            }
        }

        pairMap = newPairMap;
        occ = newOcc;
    }

    occ[str.back()]++;
    mp_int min = 1, max = 0;
    min <<= 200;
    for (auto &[k, v]: occ) {
        min = std::min(v, min);
        max = std::max(v, max);
    }

    PrintShit(min.str(), max.str(), mp_int(max - min).str()); //3152788426517, 3152788426518

    /*for (u64 i = 1; i <= 30; i++) {
        std::string newStr{};
        for (auto it = str.begin(); it != str.end() - 1; it++) {
            std::string_view tp{it, it + 2};
            auto it2 = find_if(rules, [&tp](auto p) { return p.first == tp; });

            newStr.push_back(*it);
            if (it2 != rules.end()) {
                newStr.push_back(it2->second);
            }
        }

        newStr.push_back(*(str.end() - 1));
        //fmt::print("{}\n", newStr);
        PrintShit(i);
        str = newStr;
    }*/
}

}
/*
NCNBCHB
NCNBCHB
NBCCNBBBCHCB
NBCCNBBBCBHCB
NBBBCNCCNBBNBNBBCHBCHB
NBBNBNBBCCNBCNCCNBBNBBNBBBNBBNBBCHCBBCHCB


 */