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

void FirstSolution(std::string_view data) {
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
        / *std::size_t addNextGen = 0;
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

void Refactor(std::string_view data) {
    const std::size_t defaultDelay = 6;
    const std::size_t maxDelay = 8;
    const std::size_t daysP1 = 80, daysP2 = 256;

    using boost::multiprecision::mpz_int;
    std::vector<mpz_int> fishCountPerTimer(maxDelay + 1 /* 0 is a valid timer */, mpz_int(0));
    for (auto initial = Utils::GetNumbersFromLines<std::size_t>(data, ","); auto v: initial) fishCountPerTimer[v] += 1;

    auto PrintResult = [&fishCountPerTimer](const std::string &str) {
        fmt::print("{}: {}\n",
                   str,
                   std::accumulate(fishCountPerTimer.cbegin(), fishCountPerTimer.cend(), mpz_int(0)).str());
    };

    for (std::size_t day = 0; day < daysP2; day++) {
        auto nowExpiring = fishCountPerTimer.front();
        std::copy(fishCountPerTimer.begin() + 1, fishCountPerTimer.end(), fishCountPerTimer.begin());
        fishCountPerTimer.back() = nowExpiring;
        fishCountPerTimer[defaultDelay] += nowExpiring;

        if (day == daysP1) PrintResult("part 1");
    }
    PrintResult("part 2");
}

template<typename T, std::size_t n>
using matrix_t = std::array<std::array<T, n>, n>;

template<typename T, std::size_t n>
matrix_t<T, n> MulMatPlain(const matrix_t<T, n> &lhs, const matrix_t<T, n> &rhs) {
    matrix_t<T, n> result{{{0}}};

    for (std::size_t i = 0; i < n; ++i)
        for (std::size_t j = 0; j < n; ++j)
            for (std::size_t k = 0; k < n; ++k)
                result[i][j] += lhs[i][k] * rhs[k][j];

    return result;
}

template<typename T, std::size_t n>
void ExpMat(matrix_t<T, n> &mat, std::size_t exp) {
    matrix_t<T, n> resid{{{0}}};
    for (std::size_t i = 0; i < n; i++) resid[i][i] = 1;

    while (exp - 1) {
        if (exp % 2) resid = MulMatPlain(resid, mat);
        mat = MulMatPlain(mat, mat);
        exp /= 2;
    }

    mat = MulMatPlain(mat, resid);
}

void Matrix(std::string_view data) {
    //using int_t = int;
    using int_t = boost::multiprecision::mpz_int;
    matrix_t<int_t, 9> matrix{{{0}}};
    for (std::size_t i = 0; i < 8; i++) matrix[i][i + 1] = 1;
    matrix[6][0] = 1;
    matrix[8][0] = 1;

    ExpMat(matrix, 9999999);

    std::array<int_t, 9> state{};
    for (auto initial = Utils::GetNumbersFromLines<std::size_t>(data, ","); auto v: initial) state[v] += 1;

    int_t sum = 0;

    for (std::size_t i = 0; i < 9; i++)
        for (std::size_t j = 0; j < 9; j++)
            sum += matrix[i][j] * state[j];

    Utils::PrintShit(sum.str());
}

void Sol::Solve(std::string_view data) {
    Matrix(data);
}

}