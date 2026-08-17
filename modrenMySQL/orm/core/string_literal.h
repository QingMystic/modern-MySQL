#pragma once
#ifndef STRING_LITERAL_H
#define STRING_LITERAL_H

#include <string_view>

namespace modernMySQL
{
    template<size_t N>
    struct StringLiteral {
        char data[N];
        constexpr StringLiteral(const char(&str)[N]) {
            for (size_t i = 0; i < N; ++i) {
                data[i] = str[i];
            }
        }

        constexpr std::string_view view() const {
            return std::string_view(data, N - 1);
        }
    };

    template<size_t N>
    StringLiteral(const char(&)[N]) -> StringLiteral<N>;
}

#endif // !STRING_LITERAL_H
