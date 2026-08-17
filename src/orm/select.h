#pragma once
#ifndef SELECT_H
#define SELECT_H

#include <string_view>
#include <string>

#include "core/column.h"
#include "core/table.h"

namespace modernMySQL 
{
	template<typename Table, typename ... Columns>
	struct SelectBuilder;

	template<typename Table, typename ConditionType, typename ... Columns>
	struct SelectBuilderWithCondition;

	template<typename ... Columns>
	struct SelectBuilderWithoutTable
	{
		template<typename Table>
		auto from(Table) const
		{
			return SelectBuilder<Table, Columns...>{};
		}
	};

	template<typename Table, typename... Columns>
	struct SelectBuilder
	{
		std::string to_sql() const
		{
			std::string sql = "SELECT ";
			((sql += Columns::GetName().data(), sql += ", "), ...);
			sql.pop_back();
			sql.pop_back();
			sql += " FROM ";
			sql += Table::GetName().data();
			return sql;
		}

		template<typename ConditionType>
		auto where(ConditionType condition) const
		{
			return SelectBuilderWithCondition<Table, ConditionType, Columns...>{condition};
		}
	};

	template<typename Table, typename ConditionType, typename ... Columns>
	struct SelectBuilderWithCondition
	{
		SelectBuilderWithCondition(ConditionType condition) : condition_(condition) {}

		ConditionType condition_;

		std::string to_sql() const
		{
			std::string sql = "SELECT ";
			((sql += Columns::GetName().data(), sql += ", "), ...);
			sql.pop_back();
			sql.pop_back();
			sql += " FROM ";
			sql += Table::GetName().data();
			sql += " WHERE ";
			sql += condition_.to_sql();
			return sql;
		}
	};

	template<typename ... Columns>
	auto select(Columns... cols)
	{
		return SelectBuilderWithoutTable<Columns...>{};
	}
}

#endif // !SELECT_H