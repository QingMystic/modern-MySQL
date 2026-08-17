#pragma once
#ifndef UPDATE_H
#define UPDATE_H

#include <string>
#include <tuple>
#include <variant>
#include <type_traits>
#include "condition.h"

namespace modernMySQL
{
	template<typename ColType, typename ValType>
	struct UpdateValue
	{
		using ColumnType = ColType;
		using ValueType = ValType;

		UpdateValue(ValType value) : value(value) {}
		ValType value;
		ColType column;
	};

	template<typename Table, typename ConditionType = std::monostate, typename... UpdateValues>
	struct UpdateBuilder
	{
		std::tuple<UpdateValues...> values_;
		ConditionType condition_;

		UpdateBuilder(UpdateValues... values) : values_(values...) {}
		UpdateBuilder(std::tuple<UpdateValues...> values, ConditionType condition)
			: values_(values), condition_(condition) {}

		std::string to_sql() const
		{
			std::string sql = "UPDATE ";
			sql += Table::GetName().data();
			sql += " SET ";

			constexpr size_t N = std::tuple_size_v<decltype(values_)>;
			for (size_t i = 0; i < N; ++i) {
				if (i > 0) sql += ", ";
				sql += std::get<i>(values_).ColumnType::GetName().data();
				sql += " = ?";
			}

			if constexpr (!std::is_same_v<ConditionType, std::monostate>) {
				sql += " WHERE ";
				sql += condition_.to_sql();
			}

			return sql;
		}

		template<typename... Values>
		auto set(Values... values)
		{
			return UpdateBuilder<Table, std::monostate, UpdateValue<Values, typename Values::Type>...>{values.value_...};
		}

		template<typename NewCondition>
		auto where(NewCondition condition)
		{
			return UpdateBuilder<Table, NewCondition, UpdateValues...>{values_, condition};
		}
	};

	template<typename Table>
	auto update(Table)
	{
		return UpdateBuilder<Table>{};
	}
}

#endif