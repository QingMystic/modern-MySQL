#pragma once
#ifndef INSERT_H
#define INSERT_H

#include <string>
#include <tuple>
#include <type_traits>

namespace modernMySQL
{
    template<typename ColType, typename ValType>
    struct InsertValue
    {
        using ColumnType = ColType;
        using ValueType = ValType;

        InsertValue(ValType value) : value(value) {}
        ValType value;
        ColType column;
    };

    template<typename Table, typename... InsertValues>
    struct InsertBuilder
    {
        std::tuple<InsertValues...> values_;

        InsertBuilder(InsertValues... values) : values_(values...) {}

        template<typename... Values>
        auto set(Values... values)
        {
            return InsertBuilder<Table, InsertValue<Values, typename Values::Type>...>{values.value_...};
        }

        std::string to_sql() const
        {
            std::string sql = "INSERT INTO ";
            sql += Table::GetName().data();
            sql += " (";

            bool first = true;
            std::apply([&](auto... vals) {
                (void)std::initializer_list<int>{
                    (first ? (first = false) : (sql += ", "),
                        sql += vals.ColumnType::GetName().data(),
                        0)...
                };
                }, values_);

            sql += ") VALUES (";

            first = true;
            std::apply([&](auto... vals) {
                (void)std::initializer_list<int>{
                    (first ? (first = false) : (sql += ", "),
                        sql += to_value_string(vals.value),
                        0)...
                };
                }, values_);

            sql += ")";
            return sql;
        }

    private:
        template<typename T>
        std::string to_value_string(const T& value) const
        {
            if constexpr (std::is_same_v<T, std::string>) {
                return "'" + value + "'";
            }
            else if constexpr (std::is_same_v<T, const char*>) {
                return "'" + std::string(value) + "'";
            }
            else if constexpr (std::is_integral_v<T>) {
                return std::to_string(value);
            }
            else if constexpr (std::is_floating_point_v<T>) {
                return std::to_string(value);
            }
            else {
                return "NULL";
            }
        }
    };

    template<typename Table>
    auto insert_into(Table)
    {
        return InsertBuilder<Table>{};
    }
}

#endif