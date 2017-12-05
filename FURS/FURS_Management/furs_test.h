#ifndef FURS_TEST_H
#define FURS_TEST_H

#include <QtWidgets>
#include <QtTest/QtTest>
#include "application.h"
#include "database_management.h"
#include "generate_letter.h"

class Furs_test : public QObject
{
Q_OBJECT
public:
    Furs_test(){}

private slots:
    void test_application_last_name()
    {
        Application application;
        application.set_last_name("singh");
        QCOMPARE(QString("singh"), QString(application.last_name().c_str()));
    }

    void test_application_first_name()
    {
        Application application;
        application.set_first_name("Raj");
        QCOMPARE(QString("Raj"), QString(application.first_name().c_str()));
    }

    void test_application_gender()
    {
        Application application;
        application.set_gender("Male");
        QCOMPARE(QString("Male"), QString(application.gender().c_str()));
    }

    void test_application_set_age()
    {
        Application application;
        application.set_age(15);
        QCOMPARE(15, application.age());
    }

    void test_application_phone()
    {
        Application application;
        application.set_phone("1234567890");
        QCOMPARE(QString("1234567890"), QString(application.phone().c_str()));
    }

    void test_application_street_address()
    {
        Application application;
        application.set_street_address("123 test");
        QCOMPARE(QString("123 test"), QString(application.street_address().c_str()));
    }

    void test_application_city()
    {
        Application application;
        application.set_city("test");
        QCOMPARE(QString("test"), QString(application.city().c_str()));
    }

    void test_application_state()
    {
        Application application;
        application.set_state("test_state");
        QCOMPARE(QString("test_state"), QString(application.state().c_str()));
    }

    void test_application_zipcode()
    {
        Application application;
        application.set_zipcode("12345");
        QCOMPARE(QString("12345"), QString(application.zipcode().c_str()));
    }

    void test_application_speciality()
    {
        Application application;
        application.set_speciality("singer");
        QCOMPARE(QString("singer"), QString(application.speciality().c_str()));
    }

    void test_application_application_status()
    {
        Application application;
        application.set_application_status("accepted");
        QCOMPARE(QString("accepted"), QString(application.application_status().c_str()));
    }

    void test_application_payment_status()
    {
        Application application;
        application.set_payment_status("accepted");
        QCOMPARE(QString("accepted"), QString(application.payment_status().c_str()));
    }

    void test_application_camp()
    {
        Application application;
        application.set_camp("July");
        QCOMPARE(QString("July"), QString(application.camp().c_str()));
    }

    void test_database_management_result_query()
    {
        Database_management db;
        std::vector<std::vector<std::string>> results;
        std::string query("select first_name from applications where last_name = 'test_entry_last'");
        db.result_from_query(query, results);
        db.close_database();

        results.at(0).at(0);
        QCOMPARE(QString("test_entry_first"), QString(results.at(0).at(0).c_str()));

    }

    void test_database_management_update_query()
    {
        Database_management db;
        std::string query("UPDATE applications SET first_name = 'test_database_first_name' WHERE last_name = 'test_entry_last'");
        db.update_query(query);

        std::vector<std::vector<std::string>> results;
        query = "select first_name from applications where last_name = 'test_entry_last'";
        db.result_from_query(query, results);

        results.at(0).at(0);
        QCOMPARE(QString("test_database_first_name"), QString(results.at(0).at(0).c_str()));

        query = "UPDATE applications SET first_name = 'test_entry_first' WHERE last_name = 'test_entry_last'";
        db.update_query(query);
        db.close_database();

    }

    void test_generate_letters()
    {
        Letter_information li;
        li.sender          = "test_sender";
        li.address_street  = "test_address_street";

        QCOMPARE(QString("test_sender"), li.sender);
        QCOMPARE(QString("test_address_street"), li.address_street);
    }
};

#endif // FURS_TEST_H
