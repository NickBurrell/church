#pragma once

#include "vector.hpp"

#include <string_view>

namespace church {
    class StaticString {

    public:
        template<std::size_t Len>
        constexpr StaticString(const char (&str)[Len]) :
            _internal(str[0]), _size(Len) {}

        constexpr StaticString(const char* str, std::size_t size) :
            _internal(str), _size(size) {}

        constexpr StaticString() :
            _internal(""), _size(0) {}

        constexpr std::size_t size() const {return _size;}

        constexpr const char * c_str() const {
            return _internal;
        }

        constexpr const char * begin() const {
            return _internal;
        }

        constexpr const char* end() const {
            return _internal + _size;
        }


        const char* _internal;
        std::size_t _size{0};

    };
    template<typename CharType, std::size_t Size>
    class BasicString : Vector<CharType, Size> {

        constexpr BasicString(const StaticString& s) :
            Vector<CharType, Size>(s.begin(), s.end()) {}
        constexpr BasicString(const std::string_view &s) :
            Vector<CharType, Size>(s.begin(), s.end()) {}

        constexpr BasicString() :
            Vector<CharType, Size>() {}

        constexpr BasicString& operator=(const StaticString& s) {
            return *this = BasicString(s);
        }
        constexpr BasicString& operator=(const std::string_view& s) {
            return *this = BasicString(s);
        }

        constexpr const char * c_str() const {
            return this->data();
        }
    };

    template<typename CharType, std::size_t Size>
    constexpr bool operator==(const BasicString<CharType, Size> &lhs, const StaticString &rhs) {
        return (lhs.begin() == rhs.begin()) && (lhs.end() == rhs.end());
    }

    using String = BasicString<char, 32>;
}
