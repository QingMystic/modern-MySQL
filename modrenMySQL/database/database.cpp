#include "database.h"
#include <mysql/mysql.h>
#include <stdexcept>
#include <cstring>

namespace modernMySQL
{
    DATABASE::DATABASE(
        const std::string& user,
        const std::string& password,
        const std::string& host,
        unsigned int port
    )
        : user_(user), password_(password), host_(host), port_(port)
    {
        query_result_ = nullptr;
        conn_ = mysql_init(nullptr);
        if (conn_ == nullptr)
        {
            logs::error("Failed to init MySQL.");
            throw MySQLException("Failed to init MySQL.", 0);
        }
    }

    DATABASE::~DATABASE()
    {
        if (in_transaction_ == true)
        {
            try
            {
                this->rollback();
            }
            catch (const MySQLException& e)
            {
                logs::error("Failed to rollback transaction in destructor: " + std::string(e.what()));
            }
        }
        if (conn_ != nullptr)
        {
            this->disconnect();
        }
    }

    void DATABASE::connect()
    {
        if (conn_ == nullptr)
        {
            logs::error("MySQL connection is not inited.");
            throw MySQLException("MySQL connection is not inited.", 0);
        }

        if (mysql_real_connect(
            conn_,
            host_.c_str(),
            user_.c_str(),
            password_.c_str(),
            nullptr,
            port_,
            nullptr,
            0
        ) == nullptr)
        {
            std::string err = mysql_error(conn_);
            logs::error("Failed to connect to MySQL: " + err);
            throw MySQLException("Failed to connect to MySQL: " + err, mysql_errno(conn_));
        }
    }

    void DATABASE::disconnect()
    {
        if (conn_ != nullptr)
        {
            mysql_close(conn_);
            conn_ = nullptr;
        }
    }

    bool DATABASE::isConnected()
    {
        if (conn_ == nullptr)
        {
            return false;
        }
        return mysql_ping(conn_) == 0;
    }

    void DATABASE::query(const std::string& sql)
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
        if (mysql_query(conn_, sql.c_str()) != 0)
        {
            std::string err = mysql_error(conn_);
            logs::error("Failed to execute query: " + err);
            throw MySQLException("Failed to execute query: " + err, mysql_errno(conn_));
        }
        query_result_ = mysql_store_result(conn_);
        if (mysql_errno(conn_) != 0)
        {
            std::string err = mysql_error(conn_);
            logs::error("Failed to store result: " + err);
            throw MySQLException("Failed to store result: " + err, mysql_errno(conn_));
        }
    }

    std::vector<std::string> DATABASE::fetchRow()
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
            std::string err = mysql_error(conn_);
            logs::error("Failed to fetch row: " + err);
            throw MySQLException("Failed to fetch row: " + err, mysql_errno(conn_));
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

    std::vector<std::vector<std::string>> DATABASE::getResultVector()
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
            std::string err = mysql_error(conn_);
            logs::error("Failed to fetch rows: " + err);
            throw MySQLException("Failed to fetch rows: " + err, mysql_errno(conn_));
        }
        return result_vector;
    }

    unsigned long long DATABASE::getRowCount()
    {
        if (query_result_ == nullptr)
        {
            logs::error("MySQL result is null.");
            throw MySQLException("MySQL result is null.", 0);
        }
        return mysql_num_rows(query_result_);
    }

    int DATABASE::getFieldCount()
    {
        if (query_result_ == nullptr)
        {
            logs::error("MySQL result is null.");
            throw MySQLException("MySQL result is null.", 0);
        }
        return mysql_num_fields(query_result_);
    }

    std::vector<std::string> DATABASE::getFieldsName()
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

    unsigned long long DATABASE::execute(const std::string& sql)
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
        if (mysql_query(conn_, sql.c_str()) != 0)
        {
            std::string err = mysql_error(conn_);
            logs::error("Failed to execute query: " + err);
            throw MySQLException("Failed to execute query: " + err, mysql_errno(conn_));
        }
        return mysql_affected_rows(conn_);
    }

    void DATABASE::beginTransaction()
    {
        if (conn_ == nullptr)
        {
            logs::error("MySQL connection is not inited.");
            throw MySQLException("MySQL connection is not inited.", 0);
        }
        if (mysql_autocommit(conn_, 0) != 0)
        {
            std::string err = mysql_error(conn_);
            logs::error("Failed to begin transaction: " + err);
            throw MySQLException("Failed to begin transaction: " + err, mysql_errno(conn_));
        }
        in_transaction_ = true;
    }

    void DATABASE::commit()
    {
        if (conn_ == nullptr)
        {
            logs::error("MySQL connection is not inited.");
            throw MySQLException("MySQL connection is not inited.", 0);
        }
        if (mysql_commit(conn_) != 0)
        {
            std::string err = mysql_error(conn_);
            logs::error("Failed to commit transaction: " + err);
            throw MySQLException("Failed to commit transaction: " + err, mysql_errno(conn_));
        }
        in_transaction_ = false;
    }

    void DATABASE::rollback()
    {
        if (conn_ == nullptr)
        {
            logs::error("MySQL connection is not inited.");
            throw MySQLException("MySQL connection is not inited.", 0);
        }
        if (mysql_rollback(conn_) != 0)
        {
            std::string err = mysql_error(conn_);
            logs::error("Failed to rollback transaction: " + err);
            throw MySQLException("Failed to rollback transaction: " + err, mysql_errno(conn_));
        }
        in_transaction_ = false;
    }

    std::string DATABASE::getError()
    {
        return mysql_error(conn_);
    }

    int DATABASE::getErrorCode()
    {
        return mysql_errno(conn_);
    }

    std::vector<DATABASE> database_list;
}