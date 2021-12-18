#include "main.hpp"

namespace AOC::Y2021::D18 {
using namespace Utils::UseThisNamespace;

struct Element {
    Element() = default;

    Element(std::string_view view, Element *parent = nullptr, Element *root = nullptr, u64 depth = 0)
      : m_root(root)
        , m_parent(parent)
        , m_depth(depth) {
        if (m_parent == nullptr) m_root = this;

        if (view.front() != '[') {
            m_value = Convert<u64>(view, 10);
            return;
        }

        view = view.substr(1, view.size() - 2);

        u64 balance = 0;
        std::string_view::iterator comma = view.end();
        for (auto it = view.cbegin(); it != view.cend(); it++) {
            if (*it == '[') ++balance;
            else if (*it == ']') --balance;
            else if (*it == ',' && balance == 0) {
                comma = it;
                break;
            }
        }

        m_children = {std::make_unique<Element>(std::string_view{view.begin(), comma}, this, m_root, m_depth + 1),
                      std::make_unique<Element>(std::string_view{comma + 1, view.end()}, this, m_root, m_depth + 1)};
        m_children[0]->m_parent = this;
        m_children[1]->m_parent = this;
    }

    template<bool side = 0>
    bool is_sidemost() {
        Element *current = m_root;

        for (;;) {
            if (current == this) return true;
            if (current->is_leaf()) return false;
            current = current->m_children[side].get();
        }

        return false;
    }

    void print_recursive() {
        if (!m_children[0]) {
            fmt::print("{}", m_value);
            return;
        }

        fmt::print("[");
        m_children[0]->print_recursive();
        fmt::print(",");
        m_children[1]->print_recursive();
        fmt::print("]");

        if (m_depth == 0) fmt::print("\n");
    }

    bool is_leaf() {
        return m_children[0] == nullptr;
    }

    bool is_numeric_pair_leaf() {
        if (is_leaf()) return false;
        return m_children[0]->is_leaf() && m_children[1]->is_leaf();
    }

    bool explode_action() {
        if (m_depth >= 4 && is_numeric_pair_leaf()) {
            explode();
            return true;
        }

        if (!is_leaf()) {
            if (m_children[0]->explode_action()) return true;
            if (m_children[1]->explode_action()) return true;
        }
        return false;
    }

    bool action() {
        if (explode_action()) return true;

        if (!is_leaf()) {
            if (m_children[0]->action()) return true;
            if (m_children[1]->action()) return true;
            return false;
        }

        if (m_value >= 10) {
            split();
            return true;
        }

        return false;
    }

    void split() {
        auto lhs_value = m_value / 2;
        auto rhs_value = lhs_value + m_value % 2;
        m_value = 0;

        m_children = {std::make_unique<Element>(),
                      std::make_unique<Element>()};

        for (auto &c: m_children) {
            c->m_root = m_root;
            c->m_parent = this;
            c->m_depth = m_depth + 1;
        }

        m_children[0]->m_value = lhs_value;
        m_children[1]->m_value = rhs_value;
    }

    void explode() {
        auto lhs_val = m_children[0]->m_value;
        auto rhs_val = m_children[1]->m_value;

        m_children = {nullptr, nullptr};
        m_value = 0;

        std::vector<Element *> window = {};
        m_root->traverse_pre_order([this, &window, done = false](Element *node) mutable {
            /*if (done) return;

            window[0] = window[1];
            window[1] = window[2];
            window[2] = node;

            if (window[1] == this) done = true;*/
            window.push_back(node);
        });

        auto it = find(window, this);
        std::span<Element *> lhs_window{window.begin(), it};
        std::span<Element *> rhs_window{it + 1, window.end()};

        auto lhs_cand_it = std::find_if(lhs_window.rbegin(), lhs_window.rend(), [](Element *e) {
            return e->is_leaf();
        });

        auto rhs_cand_it = find_if(rhs_window, [](Element *e) {
            return e->is_leaf();
        });

        //PrintShit(window[0]->m_value, window[2]->m_value);

        if (lhs_cand_it != lhs_window.rend()) {
            auto *lhs_candidate = *lhs_cand_it;
            while (!lhs_candidate->is_leaf()) {
                lhs_candidate = lhs_candidate->m_children[1].get();
            }
            lhs_candidate->m_value += lhs_val;
        }

        if (rhs_cand_it != rhs_window.end()) {
            auto *rhs_candidate = *rhs_cand_it;
            while (!rhs_candidate->is_leaf()) {
                rhs_candidate = rhs_candidate->m_children[1].get();
            }
            rhs_candidate->m_value += rhs_val;
        }
    }

    template<typename Callback>
    void traverse_pre_order(Callback &&cb) {
        std::invoke(cb, this);

        if (!m_children[0]) return;

        m_children[0]->template traverse_pre_order(std::forward<Callback>(cb));
        m_children[1]->template traverse_pre_order(std::forward<Callback>(cb));
    }

    template<typename Callback>
    void traverse_post_order(Callback &&cb) {
        if (m_children[0]) {
            m_children[0]->template traverse_post_order(std::forward<Callback>(cb));
            m_children[1]->template traverse_post_order(std::forward<Callback>(cb));
        }
        std::invoke(cb, this);
    }

    u64 m_magnitude = 0;
    u64 m_depth = 0;
    Element *m_root = nullptr;
    Element *m_parent = nullptr;
    std::array<std::unique_ptr<Element>, 2> m_children{nullptr, nullptr};
    u64 m_value = 0;
};

auto add(std::unique_ptr<Element> lhs, std::unique_ptr<Element> rhs) {
    auto res = std::make_unique<Element>();
    res->m_root = res.get();

    auto l = [&](Element *node) {
        node->m_root = res->m_root;
        node->m_depth++;
    };

    lhs->traverse_pre_order(l);
    rhs->traverse_pre_order(l);
    lhs->m_parent = res.get();
    rhs->m_parent = res.get();

    res->m_children = {
      std::move(lhs),
      std::move(rhs),
    };

    return res;
}

void part_1(std::span<std::string_view> lines) {
    auto result = std::make_unique<Element>(lines[0]);

    for (auto rest = std::span<std::string_view>{lines.begin() + 1, lines.end()}; auto line: rest) {
        auto root = std::make_unique<Element>(line);

        fmt::print("adding:\n  ");
        result->print_recursive();
        fmt::print("+ ");
        root->print_recursive();
        fmt::print("= ");

        result = add(std::move(result), std::move(root));
        for (;;) {
            bool act = result->action();
            if (!act) break;
        }
        result->print_recursive();
    }
    //result->print_recursive();
    for (;;) {
        bool act = result->action();
        if (!act) break;
        //result->print_recursive();
    }

    fmt::print("finally:\n");
    result->print_recursive();

    result->traverse_post_order([](Element *node) {
        if (!node->is_leaf()) {
            node->m_magnitude = node->m_children[0]->m_magnitude * 3 + node->m_children[1]->m_magnitude * 2;
        } else {
            node->m_magnitude = node->m_value;
        }
    });

    fmt::print("magnitude sum: {}\n", result->m_magnitude);
}

void Sol::Solve(std::string_view data) {
    auto lines = GetLines(data, "\n");

    u64 max_sum = 0;
    for (u64 i = 0; i < lines.size(); i++) {
        for (u64 j = 0; j < lines.size(); j++) {
            if (i == j)continue;

            auto lhs = std::make_unique<Element>(lines[i]);
            auto rhs = std::make_unique<Element>(lines[j]);

            auto result = add(std::move(lhs), std::move(rhs));

            for (;;) {
                bool act = result->action();
                if (!act) break;
            }

            result->traverse_post_order([](Element *node) {
                if (!node->is_leaf()) {
                    node->m_magnitude = node->m_children[0]->m_magnitude * 3 + node->m_children[1]->m_magnitude * 2;
                } else {
                    node->m_magnitude = node->m_value;
                }
            });

            //fmt::print("magnitude sum: {}\n", result->m_magnitude);
            max_sum = std::max(max_sum, result->m_magnitude);
        }
    }
    fmt::print("magnitude sum: {}\n", max_sum); //high: 4808
}

}
