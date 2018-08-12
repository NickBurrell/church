#pragma once

#include "algorithm.hpp"

#include <array>
#include <string_view>

namespace church{
    template<typename Value, std::size_t Size>
    class Vector {

    public:
        using data_t = std::array<Value, Size>;
        using iterator = typename data_t::iterator;
        using const_iterator = typename data_t::const_iterator;
        using value_type = Value;
        using reference = typename data_t::reference;
        using const_reference = typename data_t::const_reference;

        template<typename Iter>
        constexpr Vector(Iter begin, const Iter& end) {
            while (begin != end) {
                push_back(*begin);
                ++begin;
            }
        }

        constexpr Vector() = default;

        constexpr auto begin() const {
            return _internal.begin();
        }

        constexpr auto end() const {
            return _internal.end();
        }

        constexpr Value& operator[](const std::size_t index) {
            return _internal[index];
        }

        constexpr Value& at(const std::size_t index) {
            if (index >= _internal.size())
                throw std::range_error("Index out of array bounds");
            else
                return _internal[index];
        }

        constexpr const Value& at(const std::size_t index) const {
            if (index >= _internal.size())
                throw std::range_error("Index out of array bounds");
            else
                return _internal[index];
        }

        constexpr Value& push_back(Value value) {
            if (_size   >= Size)
                throw std::range_error("Index out of array bounds");
            else {
                Value& v = _internal[_size++];
                v = std::move(value);
                return v;
            }
        }


        constexpr Value& back() {
            if (empty())
                throw std::range_error("Array is empty");
            else
                return _internal[_size-1];
        }
        constexpr const Value& back() const {
            if (empty())
                throw std::range_error("Array is empty");
            else
                return _internal[_size-1];
        }

        constexpr auto capacity() const {return Size;}
        constexpr auto size() const {return _size;}
        constexpr auto empty() const {return _size == 0;}

        constexpr void clear() {_size = 0;}

        constexpr const Value* data() const {
            return _internal.data();
        }

    private:
        data_t _internal;
        std::size_t _size{0};
    };

    template<typename Value, std::size_t Size>
    constexpr bool operator==(const Vector<Value, Size>& lhs, const Vector<Value, Size>& rhs) {
        return (rhs.begin() == lhs.begin()) && (lhs.end() == rhs.end());
    }


}
