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

void Database_management::update_query(std::string query_string)
{
    //query_string = "INSERT  INTO  applications(id,last_name,first_name,age,gender,phone,street_address,city,state,zip_code,zip_code,app_status,payment_status,camp) VALUES('{ss6f52bc-adce-4a57-846f-7738e2870a32}','Bush','sddasadsdsdd','guid.toString().toStdString()','Not selected','','','','Not selected','','Not selected','Not selected','Not selected','Not selected')";
    QSqlQuery query;
    if (!query.exec(QString(query_string.c_str())))
    {
        qDebug() << "Failed to execute query: " << query_string.c_str();
    }
}

std::vector<std::vector<std::string>> Database_management::result_from_query(std::string query_string)
{
    QSqlQuery query;
    QString aa(query_string.c_str());
    if(!query.exec(aa))
    {
        qDebug() << "Error";
    }

    int column_count = query.record().count();
    std::vector<std::vector<std::string>> results;
    while (query.next())
    {
        std::vector<std::string> result;
        for (int i = 0; i < column_count; ++i)
        {
            result.emplace_back(query.value(i).toString().toStdString());
        }
        results.emplace_back(result);
    }

    return results;
}

std::vector<std::vector<std::string>> Database_management::select_result_from_query( std::vector<std::string> select_fields,  std::string table, std::string where_clause)
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

