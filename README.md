# modern MySQL

[![stars](https://badgen.net/github/stars/QingMystic/modern-MySQL)](https://github.com/QingMystic/modern-MySQL/stargazers)
[![forks](https://badgen.net/github/forks/QingMystic/modern-MySQL)](https://github.com/QingMystic/modern-MySQL/network/members)
[![issues](https://badgen.net/github/issues/QingMystic/modern-MySQL)](https://github.com/QingMystic/modern-MySQL/issues)
[![license](https://badgen.net/github/license/QingMystic/modern-MySQL)](https://github.com/QingMystic/modern-MySQL/blob/main/LICENSE)
[![release](https://badgen.net/github/release/QingMystic/modern-MySQL)](https://github.com/QingMystic/modern-MySQL/releases)
[![last commit](https://badgen.net/github/last-commit/QingMystic/modern-MySQL)](https://github.com/QingMystic/modern-MySQL/commits/main)
[![C++](https://badgen.net/badge/C++/17/blue?icon=github)](https://isocpp.org/)
[![platform](https://badgen.net/badge/platform/Windows/lightgrey)](https://github.com/QingMystic/modern-MySQL)

## 项目介绍

**modern-MySQL** 是一个基于 MySQL C API 的现代 C++ 封装库。它提供了简单、直观且类型安全的接口，用于执行SQL查询、处理结果集以及管理事务。

### 主要特性
*   **简单易用**：封装了复杂的 C API，提供 `query()`、`execute()` 等简洁方法。
*   **结果处理**：自动将结果集转换为 `std::vector`，方便在 C++ 中使用。
*   **事务支持**：支持 `beginTransaction()`、`commit()` 和 `rollback()`。
*   **异常安全**：统一的 `MySQLException` 异常处理机制。
*   **内存安全**：自动管理结果集内存，防止资源泄漏。

---
## 项目依赖

**modern-MySQL** 依赖于 MySQL C API 和 QingMystic logs。MySQL C API 支持到 8.0.46 版本，QingMystic logs 支持到 0.1.0 版本。在使用前需要确保项目中已安装 MySQL 开发库和 QingMystic logs。项目暂时只支持 Windows 平台，后续将计划支持 linux 平台。<br>
下载依赖项：
*  [MySQL C API](https://dev.mysql.com/downloads/connector/c/)
*  [QingMystic logs](https://github.com/QingMystic/QingMystic-logs)

---
## 快速开始
```cpp
#include <iostream>
#include "modernMySQL.h"

int main() 
{
    try 
    {
        // 1. 创建数据库对象并连接
        modernMySQL::DATABASE db("your_user", "your_password", "127.0.0.1", 3306);
        db.connect();

        // 2. 执行查询
        db.query("SELECT * FROM users");
        
        // 3. 获取所有结果
        auto rows = db.getResultVector();
        
        // 4. 打印结果
        for (const auto& row : rows) 
        {
            for (const auto& field : row) 
            {
                std::cout << field << "\t";
            }
            std::cout << std::endl;
        }
    }
    catch (const mordenMySQL::MySQLException& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        std::cerr << "错误码: " << e.getErrorCode() << std::endl;
    }
    
    return 0;
}

```

---

## API 参考
### 连接管理

| 方法 | 描述 |
|------|------|
| `DATABASE(user, password, host, port)` | 构造函数，初始化连接参数 |
| `~DATABASE()` | 析构函数，自动释放资源 |
| `connect()` | 连接 MySQL 服务器，失败抛出 `MySQLException` |
| `disconnect()` | 断开连接 |
| `isConnected()` | 返回 `true` 表示连接正常 |

### 查询执行

| 方法 | 描述 |
|------|------|
| `query(const std::string& sql)` | 执行 SELECT 查询，结果存储在内部 |
| `execute(const std::string& sql)` | 执行 INSERT/UPDATE/DELETE，返回影响行数 |
| `fetchRow()` | 逐行获取结果，返回 `vector<string>`，取完返回空 |
| `getResultVector()` | 以 `vector<vector<string>>` 返回所有结果 |

### 结果集信息

| 方法 | 描述 |
|------|------|
| `getRowCount()` | 返回当前结果集的行数 |
| `getFieldCount()` | 返回当前结果集的列数 |
| `getFieldsName()` | 返回当前结果集的列名列表 |

### 事务控制

| 方法 | 描述 |
|------|------|
| `beginTransaction()` | 开始一个新事务 |
| `commit()` | 提交当前事务 |
| `rollback()` | 回滚当前事务 |

### 错误处理

| 方法 | 描述 |
|------|------|
| `getError()` | 获取最后一次错误信息 |
| `getErrorCode()` | 获取最后一次错误码 |

### 异常类

```cpp
class MySQLException : public std::exception {
public:
    MySQLException(const std::string& message, int error_code);
    const char* what() const noexcept override;
    int getErrorCode() const noexcept;
};
```