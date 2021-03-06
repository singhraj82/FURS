#ifndef DATABASE_MANAGEMENT_H
#define DATABASE_MANAGEMENT_H

#include <map>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

class Database_management
{
public:
    Database_management();
    virtual ~Database_management();

    bool update_query(std::string query_string);
    bool result_from_query(std::string query_string, std::vector<std::vector<std::string>>& results);
    bool select_result_from_query(std::vector<std::string> select_fields, std::string table, std::string where_clause, std::vector<std::vector<std::string>>& results);
    void close_database();

protected:
    void setup_database_();
    void close_database_();

protected:
    QSqlDatabase m_db;

};

#endif // DATABASE_MANAGEMENT_H
