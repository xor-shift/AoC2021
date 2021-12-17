#include "main.hpp"

namespace AOC::Y2021::D17 {
using namespace Utils::UseThisNamespace;

using Coordinate = std::array<i64, 2>;

struct Object {
    Coordinate m_pos;
    Coordinate m_vel;
    i64 m_max_y = 0;

    void tick() {
        m_pos[0] += m_vel[0];
        m_pos[1] += m_vel[1];
        m_vel[0] -= m_vel[0] > 0 ? 1 : m_vel[0] < 0 ? -1 : 0;
        --m_vel[1];
        m_max_y = std::max(m_pos[1], m_max_y);
    }
};

std::pair<Coordinate, Coordinate> s_actual_input{{56,   76},
                                                 {-162, -134}};

std::pair<bool, bool> obj_in_range(Object obj, std::pair<Coordinate, Coordinate> ranges) {
    bool in_x = ranges.first[0] <= obj.m_pos[0] && obj.m_pos[0] <= ranges.first[1];
    bool in_y = ranges.second[0] <= obj.m_pos[1] && obj.m_pos[1] <= ranges.second[1];
    bool x_bad = obj.m_vel[0] == 0 && !in_x;
    bool y_bad = obj.m_vel[1] < 0 && obj.m_pos[1] < ranges.second[0];

    return {in_x && in_y, x_bad || y_bad};
}

void Sol::Solve(std::string_view data) {
    auto lines = GetLines(data, "\n");

    const auto target_range = s_actual_input;

    i64 highest_y = 0;

    umap<i64, umap<i64, bool>> valids{};

    for (i64 y_sweep = -300; y_sweep <= 300; ++y_sweep) {
        for (i64 x_sweep = -300; x_sweep <= 300; ++x_sweep) {
            if (y_sweep == 9 && x_sweep == 6) {
                PrintShit("");
            }

            //PrintShit("trying", x_sweep, y_sweep);
            Coordinate init_vel{x_sweep, y_sweep};

            Object obj{
              .m_pos = {0, 0},
              .m_vel = init_vel,
            };

            for (;;) {
                obj.tick();

                auto[in_range, overshot_status] = obj_in_range(obj, target_range);

                if (in_range) {
                    valids[init_vel[1]][init_vel[0]] = true;
                    highest_y = std::max(obj.m_max_y, highest_y);
                    break;
                }

                if (overshot_status) {
                    break;
                }
            }
        }
    }
    PrintShit("part 1", highest_y); //50005000, 5000050000, 125250, 124750, 45150

    u64 sum = 0;
    for (auto &r: valids)
        for (auto b: r.second) {
            sum += b.second;
            PrintShit(b.first, r.first);
        }
    PrintShit("part 2", sum);
}

}
