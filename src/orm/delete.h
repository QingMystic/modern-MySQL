#pragma once
#ifndef DELETE_H
#define DELETE_H

#include <string>
#include <variant>


namespace modernMySQL
{
	template<typename Table, typename ConditionType = std::monostate>
	struct DeleteBuilder
	{
		ConditionType condition_;

		DeleteBuilder() = default;
		DeleteBuilder(ConditionType condition) : condition_(condition) {}

		template<typename ConditionType>
		auto where(ConditionType condition) const
		{
			return DeleteBuilder<Table, ConditionType>{condition};
		}

		std::string to_sql() const
		{
			std::string sql = "DELETE FROM ";
			sql += Table::GetName().data();
			if constexpr (!std::is_same_v<ConditionType, std::monostate>) {
				sql += " WHERE ";
				sql += condition_.to_sql();
			}
			return sql;
		}
	};

	template<typename Table>
	auto delete_from(Table) {
		return DeleteBuilder<Table>{};
	}
}

#endif // !DELETE_H
