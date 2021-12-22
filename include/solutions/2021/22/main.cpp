#include "main.hpp"

namespace AOC::Y2021::D22 {
using namespace Utils::UseThisNamespace;

void part_1(std::string_view data) {
    auto lines = GetLines(data, "\n");

    umap<i64, umap<i64, umap<i64, bool>>> space;
    for (auto line: lines) {
        auto[instruction, area] = VecToTuple<2>(GetLines(line, " "));
        //fmt::print("{}, {}\n", instruction, area);
        auto[x_instr, y_instr, z_instr] = Map(VecToTuple<3>(GetLines(area, ",")), [](std::string_view segment) {
            segment = segment.substr(2);
            auto dot_index = segment.find_first_of('.');
            auto lhs = segment.substr(0, dot_index);
            auto rhs = segment.substr(dot_index + 2);
            return std::make_pair(Convert<i64>(lhs), Convert<i64>(rhs));
        });

        for (i64 z = z_instr.first; z <= z_instr.second; z++) {
            for (i64 y = y_instr.first; y <= y_instr.second; y++) {
                for (i64 x = x_instr.first; x <= x_instr.second; x++) {
                    if (x > 50 || x < -50) continue;
                    if (y > 50 || y < -50) continue;
                    if (z > 50 || z < -50) continue;
                    space[z][y][x] = instruction == "on";
                }
            }
        }
    }

    u64 sum = 0;
    for (const auto &[z, plane]: space)
        for (const auto &[y, line]: plane)
            for (const auto &[x, v]: line) {
                sum += v;
            }
    PrintShit(sum);
}

using Span = std::array<i64, 2>;
using VolumeSpan = std::array<Span, 3>;

bool has_overlap(const VolumeSpan &lhs, const VolumeSpan &rhs) {
    bool no_x_overlap = lhs[0][1] < rhs[0][0] || rhs[0][1] < lhs[0][0];
    bool no_y_overlap = lhs[1][1] < rhs[1][0] || rhs[1][1] < lhs[1][0];
    bool no_z_overlap = lhs[2][1] < rhs[2][0] || rhs[2][1] < lhs[2][0];

    return !(no_x_overlap || no_y_overlap || no_z_overlap);
}

std::vector<VolumeSpan> repartition_overlap_old(const VolumeSpan &partition_span, const VolumeSpan &new_span) {
    if (!has_overlap(partition_span, new_span)) return {partition_span};

    std::vector<VolumeSpan> partitions;

    VolumeSpan left_span = partition_span;

    for (std::size_t dim = 0; dim < 3; dim++) {
        const auto new_dim_min = std::min(new_span[dim][0], new_span[dim][1]);
        const auto new_dim_max = std::max(new_span[dim][0], new_span[dim][1]);
        const auto left_dim_min = std::min(left_span[dim][0], left_span[dim][1]);
        const auto left_dim_max = std::max(left_span[dim][0], left_span[dim][1]);

        bool in_middle = (left_span[dim][1] > new_dim_max) &&
                         (left_span[dim][0] < new_dim_min);
        bool asd = (new_span[dim][1] >= left_dim_max) &&
                   (new_span[dim][0] <= left_dim_min);

        if (asd) continue;

        if (in_middle) { //left_span will be the middle section
            VolumeSpan lhs_span{left_span};
            VolumeSpan rhs_span{left_span};

            lhs_span[dim][1] = lhs_span[dim][0] + new_dim_min - 1;
            rhs_span[dim][0] = rhs_span[dim][1] - new_dim_max + 1;
            left_span[dim][0] = new_dim_min;
            left_span[dim][1] = new_dim_max;

            partitions.push_back(lhs_span);
            partitions.push_back(rhs_span);
        } else {
            bool orientation = left_span[dim][1] > new_span[dim][1];
            const auto a_1 = new_span[dim][0];
            const auto b_1 = new_span[dim][1];
            const auto a_0 = left_span[dim][0];
            const auto b_0 = left_span[dim][1];
            Span::value_type x, y;
            if (!orientation) {
                x = a_1 - a_0;
                y = b_0 - a_1 + 1;
            } else {
                x = b_0 - b_1;
                y = b_1 - a_0 + 1;
            }

            VolumeSpan free_span{left_span};

            if (!orientation) {
                free_span[dim][1] = free_span[dim][0] + x - 1;
                left_span[dim][0] = left_span[dim][1] - y + 1;
            } else {
                free_span[dim][0] = free_span[dim][1] - x + 1;
                left_span[dim][1] = left_span[dim][0] + y - 1;
            }

            partitions.push_back(free_span);
        }
    }

    return partitions;
}

bool is_inside(Span::value_type v, Span span) { return v >= span[0] && v <= span[1]; }

bool to_the_right(Span::value_type v, Span span) { return v > span[1]; }

bool to_the_left(Span::value_type v, Span span) { return v < span[0]; }

bool is_inside(Span v, Span span) { return is_inside(v[0], span) && is_inside(v[1], span); }

bool to_the_right(Span v, Span span) { return to_the_right(v[0], span); }

bool to_the_left(Span v, Span span) { return to_the_left(v[1], span); }

//a,b,c
std::tuple<std::optional<Span>, std::optional<Span>, std::optional<Span>>
repartition_span_overlap(Span to_partition, Span span) {
    auto zero = std::nullopt;
    //000
    //not possible

    //001
    //sp: ###
    //tp:    ####
    if (to_the_right(to_partition, span))
        return {zero, zero, to_partition};

    //010
    //sp: ######
    //tp:   ####
    if (is_inside(to_partition, span))
        return {zero, to_partition, zero};

    //011
    //sp: ########
    //tp:     ######
    if (is_inside(to_partition[0], span) && to_the_right(to_partition[1], span))
        return {zero, Span{to_partition[0], span[1]}, Span{span[1] + 1, to_partition[1]}};

    //100
    //sp:        ###
    //tp: #######
    if (to_the_left(to_partition, span))
        return {to_partition, zero, zero};

    //101
    //not possible

    //110
    //sp:    ####
    //tp: ####
    if (to_the_left(to_partition[0], span) && is_inside(to_partition[1], span))
        return {Span{to_partition[0], span[0] - 1}, Span{span[0], to_partition[1]}, zero};

    //111
    //sp:   ####
    //tp: ########
    if (is_inside(span, to_partition))
        return {Span{to_partition[0], span[0] - 1}, span, Span{span[1] + 1, to_partition[1]}};

    std::abort();
}

std::vector<VolumeSpan> repartition_overlap(const VolumeSpan &partition_span, const VolumeSpan &new_span) {
    if (!has_overlap(partition_span, new_span)) return {partition_span};

    std::vector<VolumeSpan> partitions{};
    VolumeSpan left_to_partition = partition_span;

    auto try_push = [&](auto v, auto dim) {
        if (v) {
            auto temp = left_to_partition;
            temp[dim] = *v;
            partitions.push_back(std::move(temp));
        }
    };

    for (std::size_t dim = 0; dim < 3; dim++) {
        auto[a_opt, b_opt, c_opt] = repartition_span_overlap(left_to_partition[dim], new_span[dim]);
        u8 state = 0;
        if (a_opt) state |= 4;
        if (b_opt) state |= 2;
        if (c_opt) state |= 1;

        if (!b_opt) {
            try_push(a_opt, dim);
            try_push(c_opt, dim);
            break;
        } else  {
            try_push(a_opt, dim);
            try_push(c_opt, dim);
            left_to_partition[dim] = *b_opt;
            continue;
        }
    }

    return partitions;
}

void print_span(const VolumeSpan &s) {
    fmt::print("{} {} {} -> {} {} {}\n",
               s[0][0],
               s[1][0],
               s[2][0],
               s[0][1],
               s[1][1],
               s[2][1]);
}

void print_span(const Span &s) {
    fmt::print("{} -> {}\n", s[0], s[1]);
}

void Sol::Solve(std::string_view data) {
    auto lines = GetLines(data, "\n");

    VolumeSpan test_span_0{{{1, 3}, {1, 3}, {1, 3}}};
    VolumeSpan test_span_1{{{2, 2}, {2, 2}, {2, 2}}};
    auto res = repartition_overlap(test_span_0, test_span_1);
    for (auto s: res) {
        /*fmt::print("{} {} {} -> {} {} {}\n",
                   s[0][0].str(),
                   s[1][0].str(),
                   s[2][0].str(),
                   s[0][1].str(),
                   s[1][1].str(),
                   s[2][1].str());*/

        print_span(s);
    }

    std::vector<VolumeSpan> repartitioned_spans;

    u64 l = 0;
    for (auto line: lines) {
        auto[instruction, area] = VecToTuple<2>(GetLines(line, " "));
        //fmt::print("{}, {}\n", instruction, area);
        auto[x_instr, y_instr, z_instr] = Map(VecToTuple<3>(GetLines(area, ",")), [](std::string_view segment) {
            segment = segment.substr(2);
            auto dot_index = segment.find_first_of('.');
            auto lhs = segment.substr(0, dot_index);
            auto rhs = segment.substr(dot_index + 2);
            return Span{Convert<i64>(lhs), Convert<i64>(rhs)};
        });

        VolumeSpan cur_span{x_instr, y_instr, z_instr};

        std::vector<VolumeSpan> new_repartitioned;
        if (!repartitioned_spans.empty()) {
            for (const auto &s: repartitioned_spans) {
                auto new_ones = repartition_overlap(s, cur_span);
                std::copy(new_ones.cbegin(), new_ones.cend(), std::back_inserter(new_repartitioned));
            }
        }

        if (instruction == "on")
            new_repartitioned.push_back(cur_span);

        repartitioned_spans = new_repartitioned;
        //PrintShit(l++);
    }

    mp_int sum = 0;
    for (auto s: repartitioned_spans) {
        mp_int volume = 1;
        for (std::size_t dim = 0; dim < 3; dim++)
            volume *= std::abs(s[dim][1] - s[dim][0]) + 1;
        //print_span(s);
        //PrintShit(volume.str());
        sum += volume;
    }
    PrintShit(sum.str()); //6051290975813587354, 39700569057640600
}

}
