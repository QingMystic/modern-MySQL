#pragma once
#ifndef MODERNMYSQL_H
#define MODERNMYSQL_H

#include <iostream>
#include <string>
#include <vector>
#include <mysql/mysql.h>
#include <logs/logs.h>

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

		const char* what() const noexcept override
		{
			return message_.c_str();
		}

		int getErrorCode() const noexcept
		{
			return error_code_;
		}
	};

	class DATABASE
	{
	private:
		std::string user_;
		std::string password_;
		std::string host_;
		unsigned int port_;
		MYSQL* conn_;
		MYSQL_RES* query_result_;
		bool in_transaction_ = false;

	public:
		DATABASE
		(
			const std::string& user,
			const std::string& password,
			const std::string& host,
			unsigned int port
		);
		~DATABASE();

		void connect();
		void disconnect();
		bool isConnected();

		void query(const std::string& sql);
		std::vector<std::string> fetchRow();
		std::vector<std::vector<std::string>> getResultVector();
		unsigned long long getRowCount();
		int getFieldCount();
		std::vector<std::string> getFieldsName();

		unsigned long long execute(const std::string& sql);

		void beginTransaction();
		void commit();
		void rollback();

		std::string getError();
		int getErrorCode();
	};

	std::vector<DATABASE> database_list;
}

#endif // !MODERNMYSQL_H
