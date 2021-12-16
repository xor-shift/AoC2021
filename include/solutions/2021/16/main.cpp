#include "main.hpp"

namespace AOC::Y2021::D16 {

u64 read_number(auto it, auto end) {
    u64 v = 0;
    for (; it != end; it++) {
        v <<= 1;
        v |= *it & 1;
    }
    return v;
}

static u64 s_version_sum = 0;

u64 parse_packet(std::span<u8> bits) {
    u8 p_ver = read_number(bits.begin(), bits.begin() + 3);
    u8 tid = read_number(bits.begin() + 3, bits.begin() + 6);
    auto current_bit = bits.begin() + 6;

    s_version_sum += p_ver;

    if (tid == 4) {
        //std::span<u8> data_span{bits.begin() + 6, bits.end()};
        std::vector<bool> read_data;

        for (bool cont = true;cont;) {
            cont = *current_bit++;
            for (u64 j = 0; j < 4; j++)
                read_data.push_back(*current_bit++);
        }

        for (auto b: read_data) fmt::print("{}", b ? '1' : '0');
        fmt::print("\n");
    } else {
        bool lt_id = *current_bit++;
        u64 rest_len = read_number(current_bit, current_bit + (lt_id ? 11 : 15));
        current_bit += lt_id ? 11 : 15;

        u64 consumed_on_subs = 0;
        u64 n_consumed_subs = 0;
        for (;;) {
            auto asd = parse_packet({current_bit, bits.end()});
            current_bit += asd;
            consumed_on_subs += asd;
            ++n_consumed_subs;
            if (lt_id) {
                if (n_consumed_subs >= rest_len) break;
            } else {
                if (consumed_on_subs >= rest_len) break;
            }
        }
    }

    return std::distance(bits.begin(), current_bit);
}

void Sol::Solve(std::string_view data) {
    using namespace Utils::UseThisNamespace;

    auto lines = GetLines(data, "\n");

    umap<char, std::string> conv{
      {'0', "0000"},
      {'1', "0001"},
      {'2', "0010"},
      {'3', "0011"},
      {'4', "0100"},
      {'5', "0101"},
      {'6', "0110"},
      {'7', "0111"},
      {'8', "1000"},
      {'9', "1001"},
      {'A', "1010"},
      {'B', "1011"},
      {'C', "1100"},
      {'D', "1101"},
      {'E', "1110"},
      {'F', "1111"},
    };

    std::vector<u8> bits;

    for (char c: lines[0]) {
        auto corresp = conv[c];

        for (char b: corresp) {
            bits.push_back(b == '1');
        }
    }

    parse_packet(bits);

    PrintShit(s_version_sum);
}

}
