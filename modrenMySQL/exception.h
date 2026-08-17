#pragma once
#ifndef EXCEPTION_H
#define EXCEPTION_H
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
#include <exception>
#include <string>

namespace modernMySQL
{
	class MySQLException : public std::exception
	{
	private:
		std::string message_;
		int error_code_;

	public:
		MySQLException(const std::string& message, int error_code)
			: message_(message), error_code_(error_code) {}
		const char* what() const noexcept override { return message_.c_str(); }
		int getErrorCode() const noexcept { return error_code_; }
	};
}

#endif // !EXCEPTION_H