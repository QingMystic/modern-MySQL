#include "mordenMySQL.h"

mordenMySQL::DATABASE::DATABASE
(
	std::string user,
	std::string password,
	std::string host,
	unsigned int port
)
{
	this->user = user;
	this->password = password;
	this->host = host;
	this->port = port;

	mysql_init(conn);
	if (conn == nullptr)
	{
		logs::error("Failed to init MySQL.");
	}
}

mordenMySQL::DATABASE::~DATABASE()
{
	if (conn != nullptr)
	{
		this->disconnect();
	}
}

void mordenMySQL::DATABASE::connect()
{
	if (conn == nullptr)
	{
		logs::error("MySQL connection is not inited.");
		return;
	}

	if (
		mysql_real_connect
		(
			conn, 
			host.c_str(), 
			user.c_str(), 
			password.c_str(), 
			nullptr, 
			port, 
			nullptr,
			0
		) == nullptr
		)
	{
		logs::error("Failed to connect to MySQL: " + std::string(mysql_error(conn)));
	}
}

void mordenMySQL::DATABASE::disconnect()
{
	if (conn != nullptr)
	{
		mysql_close(conn);
		conn = nullptr;
	}
}

bool mordenMySQL::DATABASE::isConnected()
{
	if (conn == nullptr)
	{
		return false;
	}
	return mysql_ping(conn) == 0;
}
