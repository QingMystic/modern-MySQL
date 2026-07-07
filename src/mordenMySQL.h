#pragma once
#ifndef MORDENMYSQL_H
#define MORDENMYSQL_H

#include <iostream>
#include <string>
#include <vector>
#include <mysql/mysql.h>
#include <logs/logs.h>

namespace mordenMySQL
{
	class DATABASE
	{
	private:
		std::string user;
		std::string password;
		std::string host;
		unsigned int port;
		MYSQL* conn;

	public:
		DATABASE
		(
			std::string user,
			std::string password,
			std::string host,
			unsigned int port
		);
		~DATABASE();

		void connect();
		void disconnect();
		bool isConnected();
	};

	std::vector<DATABASE> database_list;
}

#endif // !MORDENMYSQL_H
