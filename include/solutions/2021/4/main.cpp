#include "main.hpp"

namespace AOC::Y2021::D4 {

void Sol::Solve(std::string_view data) {
    auto lines = Utils::GetLines(data, "\n");

    std::vector<std::size_t> fr{};
    for (auto nums = Utils::GetLines(lines[0], ","); auto i: nums) fr.push_back(Utils::Convert<std::size_t>(i, 0, 10));

    auto bingoSpan = std::span<std::string_view>(lines.begin() + 2, lines.end());

    typedef std::array<std::array<std::pair<std::size_t, bool>, 5>, 5> bingo_t;
    std::vector<bingo_t> bingos;
    std::vector<std::size_t> winningScores;
    std::vector<std::size_t> winningOrder;

    for (std::size_t i = 0; true; i++) {
        std::size_t idx = i * 6;
        if (idx >= bingoSpan.size()) break;
        auto bingo = bingoSpan.subspan(idx, 5);
        bingo_t board{};
        for (std::size_t j = 0; j < 5; j++) {
            auto asd = Utils::GetNumbersFromLines<std::size_t, 1>(bingo[j], " ");
            for (std::size_t k = 0; k < asd.size(); k++) {
                board[j][k] = {asd[k], false};
            }
        }
        bingos.push_back(board);
        winningScores.push_back(0);
    }
    winningOrder.reserve(bingos.size());

    auto RowCheck = [](const bingo_t &board) {
        for (std::size_t row = 0; row < 5; row++) {
            if (std::all_of(board[row].cbegin(), board[row].cend(), [](auto &line) { return line.second; })) {
                return true;
            }
        }
        return false;
    };

    auto ColCheck = [](const bingo_t &board) {
        for (std::size_t col = 0; col < 5; col++) {
            bool ok = true;
            for (std::size_t row = 0; row < 5; row++) {
                if (!board[row][col].second) {
                    ok = false;
                    break;
                }
            }
            if (ok) return true;
        }
        return false;
    };

    auto DiagCheck = [](const bingo_t &board) {
        for (std::size_t row = 0; row < 5; row++) {
            if (!board[row][row].second) return false;
        }
        return true;
    };

    auto HasWon = [](const bingo_t &board) {
        for (auto row: board) {
            if (!std::all_of(row.cbegin(), row.cend(), [](auto a) { return a.second; })) return false;
        }
        return true;
    };

    auto WonCount = [&bingos, HasWon]() {
        return std::count_if(bingos.cbegin(), bingos.cend(), [HasWon](const auto &b) { return HasWon(b); });
    };


    for (auto drawn: fr) {
        bool gotBingo = false;
        for (std::size_t i = 0; auto &board: bingos) {
            for (auto &line: board) {
                for (auto &v: line) {
                    if (v.first == drawn)
                        v.second = true;
                }
            }

            if (RowCheck(board) || ColCheck(board) || DiagCheck(board)) {
                constexpr bool p1 = true;

                if (std::find(winningOrder.cbegin(), winningOrder.cend(), i) == winningOrder.cend()) {
                    winningOrder.push_back(i);
                    std::size_t sum = 0;
                    for (auto &l: board) {
                        for (auto k: l) {
                            sum += k.first * static_cast<std::size_t>(!k.second);
                        }
                    }
                    winningScores[i] = sum * drawn;
                    gotBingo = true;
                }

                if (gotBingo) {
                    fmt::print("bingo for {}, ", i);
                    fmt::print("{}\n", winningScores[i]);
                }

                if constexpr (p1) {


                } else {
                    if (static_cast<std::size_t>(WonCount()) == bingos.size() - 1) {
                        fmt::print("bingo for {}, ", i);
                        gotBingo = true;

                        std::size_t sum = 0;
                        for (auto &l: board) {
                            for (auto k: l) {
                                sum += k.first * static_cast<std::size_t>(!k.second);
                            }
                        }
                        fmt::print("{}\n", sum * drawn);
                        break;
                    }
                }
            }


            i++;
        }
        //if (gotBingo) break;
    }

    fmt::print("{}\n", winningScores[winningOrder[winningOrder.size() - 1]]);
}

}