#include "main.hpp"

namespace AOC::Y2021::D21 {
using namespace Utils::UseThisNamespace;

static const std::array<u64, 2> s_test_start_pos{4, 8};
static const std::array<u64, 2> s_actual_start_pos{1, 5};

static struct Die {
    u64 m_last = 0;
    u64 m_rolls = 0;

    u64 roll() {
        ++m_rolls;
        u64 ret = ++m_last;
        if (ret == 100) m_last = 0;
        return ret;
    }
} s_die;

bool part_1_win_check(std::array<u64, 2> score) {
    if (std::max(score[0], score[1]) >= 1000) {
        const auto losing = std::min(score[0], score[1]);
        fmt::print("part 1: {}, {}, {}\n", s_die.m_rolls, losing, s_die.m_rolls * losing);
        return true;
    }
    return false;
}

u64 add_pos(const u64 &pos, const u64 &val) {
    return (pos + val - 1) % 10 + 1;
}

void part_1(std::array<u64, 2> pos) {
    std::array<u64, 2> score{0, 0};
    for (u64 i = 1;; i++) {
        u64 p_1_roll = s_die.roll() + s_die.roll() + s_die.roll();
        pos[0] = add_pos(pos[0], p_1_roll);
        score[0] += pos[0];
        if (part_1_win_check(score)) break;
        u64 p_2_roll = s_die.roll() + s_die.roll() + s_die.roll();
        pos[1] = add_pos(pos[1], p_2_roll);
        score[1] += pos[1];
        if (part_1_win_check(score)) break;
    }
}

struct State {
    std::array<u64, 2> m_pos;
    std::array<u64, 2> m_score;
    bool m_player;

    friend bool operator==(const State &lhs, const State &rhs) {
        return lhs.m_pos == rhs.m_pos && lhs.m_score == rhs.m_score && lhs.m_player == rhs.m_player;
    }

    struct Hasher {
        std::size_t operator()(State state) const {
            return std::hash<std::string>{}(
              fmt::format("{}{}{}{}{}", state.m_pos[0], state.m_pos[1], state.m_score[0],
                          state.m_score[1], state.m_player));
        }
    };
};

static std::unordered_map<State, std::array<u64, 2>, State::Hasher> s_wins_memo{};

std::array<u64, 2> part_2(std::array<u64, 2> pos, std::array<u64, 2> score = {0, 0}, bool player = false) {
    std::array<u64, 2> ret{0, 0};

    if (auto it = s_wins_memo.find({pos, score, player}); it != s_wins_memo.end())
        return it->second;

    for (u64 i = 1; i <= 3; i++) {
        for (u64 j = 1; j <= 3; j++) {
            for (u64 k = 1; k <= 3; k++) {
                const auto new_pos = add_pos(pos[player], i + j + k);

                auto temp_pos = pos;
                auto temp_score = score;

                temp_pos[player] = new_pos;
                temp_score[player] += new_pos;

                if (temp_score[player] >= 21) {
                    ret[player]++;
                } else {
                    auto wins = part_2(temp_pos, temp_score, !player);

                    ret[0] += wins[0];
                    ret[1] += wins[1];
                }
            }
        }
    }

    s_wins_memo[{pos, score, player}] = ret;
    return ret;
}

void Sol::Solve(std::string_view data) {
    auto lines = GetLines(data, "\n");
    part_1(s_test_start_pos);
    auto[s_0, s_1] = part_2(s_actual_start_pos);
    fmt::print("part 2: {}, {}, {}\n", s_0, s_1, std::max(s_0, s_1));
}

}
