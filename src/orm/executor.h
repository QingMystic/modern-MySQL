#pragma once
#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "../database/database.h"
#include "select.h"

namespace modernMySQL
{
	template<typename Table, typename ... Columns>
	auto fetch(DATABASE& database, SelectBuilder<Table, Columns...> select_builder)
	{
		std::string sql = select_builder.to_sql();
		database.query(sql);
		auto field_names = database.getFieldsName();
		return database.getResultVector();
	}

	template<typename Builder>
	unsigned long long execute(DATABASE& database, Builder builder)
	{
		std::string sql = builder.to_sql();
		return database.execute(sql);
	}
}

#endif // !EXECUTOR_H
