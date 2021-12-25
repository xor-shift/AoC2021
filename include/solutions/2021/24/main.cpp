#include "main.hpp"

namespace AOC::Y2021::D24 {

namespace FirstSolution {
using namespace Utils::UseThisNamespace;

struct Instruction {
    std::string m_opcode;
    char m_arg0;
    std::optional<std::variant<char, i64>> m_arg1 = std::nullopt;
};

using Block = std::span<Instruction, 18>;

std::array<i64, 3> get_block_constants(const Block &block) {
    std::array<i64, 3> ret{0, 0, 0};

    ret[0] = std::get<i64>(*block[5].m_arg1);
    ret[1] = std::get<i64>(*block[4].m_arg1);
    ret[2] = std::get<i64>(*block[15].m_arg1);

    return ret;
};

void print_instruction(const Instruction &i) {
    fmt::print("{} {}", i.m_opcode, i.m_arg0);
    if (i.m_arg1) {
        if (std::holds_alternative<char>(*i.m_arg1)) fmt::print(" {}", std::get<char>(*i.m_arg1));
        if (std::holds_alternative<i64>(*i.m_arg1)) fmt::print(" {}", std::get<i64>(*i.m_arg1));
    }
    std::putchar('\n');
}

struct VM {
    std::vector<Instruction> m_program;
    std::vector<u64> m_input{};
    std::size_t m_inp_ptr = 0;
    std::size_t m_pc = 0;
    std::array<i64, 4> m_registers{0, 0, 0, 0};
    bool m_running = true;

    void tick();
};

void VM::tick() {
    if (!m_running) return;

    const auto &instr = m_program[m_pc++];
    //print_instruction(instr);

    auto &target = m_registers[instr.m_arg0 - 'w'];

    i64 rhs;
    if (instr.m_arg1) {
        if (std::holds_alternative<char>(*instr.m_arg1)) rhs = m_registers[std::get<char>(*instr.m_arg1) - 'w'];
        if (std::holds_alternative<i64>(*instr.m_arg1)) rhs = std::get<i64>(*instr.m_arg1);
    }

    const auto opcode = instr.m_opcode;
    if (opcode == "inp") {
        if (m_inp_ptr >= m_input.size()) std::abort();
        target = m_input[m_inp_ptr++];
    } else if (opcode == "add") {
        target = target + rhs;
    } else if (opcode == "mul") {
        target = target * rhs;
    } else if (opcode == "div") {
        target = target / rhs;
    } else if (opcode == "mod") {
        target = target % rhs;
    } else if (opcode == "eql") {
        target = target == rhs ? 1 : 0;
    } else {
        std::abort();
    }

    if (m_pc == m_program.size()) m_running = false;
}

std::vector<u64> digitize(u64 v) {
    std::vector<u64> ret;
    ret.reserve(static_cast<u64>(std::log10(v)));
    for (std::size_t i = 0;; i++) {
        ret.push_back(v % 10);
        v /= 10;
        if (v == 0) break;
    }
    return ret;
}

std::vector<Instruction> parse_program(std::string_view data) {
    std::vector<Instruction> program{};
    ProcessLines(data, "\n", [&](std::string_view instruction_str, std::size_t) {
        std::string_view asd = instruction_str;
        Instruction current{
          .m_opcode = std::string{asd.begin(), asd.begin() + 3},
          .m_arg0 = instruction_str[4],
        };

        if (instruction_str.size() > 5) {
            auto m_arg1_str = asd.substr(6);
            if (std::isalpha(m_arg1_str.front())) current.m_arg1 = m_arg1_str.front();
            else current.m_arg1 = Convert<i64>(m_arg1_str);
        }

        program.push_back(std::move(current));
    });
    return program;
}

void scrapped_attempt(std::string_view data) {
    auto program = parse_program(data);
    /*for (auto i : program) {
        fmt::print("{} {}", i.m_opcode, i.m_arg0);
        if (i.m_arg1) {
            if (std::holds_alternative<char>(*i.m_arg1)) fmt::print(" {}", std::get<char>(*i.m_arg1));
            if (std::holds_alternative<i64>(*i.m_arg1)) fmt::print(" {}", std::get<i64>(*i.m_arg1));
        }
        std::putchar('\n');
    }*/

    VM vm{
      .m_program = program,
    };

    for (u64 i = 99'999'999'999'999; i >= 10'000'000'000'000; i--) {
        if (!(i % 1'000'000)) fmt::print("@ {}\n", i);
        auto digits = digitize(i);
        vm.m_input = digits;
        while (vm.m_running) vm.tick();
        if (const auto result = vm.m_registers[3]; result == 0)
            fmt::print("part 1: {}\n", i);
    }
}

void scrapped_attempt_2(std::string_view data) {
    //observations:
    //each digit is dependent on the previous digits' computations
    //each "block" (parts that processes digits) starts with:
    /*
inp w
mul x 0
add x z
mod x 26
     */
    //each block ends with 'add z y'
    //w is only read from
    //each block is exactly 18 instructions long
    //there are 14 blocks

    //okay actually there are more common points like first arguments, time to compute the similarities:
    auto program = parse_program(data);
    std::array<Instruction, 18> common_stuff{};
    std::copy(program.begin(), program.begin() + 18, common_stuff.begin());

    for (std::size_t i = 0; i < 14; i++) {
        std::span<Instruction, 18> block{program.begin() + i * 18, program.begin() + (i + 1) * 18};

        for (std::size_t j = 0; j < 18; j++) {
            if (common_stuff[j].m_opcode != block[j].m_opcode) common_stuff[j].m_opcode = "???";
            if (common_stuff[j].m_arg0 != block[j].m_arg0) common_stuff[j].m_arg0 = '?';
            if (!common_stuff[j].m_arg1 != !block[j].m_arg1) common_stuff[j].m_arg1 = std::nullopt;
            else if (block[j].m_arg1) {
                if (std::holds_alternative<char>(*common_stuff[j].m_arg1) !=
                    std::holds_alternative<char>(*block[j].m_arg1))
                    common_stuff[j].m_arg1 = std::nullopt;
                else {
                    if (std::holds_alternative<char>(*common_stuff[j].m_arg1)) {
                        auto lhs = std::get<char>(*common_stuff[j].m_arg1);
                        auto rhs = std::get<char>(*block[j].m_arg1);
                        if (lhs != rhs) common_stuff[j].m_arg1 = std::nullopt;
                    } else {
                        auto lhs = std::get<i64>(*common_stuff[j].m_arg1);
                        auto rhs = std::get<i64>(*block[j].m_arg1);
                        if (lhs != rhs) common_stuff[j].m_arg1 = std::nullopt;
                    }
                }
            }
        }
    }

    for (auto i: common_stuff) print_instruction(i);

    /*
     higher level overview for blocks:
     z_0 = 0

     a_i = L6 (random)
     b_i = L5 (1 or 26)
     c_i = L16 (random)

     w_i = inp_i
     x = ((z % 26) + a_i) != w_i
     z_i = z_{i-1}/b_i
     y = 25x + 1
     z_i *= y
     y = (w_i + c_i) * x
     z_i += y

     x = ((z_{i-1} % 26) + a_i) != inp_i
     z_i = ((25x + 1) * (z_{i-1} / b_i)) + (inp_i + c_i) * x

     case of x = 0:
     z_i = ((25 * 0 + 1) * (z_{i-1} / b_i)) + (inp_i + c_i) * 0
     z_i = z_{i-1} / b_i
     z_{i-1} = z_i * b_i
     when
     ((z_{i-1} % 26) + a_i) == inp_i
     ((z_i * b_i % 26) + a_i) == inp_i --> condition for the previous inverse

     case of x = 1:
     ((z_i - k_i - c_i)b_i + 1)/26 = z_{i-1}
     when
     X = ((z_i - k_i - c_i)b_i + 1)/26
     ((X % 26) + a_i) != inp_i
     */

    using Block = std::span<Instruction, 18>;

    auto get_constants = [](const Block &block) {
        std::array<i64, 3> ret{0, 0, 0};

        ret[0] = std::get<i64>(*block[5].m_arg1);
        ret[1] = std::get<i64>(*block[4].m_arg1);
        ret[2] = std::get<i64>(*block[15].m_arg1);

        return ret;
    };

    i64 z_i = 0;
    std::array<i64, 14> k_set{0};
    for (std::size_t i = 14; i > 0; i--) {
        std::size_t block_index = i - 1;
        Block block{program.begin() + block_index * 18, program.begin() + (block_index + 1) * 18};

        auto[a_i, b_i, c_i] = get_constants(block);
        bool case_x_zero = false;

        //(b_i * z_i) % 26 + a_i ==
    }

    //okay k_i is an unknown
}

inline static std::array<std::array<i64, 3>, 14> s_block_constants{};

struct State {
    i64 m_i = 0;
    i64 m_k_i = 0; //the important part
    i64 m_z_i_1 = 0;

    friend bool operator==(const State &lhs, const State &rhs) {
        return lhs.m_k_i == rhs.m_k_i && lhs.m_z_i_1 == rhs.m_z_i_1 && lhs.m_i == rhs.m_i;
    }

    i64 get_z_i() const;

    std::optional<i64> get_best_solution() const;
};

struct StateHasher {
    std::size_t operator()(const State &state) const {
        return std::hash<std::string>{}(fmt::format("{}{}{}",
                                                    state.m_i,
                                                    state.m_k_i,
                                                    state.m_z_i_1));
    }
};

i64 State::get_z_i() const {
    //static std::unordered_map<State, i64, StateHasher> memoization;
    //if (auto it = memoization.find(*this); it != memoization.end()) return it->second;
    const auto x = ((m_z_i_1 % 26) + s_block_constants[m_i][0]) != m_k_i;
    const auto res = ((25 * x + 1) * (m_z_i_1 / s_block_constants[m_i][1])) + x * (m_k_i + s_block_constants[m_i][2]);

    //memoization[*this] = res;
    return res;
}

std::optional<i64> State::get_best_solution() const {
    if (m_i < 3) {
        for (i64 i = 0; i < m_i; i++) std::putchar(' ');
        std::putchar('0' + m_k_i);
        std::putchar('\n');
    }

    static std::unordered_map<State, std::optional<i64>, StateHasher> memoization;
    if (auto it = memoization.find(*this); it != memoization.end()) return it->second;

    std::optional<i64> res = std::nullopt;

    if (m_i == 13) { //end of the tree
        const auto z_i = get_z_i();
        if (z_i != 0) res = std::nullopt;
        else res = m_k_i;
    } else {
        //i64 best_result = -1;
        i64 best_result = std::numeric_limits<i64>::max();

        for (i64 k = 1; k <= 9; k++) {
            State sub_state{
              .m_i = m_i + 1,
              .m_k_i = k,
              .m_z_i_1 = get_z_i(),
            };

            const auto sub_result = sub_state.get_best_solution();
            if (!sub_result) continue;

            best_result = std::min(best_result, *sub_result);
        }

        if (best_result != std::numeric_limits<i64>::max()) res = m_k_i * std::pow(10, 13 - m_i) + best_result;
    }

    memoization[*this] = res;
    return res;
}

void solve(std::string_view data) {
    auto program = parse_program(data);
    std::array<std::array<i64, 3>, 14> block_constants{};

    for (std::size_t i = 0; i < 14; i++) {
        Block block{program.begin() + i * 18, program.begin() + (i + 1) * 18};

        block_constants[i] = get_block_constants(block);
    }

    s_block_constants = block_constants;

    //sanity check
    State sanity_check_state;
    i64 prev_z = 0;
    for (std::size_t i = 0; i < 14; i++) {
        sanity_check_state.m_i = i;
        sanity_check_state.m_k_i = 9;
        sanity_check_state.m_z_i_1 = prev_z;

        prev_z = sanity_check_state.get_z_i();
    }
    fmt::print("state method: {}\n", prev_z);

    VM vm{
      .m_program = program,
      .m_input = {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
    };
    while (vm.m_running) vm.tick();
    fmt::print("vm method: {}\n", vm.m_registers[3]);

    for (i64 k = 1; k <= 9; k++) {
        State state{
          .m_i = 0,
          .m_k_i = k,
          .m_z_i_1 = 0,
        };

        auto res = state.get_best_solution();
        if (res) {
            fmt::print("{}", *res);
            break;
        }
    }
    //9998426997919
    //99998426997919
    //19998426997975
}

}

static constexpr std::size_t k_block_size = 18;
using SingleBlockConstants = std::array<int64_t, 3>;
using BlockConstants = std::array<SingleBlockConstants, k_block_size>;

inline static BlockConstants s_block_constants{};

int64_t get_z_i(int64_t i, int64_t k_i, int64_t z_i_1) {
    const auto x = ((z_i_1 % 26) + s_block_constants[i][0]) != k_i;
    const auto res = ((25 * x + 1) * (z_i_1 / s_block_constants[i][1])) + x * (k_i + s_block_constants[i][2]);

    return res;
}

template<bool part_1>
struct Memoizer
  : public Utils::MemoizationWrapper<std::optional<int64_t>, int64_t, int64_t, int64_t> {
    Memoizer() noexcept = default;

    Memoizer(std::size_t lru_size)
      : Utils::MemoizationWrapper<std::optional<int64_t>, int64_t, int64_t, int64_t>(lru_size) {}

  protected:
    std::optional<int64_t> impl(int64_t i, int64_t k_i, int64_t z_i_1) final override {
        /*if (i < 2) {
            for (i64 j = 0; j < i; j++) std::putchar(' ');
            std::putchar('0' + k_i);
            std::putchar('\n');
        }*/

        std::optional<int64_t> res{std::nullopt};
        const int64_t initial_result_choice = part_1 ? -1 : std::numeric_limits<int64_t>::max();

        const auto z_i = get_z_i(i, k_i, z_i_1);

        if (i == 13) { //end of the tree
            if (z_i != 0) res = std::nullopt;
            else res = k_i;
        } else {
            int64_t best_result = initial_result_choice;

            for (int64_t k = part_1 ? 9 : 1; part_1 ? (k >= 1) : (k <= 9); k = part_1 ? k - 1 : k + 1) {

                const auto sub_result = (*this)(i + 1, k, z_i);
                if (!sub_result) continue;

                if constexpr (part_1)
                    best_result = std::max(best_result, *sub_result);
                else
                    best_result = std::min(best_result, *sub_result);
            }

            if (best_result != initial_result_choice)
                res =
                  k_i * static_cast<int64_t>(std::pow(10, 13 - i)) +
                  best_result;
        }
        return res;
    }
};

template<bool part_1>
void solve() {
    Memoizer<part_1> memoizer{4ull * 1024 * 1024};

    for (int64_t k = part_1 ? 9 : 1; part_1 ? (k >= 1) : (k <= 9); k = part_1 ? k - 1 : k + 1) {
        auto res = memoizer(0, k, 0);

        if (res) {
            fmt::print("{}\n", *res);
            break;
        }
    }

    fmt::print("avoided computing {}% of the calls\n", memoizer.memoized_ratio() * 100);
}

void Sol::Solve(std::string_view data) {
    Utils::ProcessLines(data, "\n", [](std::string_view line, std::size_t line_no) {
        const auto block_no = line_no / k_block_size;
        const auto in_block_line_no = line_no % k_block_size;

        const auto segments = Utils::GetLines(line, " ");

        if (in_block_line_no == 5) s_block_constants[block_no][0] = Utils::Convert<int64_t>(segments.back());
        if (in_block_line_no == 4) s_block_constants[block_no][1] = Utils::Convert<int64_t>(segments.back());
        if (in_block_line_no == 15) s_block_constants[block_no][2] = Utils::Convert<int64_t>(segments.back());
    });

    solve<true>();
    solve<false>();
}

}
