#pragma once
#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <mysql/mysql.h>
#include <logs/logs.h>

#include "../exception.h"

namespace modernMySQL
{
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
        DATABASE(
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

    extern std::vector<DATABASE> database_list;
}

#endif // DATABASE_H