#pragma once

#include <utility>
#include <optional>

namespace church {
    template<typename T>
    class Maybe {
        using value_type = T;
        constexpr Maybe(std::nullopt_t) {}
        constexpr explicit Maybe(const T& t)
            : _valid(true), _value(t) {}

        constexpr explicit operator bool() const {return _valid;}

        constexpr const T* operator->() const {return &_value;}
        constexpr T* operator->() {return &_value;}
        constexpr const T& operator*() const {return _value;}
        constexpr T& operator*() {return _value;}
    private:
        bool _valid = false;
        T _value;
    };
}
