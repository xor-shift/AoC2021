#include "main.hpp"

namespace AOC::Y2021::D12 {

using namespace Utils::UseThisNamespace;

using Nodes = umap<std::string_view, std::vector<std::string_view>>;

void TraverseImpl(const Nodes &nodes, std::string_view from, umap<std::string_view, int> traversed, u64 &ends) {
    ++traversed[from];

    const auto &nbrs = nodes.at(from);
    for (auto nbr: nbrs) {
        if (nbr == "end") {
            ++ends;
        } else if (nbr != "start") {
            auto f = std::ranges::find_if(traversed, [](auto a) {
                if (std::islower(a.first[0])) {
                    return a.second >= 2;
                }
                return false;
            });
            int a = (f == traversed.end());

            if (std::isupper(nbr[0]) || traversed[nbr] < (1 + a)) {
                TraverseImpl(nodes, nbr, traversed, ends);
            }
        }
    }
}

u64 Traverse(const Nodes &nodes, std::string_view node) {
    u64 res = 0;

    umap<std::string_view, int> traversed;
    TraverseImpl(nodes, node, traversed, res);

    return res;
}

void Sol::Solve(std::string_view data) {
    auto lines = GetLines(data, "\n");

    Nodes graph;

    for (auto line: lines) {
        auto[lhs, rhs] = VecToTuple<2>(GetLines(line, "-"));
        graph[lhs].push_back(rhs);
        graph[rhs].push_back(lhs);
    }

    PrintShit(Traverse(graph, "start"));
}

}