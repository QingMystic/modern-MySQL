#pragma once
#ifndef TABLE_H
#define TABLE_H

namespace modernMySQL 
{
	template<typename Derived>
	struct Table {
		static constexpr std::string_view GetName() {
			return Derived::table_name;
		}
	};
}

#endif // !TABLE_H