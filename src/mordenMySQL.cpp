#include "mordenMySQL.h"

mordenMySQL::DATABASE::DATABASE
(
	const std::string& user,
	const std::string& password,
	const std::string& host,
	unsigned int port
)
{
	this->user_ = user;
	this->password_ = password;
	this->host_ = host;
	this->port_ = port;
	query_result_ = nullptr;

	conn_ = mysql_init(nullptr);
	if (conn_ == nullptr)
	{
		logs::error("Failed to init MySQL.");
	}
}

mordenMySQL::DATABASE::~DATABASE()
{
	if (conn_ != nullptr)
	{
		this->disconnect();
	}
}

void mordenMySQL::DATABASE::connect()
{
	if (conn_ == nullptr)
	{
		logs::error("MySQL connection is not inited.");
		throw MySQLException("MySQL connection is not inited.", 0);
	}

	if (
		mysql_real_connect
		(
			conn_, 
			host_.c_str(), 
			user_.c_str(), 
			password_.c_str(), 
			nullptr, 
			port_, 
			nullptr,
			0
		) == nullptr
		)
	{
		logs::error("Failed to connect to MySQL: " + std::string(mysql_error(conn_)));
		throw MySQLException("Failed to connect to MySQL: " + std::string(mysql_error(conn_)), mysql_errno(conn_));
	}
}

void mordenMySQL::DATABASE::disconnect()
{
	if (conn_ != nullptr)
	{
		mysql_close(conn_);
		conn_ = nullptr;
	}
}

bool mordenMySQL::DATABASE::isConnected()
{
	if (conn_ == nullptr)
	{
		return false;
	}
	return mysql_ping(conn_) == 0;
}

void mordenMySQL::DATABASE::query(const std::string& sql)
{
	if (conn_ == nullptr)
	{
		logs::error("MySQL connection is not inited.");
		throw MySQLException("MySQL connection is not inited.", 0);
	}
	if (query_result_ != nullptr)
	{
		mysql_free_result(query_result_);
		query_result_ = nullptr;
	}
	if ((mysql_query(conn_, sql.c_str())) != 0)
	{
		logs::error("Failed to execute query: " + std::string(mysql_error(conn_)));
		throw MySQLException("Failed to execute query: " + std::string(mysql_error(conn_)), mysql_errno(conn_));
	}
	query_result_ = mysql_store_result(conn_);
	if (mysql_errno(conn_) != 0)
	{
		logs::error("Failed to store result: " + std::string(mysql_error(conn_)));
		throw MySQLException("Failed to store result: " + std::string(mysql_error(conn_)), mysql_errno(conn_));
	}
}

std::vector<std::string> mordenMySQL::DATABASE::fetchRow()
{
	if (query_result_ == nullptr)
	{
		logs::error("MySQL result is null.");
		throw MySQLException("MySQL result is null.", 0);
	}
	if (conn_ == nullptr)
	{
		logs::error("MySQL connection is not inited.");
		throw MySQLException("MySQL connection is not inited.", 0);
	}
	MYSQL_ROW row = mysql_fetch_row(query_result_);
	if (row == nullptr && mysql_errno(conn_) != 0)
	{
		logs::error("Failed to fetch row: " + std::string(mysql_error(conn_)));
		throw MySQLException("Failed to fetch row: " + std::string(mysql_error(conn_)), mysql_errno(conn_));
	}
	if (row == nullptr)
	{
		return std::vector<std::string>();
	}

	std::vector<std::string> row_vector;
	unsigned int num_fields = mysql_num_fields(query_result_);
	for (unsigned int i = 0; i < num_fields; i++)
	{
		row_vector.push_back(row[i] ? row[i] : "NULL");
	}

	return row_vector;
}

std::vector<std::vector<std::string>> mordenMySQL::DATABASE::getResultVector()
{
	std::vector<std::vector<std::string>> result_vector;
	if (query_result_ == nullptr)
	{
		logs::error("MySQL result is null.");
		throw MySQLException("MySQL result is null.", 0);
	}
	if (conn_ == nullptr)
	{
		logs::error("MySQL connection is not inited.");
		throw MySQLException("MySQL connection is not inited.", 0);
	}
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(query_result_)) != nullptr)
	{
		std::vector<std::string> row_vector;
		for (unsigned int i = 0; i < mysql_num_fields(query_result_); i++)
		{
			row_vector.push_back(row[i] ? row[i] : "NULL");
		}
		result_vector.push_back(row_vector);
	}
	if (mysql_errno(conn_) != 0)
	{
		logs::error("Failed to fetch rows: " + std::string(mysql_error(conn_)));
		throw MySQLException("Failed to fetch rows: " + std::string(mysql_error(conn_)), mysql_errno(conn_));
	}
	return result_vector;
}

unsigned long long mordenMySQL::DATABASE::getRowCount()
{
	if (query_result_ == nullptr)
	{
		logs::error("MySQL result is null.");
		throw MySQLException("MySQL result is null.", 0);
	}
	return mysql_num_rows(query_result_);
}

int mordenMySQL::DATABASE::getFieldCount()
{
	if (query_result_ == nullptr)
	{
		logs::error("MySQL result is null.");
		throw MySQLException("MySQL result is null.", 0);
	}
	return mysql_num_fields(query_result_);
}

std::vector<std::string> mordenMySQL::DATABASE::getFieldsName()
{
	std::vector<std::string> field_name_list;
	if (query_result_ == nullptr)
	{
		logs::error("MySQL result is null.");
		throw MySQLException("MySQL result is null.", 0);
	}

	mysql_field_seek(query_result_, 0);

	MYSQL_FIELD* field;
	while ((field = mysql_fetch_field(query_result_)) != nullptr)
	{
		field_name_list.push_back(field->name);
	}
	return field_name_list;
}

std::string mordenMySQL::DATABASE::getError()
{
	return mysql_error(conn_);
}

int mordenMySQL::DATABASE::getErrorCode()
{
	return mysql_errno(conn_);
}
