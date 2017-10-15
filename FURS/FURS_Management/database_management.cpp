#include "database_management.h"
#include <QVariant>
#include <QDebug>

Database_management::Database_management()
{
    setup_database_();
}

Database_management::~Database_management()
{
    close_database_();
}

bool Database_management::update_query(std::string query_string)
{
    QSqlQuery query;
    auto query_result = query.exec(QString(query_string.c_str()));
    if (!query_result)
    {
        qDebug() << "Failed to execute query: " << query_string.c_str();
    }

    return query_result;
}

bool Database_management::result_from_query(std::string query_string, std::vector<std::vector<std::string>>& results)
{
    QSqlQuery query;
    auto query_result = query.exec(QString(query_string.c_str()));
    if (!query_result)
    {
        qDebug() << "Failed to execute query: " << query_string.c_str();
        return false;
    }

    int column_count = query.record().count();
    while (query.next())
    {
        std::vector<std::string> result;
        for (int i = 0; i < column_count; ++i)
        {
            result.emplace_back(query.value(i).toString().toStdString());
        }
        results.emplace_back(result);
    }

    return query_result;
}

bool Database_management::select_result_from_query(std::vector<std::string> select_fields, std::string table, std::string where_clause, std::vector<std::vector<std::string>>& results)
{
    std::string query_string("select ");

    // Add select fields
    for (auto field_itr = select_fields.begin(); field_itr != select_fields.end(); ++field_itr)
    {
        query_string += *field_itr;
        if (field_itr != select_fields.end())
        {
            query_string += ", ";
        }
        else
        {
            query_string += " ";
        }
    }

    // Add table name
    query_string += "from " + table + " " + where_clause;

    QSqlQuery query;
    auto query_result = query.exec(QString(query_string.c_str()));
    if (!query_result)
    {
        qDebug() << "Failed to execute query: " << query_string.c_str();
        return false;
    }

    int column_count = query.record().count();
    while (query.next())
    {
        std::vector<std::string> result;
        for (int i = 0; i < column_count; ++i)
        {
            result.emplace_back(query.value(i).toString().toStdString());
        }
        results.emplace_back(result);
    }

    return query_result;
}

void Database_management::setup_database_()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("C:\\Personal\\CGU\\FURS\\FURS\\FURS_Management\\furs_data");

    if (!m_db.open())
       {
          qDebug() << "Failed to connect to load the database";
       }
}

void Database_management::close_database_()
{
   if (m_db.open())
   {
       m_db.close();
   }
}

