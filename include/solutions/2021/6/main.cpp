#include "main.hpp"

namespace AOC::Y2021::D6 {

std::size_t Tick(std::vector<std::size_t> &fish) {
    std::size_t addNextGen = 0;
    for (std::size_t j = 0; j < fish.size(); j++) {
        if (fish[j]) --fish[j];
        else {
            fish[j] = 6;
            ++addNextGen;
        }
    }
    return addNextGen;
}

void Sol::Solve(std::string_view data) {
    using namespace Utils;
    auto fish = GetNumbersFromLines<std::size_t>(data, ",");

    //easy copy & paste shop
    //VecToTuple
    //ToPair
    //Convert

    using boost::multiprecision::cpp_int;
    cpp_int count = 0;

    /*std::vector<std::size_t> timeToZeroInit{};
    std::vector<std::size_t> triggerCount(fish.size(), 0);
    for (auto &f: fish) timeToZeroInit.push_back(6 - f);*/

    //std::vector<std::size_t> eight;

    /*for (auto i = 0; i < fish.size(); i++) {
        Tick(eight);
        for (std::size_t j = 0; j < Tick(fish); j++) {
            eight.push_back(8);
        }
        for (auto v : fish) fmt::print("{},", v);
        fmt::print("\n");
    }

    for (auto v : eight) fmt::print("{},", v);
    fmt::print("\n");*/

    /*for (auto i = 0; i < 80; i++) {
        auto addNextGen = Tick(fish);
        /*std::size_t addNextGen = 0;
        for (std::size_t j = 0; j < fish.size(); j++) {
            //fmt::print("{},", fish[j]);
            if (fish[j]) --fish[j];
            else {
                fish[j] = 6;
                //fish.push_back(8);
                ++addNextGen;
            }
        }* /
        //fmt::print("\n");
        for (auto j = addNextGen; j; j--) fish.push_back(8);
    }

    //PrintShit(std::accumulate(fish.cbegin(),  fish.cend(), 0));
    //PrintShit(fish.size());*/

    std::vector<cpp_int> state(9, 0);
    for (auto v: fish) ++state[v];

    for (auto i = 0; i < 256; i++) {
        auto v = state.front();
        std::copy(state.begin() + 1, state.end(), state.begin());
        state.back() = v;
        state[6] += v;
    }

    PrintShit(std::accumulate(state.cbegin(), state.cend(), cpp_int(0)).str());

}

}