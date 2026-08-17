#pragma once
#ifndef CONDITION_H
#define CONDITION_H

#include <string>

namespace modernMySQL
{
    struct Equal 
	{
        static constexpr const char* op = "=";
    };
    struct NotEqual 
	{
        static constexpr const char* op = "!=";
    };
    struct Greater 
	{
        static constexpr const char* op = ">";
    };
    struct GreaterEqual 
	{
        static constexpr const char* op = ">=";
    };
    struct Less 
	{
        static constexpr const char* op = "<";
    };
    struct LessEqual 
	{
        static constexpr const char* op = "<=";
    };

	template<typename ColumnType, typename Operator, typename ValueType>
	struct Condition
	{
		auto to_sql() const
		{
			return std::string(std::string(ColumnType::GetName().data())) + " " + Operator::op + " " + "?";
		}
	};

	template<typename ColumnType, typename ValueType>
	auto operator==(ColumnType, ValueType) 
	{
		return Condition<ColumnType, Equal, ValueType>{};
	}
    
	template<typename ColumnType, typename ValueType>
	auto operator!=(ColumnType, ValueType) 
	{
		return Condition<ColumnType, NotEqual, ValueType>{};
	}

	template<typename ColumnType, typename ValueType>
	auto operator>(ColumnType, ValueType) 
	{
		return Condition<ColumnType, Greater, ValueType>{};
	}

	template<typename ColumnType, typename ValueType>
	auto operator>=(ColumnType, ValueType) 
	{
		return Condition<ColumnType, GreaterEqual, ValueType>{};
	}

	template<typename ColumnType, typename ValueType>
	auto operator<(ColumnType, ValueType) 
	{
		return Condition<ColumnType, Less, ValueType>{};
	}

	template<typename ColumnType, typename ValueType>
	auto operator<=(ColumnType, ValueType) 
	{
		return Condition<ColumnType, LessEqual, ValueType>{};
	}
}

#endif // !CONDITION_H
