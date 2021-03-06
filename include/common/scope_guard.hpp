#pragma once

#include <functional>

namespace AOC::Utils::SG {

enum Condition {
    Failure, Success, Any
};

template<Condition condition, typename Callable>
struct ScopeGuard {
    constexpr ScopeGuard() noexcept = delete;

    constexpr explicit ScopeGuard(Callable &&cb) noexcept(noexcept(Callable(std::forward<Callable>(cb))))
      : cb(std::forward<Callable>(cb)) {}

    constexpr explicit ScopeGuard(const Callable &cb) noexcept(noexcept(Callable(cb)))
      : cb(cb) {}

    constexpr ~ScopeGuard() noexcept {
        if (!armed) return;
        if ((condition == Condition::Failure && std::current_exception()) ||
            (condition == Condition::Success && !std::current_exception()) ||
            (condition == Condition::Any))
            std::invoke(cb);
    }

    bool armed = true;

  private:
    Callable cb;
};

template<typename Callable>
constexpr auto MakeSuccessGuard(Callable &&cb) { return ScopeGuard<Condition::Success, Callable>(std::forward<Callable>(cb)); }

template<typename Callable>
constexpr auto MakeFailureGuard(Callable &&cb) { return ScopeGuard<Condition::Failure, Callable>(std::forward<Callable>(cb)); }

template<typename Callable>
constexpr auto MakeGuard(Callable &&cb) { return ScopeGuard<Condition::Any, Callable>(std::forward<Callable>(cb)); }

}
