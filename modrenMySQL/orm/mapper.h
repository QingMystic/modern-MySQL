#pragma once
#ifndef MAPPER_H
#define MAPPER_H

#include <vector>
#include <string>

#include <logs/logs.h>

namespace modernMySQL
{
	template<typename T, size_t... I>
	T from_row_impl(const std::vector<std::string>& row,
		const std::vector<std::string>& field_names,
		std::index_sequence<I...>) {
		T result;
		auto columns = T::columns();
		auto members = T::members();

		(([&]() {
			auto column = std::get<I>(columns);
			auto member_ptr = std::get<I>(members);

			size_t index = -1;
			for (size_t i = 0; i < field_names.size(); i++) {
				if (field_names[i] == column.GetName()) {
					index = i;
					break;
				}
			}

			if (index == -1) {
				logs::error("Field name " + std::string(column.GetName()) + " not found in field names.");
			}

			using ValueType = typename std::tuple_element<I, decltype(columns)>::type::Type;

			if constexpr (std::is_same_v<ValueType, int>) 
			{
				result.*member_ptr = std::stoi(row[index]);
			}
			else if constexpr (std::is_same_v<ValueType, std::string>) 
			{
				result.*member_ptr = row[index];
			}
			else if constexpr (std::is_same_v<ValueType, float>) 
			{
				result.*member_ptr = std::stof(row[index]);
			}
			else if constexpr (std::is_same_v<ValueType, double>) 
			{
				result.*member_ptr = std::stod(row[index]);
			}
			else if constexpr (std::is_same_v<ValueType, bool>)
			{
				result.*member_ptr = (row[index] == "1" || row[index] == "true");
			}
			else 
			{
				static_assert(!sizeof(ValueType), "Unsupported type in from_row_impl");
			}
			}()), ...);

		return result;
	}

	template<typename T>
	T from_row(std::vector<std::string> row, const std::vector<std::string>& field_names)
	{
		constexpr size_t N = std::tuple_size_v<decltype(T::columns())>;
		return from_row_impl<T>(row, field_names, std::make_index_sequence<N>{});
	}
}

#endif // !MAPPER_H
