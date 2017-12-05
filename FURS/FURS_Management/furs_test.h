#ifndef FURS_TEST_H
#define FURS_TEST_H

#ifdef QT_TESTS
#include <QtWidgets>
#include <QtTest/QtTest>
#include "application.h"
#include "database_management.h"
#include "generate_letter.h"
#include "furs_main_window.h"
#include "ui_furs_main_window.h"

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

    void test_letter_path()
    {
        Letter_information li;
        li.receiver_last_name   = "test_last_name";
        li.receiver_first_name  = "test_first_name";
        li.document_title       = "test_letter";

        QString letter_path = QCoreApplication::applicationDirPath() + "/../../letters/" +
                "test_last_name" + "_" +
                "test_first_name" + "_" +
                "test_letter" + ".pdf";

        QCOMPARE(letter_path, li.letter_path());
    }

    void test_application_launch_page()
    {
        // On creation of app launch page should be opened
        FURS_main_window mw;
        QCOMPARE(0, mw.ui->furs_stacked_control->currentIndex());
    }

    void test_welcome_page_start_button()
    {
        // On clicking start button actions page should open.
        FURS_main_window mw;
        mw.ui->start_button->pressed();
        QCOMPARE(1, mw.ui->furs_stacked_control->currentIndex());
    }

    void test_action_page_registration_button()
    {
        // On clicking registration button registration page should open.
        FURS_main_window mw;
        mw.ui->registration_button->pressed();
        QCOMPARE(2, mw.ui->furs_stacked_control->currentIndex());
    }

    void test_action_page_checkin_button()
    {
        // On clicking checkin button checkin page should open.
        FURS_main_window mw;
        mw.ui->checkin_button->pressed();
        QCOMPARE(3, mw.ui->furs_stacked_control->currentIndex());
    }

    void test_action_page_dorms_button()
    {
        // On clicking dorms button dorms page should open.
        FURS_main_window mw;
        mw.ui->dorms_button->pressed();
        QCOMPARE(4, mw.ui->furs_stacked_control->currentIndex());
    }

    void test_action_page_bands_button()
    {
        // On clicking bands button bands page should open.
        FURS_main_window mw;
        mw.ui->bands_button->pressed();
        QCOMPARE(5, mw.ui->furs_stacked_control->currentIndex());
    }

    void test_action_page_secondary_bands_button()
    {
        // On clicking secondary bands button secondary bands page should open.
        FURS_main_window mw;
        mw.ui->bands_secondary_button->pressed();
        QCOMPARE(6, mw.ui->furs_stacked_control->currentIndex());
    }

    void test_registration_page_last_name_line_edit()
    {
        FURS_main_window mw;
        mw.ui->line_edit_last_name->setText("last_name");
        QCOMPARE(QString("last_name"), mw.ui->line_edit_last_name->text());
    }

    void test_existing_page_last_name_line_edit()
    {
        FURS_main_window mw;
        mw.ui->line_edit_last_name_exist->setText("last_name");
        QCOMPARE(QString("last_name"), mw.ui->line_edit_last_name_exist->text());
    }

    void test_registration_page_first_name_line_edit()
    {
        FURS_main_window mw;
        mw.ui->line_edit_first_name->setText("first_name");
        QCOMPARE(QString("first_name"), mw.ui->line_edit_first_name->text());
    }

    void test_existing_page_first_name_line_edit()
    {
        FURS_main_window mw;
        mw.ui->line_edit_first_name_exist->setText("first_name");
        QCOMPARE(QString("first_name"), mw.ui->line_edit_first_name_exist->text());
    }

    void test_registration_page_street_line_edit()
    {
        FURS_main_window mw;
        mw.ui->line_edit_street->setText("test_street");
        QCOMPARE(QString("test_street"), mw.ui->line_edit_street->text());
    }

    void test_exist_page_street_line_edit()
    {
        FURS_main_window mw;
        mw.ui->line_edit_street_exist->setText("test_street");
        QCOMPARE(QString("test_street"), mw.ui->line_edit_street_exist->text());
    }

    void test_registration_page_city_line_edit()
    {
        FURS_main_window mw;
        mw.ui->line_edit_city->setText("test");
        QCOMPARE(QString("test"), mw.ui->line_edit_city->text());
    }

    void test_registration_page_phone_line_edit()
    {
        FURS_main_window mw;
        mw.ui->line_edit_phone->setText("123456780");
        QCOMPARE(QString("123456780"), mw.ui->line_edit_phone->text());
    }

    void test_registration_page_camp_combo()
    {
        FURS_main_window mw;
        mw.ui->combo_box_camp->setCurrentText(QString("Not selected"));
        QCOMPARE(QString("Not selected"), mw.ui->combo_box_camp->currentText());
    }

    void test_existing_page_camp_combo()
    {
        FURS_main_window mw;
        mw.ui->combo_box_camp_exist->setCurrentText(QString("Not selected"));
        QCOMPARE(QString("Not selected"), mw.ui->combo_box_camp_exist->currentText());
    }

    void test_registration_page_app_status_combo()
    {
        FURS_main_window mw;
        mw.ui->combo_box_app_status->setCurrentText(QString("Accepted"));
        QCOMPARE(QString("Accepted"), mw.ui->combo_box_app_status->currentText());
    }

    void test_existing_page_app_status_combo()
    {
        FURS_main_window mw;
        mw.ui->combo_box_app_status_exist->setCurrentText(QString("Accepted"));
        QCOMPARE(QString("Accepted"), mw.ui->combo_box_app_status_exist->currentText());
    }

    void test_registration_page_cancel_button()
    {
        FURS_main_window mw;
        mw.ui->button_cancel->pressed();
        QCOMPARE(1, mw.ui->furs_stacked_control->currentIndex());
    }

    void test_existing_page_cancel_button()
    {
        FURS_main_window mw;
        mw.ui->button_cancel_existing->pressed();
        QCOMPARE(1, mw.ui->furs_stacked_control->currentIndex());
    }

    void test_checkin_page_cancel_button()
    {
        FURS_main_window mw;
        mw.ui->checkin_button->pressed();
        mw.ui->button_cancel_check_in_applicant->pressed();
        QCOMPARE(1, mw.ui->furs_stacked_control->currentIndex());
    }

    void test_dorms_page_exit_button()
    {
        FURS_main_window mw;
        mw.ui->dorms_button->pressed();
        mw.ui->button_dorms_exit->pressed();
        QCOMPARE(1, mw.ui->furs_stacked_control->currentIndex());
    }

    void test_bands_page_exit_button()
    {
        FURS_main_window mw;
        mw.ui->bands_button->pressed();
        mw.ui->button_bands_exit->pressed();
        QCOMPARE(1, mw.ui->furs_stacked_control->currentIndex());
    }

    void test_secondary_bands_page_exit_button()
    {
        FURS_main_window mw;
        mw.ui->bands_secondary_button->pressed();
        mw.ui->button_secondary_bands_exit->pressed();
        QCOMPARE(1, mw.ui->furs_stacked_control->currentIndex());
    }

};

#endif
#endif // FURS_TEST_H
