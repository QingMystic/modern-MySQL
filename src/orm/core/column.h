#pragma once
#ifndef COLUMN_H
#define COLUMN_H

#include <tuple>

#include "string_literal.h"

namespace modernMySQL 
{
	template<StringLiteral name, typename T, typename ... args>
	struct Column 
	{
		Column(T value) : value_(value) {}
		Column() = default;

		static constexpr std::string_view GetName()
		{
			return name.view();
		}

		using Type = T;
		using tags = std::tuple<args...>;

		T value_;
	};
}

#endif // !COLUMN_H
