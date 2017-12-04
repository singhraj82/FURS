#include "furs_main_window.h"
#include "ui_furs_main_window.h"
#include "constants.h"
#include "application.h"
#include "database_management.h"
#include <QMessageBox>
#include <QDebug>
#include <QUuid>
#include "constants.h"
#include <QTableWidget>
#include "generate_letter.h"
#include <QDateTime>
#include <QFileInfo>
#include <QRadioButton>
#include <map>

FURS_main_window::FURS_main_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FURS_main_window)
{
    // initialize database
    m_db_management = std::make_shared<Database_management>();

    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    // Always open main page when application launches.
    ui->furs_stacked_control->setCurrentIndex(0);

    // Always go to new registration tab when clicked on registration button
    ui->registration_tab_widget->setCurrentIndex(0);

    connect(ui->start_button, SIGNAL(pressed()), this, SLOT(open_action_window()));
    connect(ui->registration_button, SIGNAL(pressed()), this, SLOT(open_applications_window()));

    //ui->checkin_button->setEnabled(false);
    // initialization
    initialize_new_application_tab_();
    initialize_existing_application_tab_();
    initialize_checkin_tab_();
    initialize_dorms_tab_();
    initialize_bands_tab_();
}

FURS_main_window::~FURS_main_window()
{
    delete ui;
}

void FURS_main_window::initialize_new_application_tab_()
{
    ui->combo_box_camp->insertItems(0, camps);
    ui->combo_box_gender->insertItems(0, gender);
    ui->combo_box_state->insertItems(0, states);
    ui->combo_box_app_status->insertItems(0, application_status);
    ui->combo_box_instrument->insertItems(0, speciality);
    ui->combo_box_pmt_status->insertItems(0, payment_status);

    // temporarily hiding payment status
    ui->label_pmt_status->hide();
    ui->combo_box_pmt_status->hide();

    connect(ui->button_save, SIGNAL(pressed()), this, SLOT(add_record()));
    connect(ui->button_cancel, SIGNAL(pressed()), this, SLOT(open_action_window()));
}

void FURS_main_window::initialize_existing_application_tab_()
{
    // temporarily hiding payment status
    ui->label_pmt_status_exist->hide();
    ui->combo_box_pmt_status_exist->hide();
    ui->label_application_id_exist->hide();

    ui->combo_box_camp_exist->insertItems(0, camps);
    ui->combo_box_gender_exist->insertItems(0, gender);
    ui->combo_box_state_exist->insertItems(0, states);
    ui->combo_box_app_status_exist->insertItems(0, application_status);
    ui->combo_box_instrument_exist->insertItems(0, speciality);
    ui->combo_box_pmt_status_exist->insertItems(0, payment_status);
    ui->check_box_waitlist_filter->setChecked(false);

    // Get the latest list of applications from database
    refresh_existing_applications_list_(ui->table_widget_filter);

    connect(ui->registration_tab_widget, SIGNAL(currentChanged(int)), this, SLOT(tab_selected(int)));
    //connect(ui->button_apply_existing, SIGNAL(pressed()), this, SLOT(save_and_open_action_window()));
    connect(ui->button_cancel_existing, SIGNAL(pressed()), this, SLOT(open_action_window()));
    connect(ui->button_apply_existing, SIGNAL(pressed()), this, SLOT(update_existing_record()));
    connect(ui->button_letter_existing, SIGNAL(pressed()), this, SLOT(generate_letter()));

    connect(ui->table_widget_filter, SIGNAL(cellClicked(int, int)), this, SLOT(pull_record_existing(int, int)));
    connect(ui->check_box_waitlist_filter, SIGNAL(stateChanged(int)), this, SLOT(update_existing_record_page(int)));
}

void FURS_main_window::initialize_checkin_tab_()
{
    ui->line_edit_last_name_checkin->setEnabled(false);
    ui->line_edit_first_name_checkin->setEnabled(false);

    connect(ui->checkin_button, SIGNAL(pressed()), this, SLOT(open_checkin_page()));

    // Get the latest list of applications from database
    refresh_existing_applications_list_(ui->table_existing_applicants);
    connect(ui->table_existing_applicants, SIGNAL(cellClicked(int, int)), this, SLOT(pull_record_checkin(int, int)));
}

void FURS_main_window::initialize_dorms_tab_()
{
    connect(ui->dorms_button, SIGNAL(pressed()), this, SLOT(open_dorms_page()));
}

void FURS_main_window::initialize_bands_tab_()
{
    connect(ui->bands_button, SIGNAL(pressed()), this, SLOT(open_bands_page()));
    connect(ui->bands_secondary_button, SIGNAL(pressed()), this, SLOT(open_secondary_bands_page()));
}

// Actions stack window
void FURS_main_window::open_action_window()
{ 
    ui->furs_stacked_control->setCurrentIndex(1);

    // Always go to new registration tab when clicked on registration button
    ui->registration_tab_widget->setCurrentIndex(0);
}

// Applications stack window
void FURS_main_window::open_applications_window()
{
    ui->furs_stacked_control->setCurrentIndex(2);
}

void FURS_main_window::add_record()
{
    if (new_form_has_empty_fields())
    {
        QMessageBox::warning( this,  tr("FURS"),  tr("Cannot save data. SOME OF THE REQUIRED(*) FIELDS ARE EMPTY !!!") );
        return;
    }

    // Generate a GUID for the user id
    QUuid guid =QUuid::createUuid();

    std::string query("INSERT  INTO  " + c_table_applications + "(");
    query += c_table_field_id + ",";
    query += c_table_field_last_name + ",";
    query += c_table_field_first_name + ",";
    query += c_table_field_age + ",";
    query += c_table_field_gender + ",";
    query += c_table_field_phone + ",";
    query += c_table_field_street_address + ",";
    query += c_table_field_city + ",";
    query += c_table_field_state + ",";
    query += c_table_field_zip_code + ",";
    query += c_table_field_speciality + ",";
    query += c_table_field_app_status + ",";
    query += c_table_field_payment_status + ",";
    query += c_table_field_pmt_type + ",";
    query += c_table_field_camp + ")";

    query += " VALUES(";
    query += "'" + guid.toString().toStdString() + "',";
    query += "'" + ui->line_edit_last_name->text().toStdString() + "',";
    query += "'" + ui->line_edit_first_name->text().toStdString() + "',";
    query += "'" + ui->line_edit_age->text().toStdString() + "',";
    query += "'" + ui->combo_box_gender->currentText().toStdString() + "',";
    query += "'" + ui->line_edit_phone->text().toStdString() + "',";
    query += "'" + ui->line_edit_street->text().toStdString() + "',";
    query += "'" + ui->line_edit_city->text().toStdString() + "',";
    query += "'" + ui->combo_box_state->currentText().toStdString() + "',";
    query += "'" + ui->line_edit_zipcode->text().toStdString() + "',";
    query += "'" + ui->combo_box_instrument->currentText().toStdString() + "',";
    query += "'" + ui->combo_box_app_status->currentText().toStdString() + "',";
    query += "'" + ui->combo_box_pmt_status->currentText().toStdString() + "',";
    query += "'" + payment_type_().toStdString() + "',";
    query += "'" + ui->combo_box_camp->currentText().toStdString() + "')";

    if (m_db_management->update_query(query))
    {
        // After saving the information reset the form to default values
        clear_new_application_form_();

        QMessageBox msg_box;
        msg_box.setWindowTitle("FURS");
        msg_box.setText("Application saved successfully");
        msg_box.exec();
    }
    else
    {
        QMessageBox msg_box;
        msg_box.setWindowTitle("FURS");
        msg_box.setText("Application Failed to Save");
        msg_box.exec();
    }
}

void FURS_main_window::pull_record_existing(int row, int col)
{
    Q_UNUSED(col);
    auto name = ui->table_widget_filter->item(row, 0)->text().toStdString();
    auto id = ui->table_widget_filter->item(row, 1)->text().toStdString();

    std::string sql_query("select * from applications where last_name = '" + name + "' AND " + "id = '" + id + "'");
    std::vector<std::vector<std::string>> results;
    auto success = m_db_management->result_from_query(sql_query, results);
    if (success)
    {
        update_existing_form(results[0]);
    }
    else
    {
        qDebug() << "Failed to fetch data";
    }
}

void FURS_main_window::update_existing_form(const std::vector<std::string>& val)
{
    ui->label_application_id_exist->setText(QString(val[0].c_str()));
    ui->line_edit_age_exist->setText(QString(val[3].c_str()));
    ui->line_edit_first_name_exist->setText(QString(val[2].c_str()));
    ui->line_edit_last_name_exist->setText(QString(val[1].c_str()));
    ui->combo_box_gender_exist->setCurrentText(QString(val[4].c_str()));
    ui->combo_box_instrument_exist->setCurrentText(QString(val[10].c_str()));
    ui->line_edit_street_exist->setText(QString(val[6].c_str()));
    ui->combo_box_state_exist->setCurrentText(QString(val[8].c_str()));
    ui->line_edit_city_exist->setText(QString(val[7].c_str()));
    ui->line_edit_zipcode_exist->setText(QString(val[9].c_str()));
    ui->line_edit_phone_exist->setText(QString(val[5].c_str()));
    ui->combo_box_app_status_exist->setCurrentText(QString(val[11].c_str()));
    ui->combo_box_pmt_status_exist->setCurrentText(QString(val[12].c_str()));
    ui->combo_box_camp_exist->setCurrentText(QString(val[13].c_str()));
    set_payment_type_radio_(QString(val[14].c_str()));
    set_waitlist_check_box_(QString(val[22].c_str()));
    ui->line_edit_rank_exist->setText(QString(val[20].c_str()));
}

void FURS_main_window::pull_record_checkin(int row, int col)
{
    Q_UNUSED(col);
    auto name = ui->table_existing_applicants->item(row, 0)->text().toStdString();
    auto id = ui->table_existing_applicants->item(row, 1)->text().toStdString();

    std::string sql_query("select * from applications where last_name = '" + name + "' AND " + "id = '" + id + "'");
    std::vector<std::vector<std::string>> results;
    auto success = m_db_management->result_from_query(sql_query, results);
    if (success)
    {
        update_checkin_form(results[0]);
    }
    else
    {
        qDebug() << "Failed to fetch data";
    }
}

void FURS_main_window::update_checkin_form(const std::vector<std::string>& val)
{
    ui->label_registration_id_value->setText(QString(val[0].c_str()));
    ui->line_edit_last_name_checkin->setText(QString(val[1].c_str()));
    ui->line_edit_first_name_checkin->setText(QString(val[2].c_str()));
    ui->label_checkin_status_value->setText(QString(val[15].c_str()));
    if (val[15] == "Checked_in")
    {
        ui->button_check_in_applicant->setEnabled(false);
    }
    else
    {
        ui->button_check_in_applicant->setEnabled(true);
    }

    if (val[16] == "no")
    {
        ui->check_box_equipment->setChecked(false);
        ui->check_box_equipment->setEnabled(true);
    }
    else
    {
        ui->check_box_equipment->setChecked(true);
        ui->check_box_equipment->setEnabled(false);
    }

    if (val[17] == "no")
    {
        ui->check_box_clothes->setChecked(false);
        ui->check_box_clothes->setEnabled(true);
    }
    else
    {
        ui->check_box_clothes->setChecked(true);
        ui->check_box_clothes->setEnabled(false);
    }

    if (val[18] == "no")
    {
        ui->check_box_forms->setChecked(false);
        ui->check_box_forms->setEnabled(true);
    }
    else
    {
        ui->check_box_forms->setChecked(true);
        ui->check_box_forms->setEnabled(false);
    }

}

void FURS_main_window::clear_new_application_form_()
{
    ui->line_edit_first_name->setText(QString());
    ui->line_edit_last_name->setText(QString());
    ui->combo_box_gender->setCurrentText(QString(gender.at(0)));
    ui->combo_box_instrument->setCurrentText(QString(speciality.at(0)));
    ui->line_edit_street->setText(QString());
    ui->combo_box_state->setCurrentText(QString(states.at(0)));
    ui->line_edit_city->setText(QString());
    ui->line_edit_zipcode->setText(QString());
    ui->line_edit_phone->setText(QString());
    ui->combo_box_app_status->setCurrentText(QString(application_status.at(0)));
    ui->combo_box_pmt_status->setCurrentText(QString(payment_status.at(0)));
    ui->combo_box_camp->setCurrentText(QString(camps.at(0)));
    ui->line_edit_age->setText(QString());
}

void FURS_main_window::tab_selected(int tab_index)
{
    // On clicking the existing tab refresh the list
    if(tab_index == 1)
    {
        refresh_existing_applications_list_(ui->table_widget_filter);
    }
}

void FURS_main_window::refresh_existing_applications_list_(QTableWidget *table_widget_filter)
{
    // Set the information for table
    table_widget_filter->clear();
    QStringList labels;
    labels << tr("LAST_NAME") << tr("APPLICATION_ID");
    table_widget_filter->setHorizontalHeaderLabels(labels);

    std::string query = "select " + c_table_field_last_name + "," + c_table_field_id + " from applications";
    if (ui->check_box_waitlist_filter->isChecked())
    {
        query += " WHERE " + c_table_field_waitlist + " = 'yes'";
    }

    std::vector<std::vector<std::string>> results;
    auto success = m_db_management->result_from_query(query, results);
    if (success)
    {
        table_widget_filter->setRowCount((results.size()));
        for (size_t i = 0; i < results.size(); ++i)
        {
            auto values = results[i];
            table_widget_filter->setItem(i, 0, new QTableWidgetItem(QString(values[0].c_str())));
            table_widget_filter->setItem(i, 1, new QTableWidgetItem(QString(values[1].c_str())));
        }
    }
    else
    {
        qDebug() << "Failed to fetch data";
    }


}

void FURS_main_window::update_existing_record()
{
    if (existing_form_has_empty_fields())
    {
        QMessageBox::warning( this,  tr("FURS"),  tr("Cannot save data. SOME OF THE REQUIRED(*) FIELDS ARE EMPTY !!!") );
        return;
    }

    std::string query("UPDATE  " + c_table_applications + " SET ");
    query += c_table_field_last_name + "='" + ui->line_edit_last_name_exist->text().toStdString() + "',";
    query += c_table_field_first_name + "='" + ui->line_edit_first_name_exist->text().toStdString() + "',";
    query += c_table_field_age + "='" + ui->line_edit_age_exist->text().toStdString() + "',";
    query += c_table_field_gender + "='" + ui->combo_box_gender_exist->currentText().toStdString() + "',";
    query += c_table_field_phone + "='" + ui->line_edit_phone_exist->text().toStdString() + "',";
    query += c_table_field_street_address + "='" + ui->line_edit_street_exist->text().toStdString() + "',";
    query += c_table_field_city + "='" + ui->line_edit_city_exist->text().toStdString() + "',";
    query += c_table_field_state + "='" + ui->combo_box_state_exist->currentText().toStdString() + "',";
    query += c_table_field_zip_code + "='" + ui->line_edit_zipcode_exist->text().toStdString() + "',";
    query += c_table_field_speciality + "='" + ui->combo_box_instrument_exist->currentText().toStdString() + "',";
    query += c_table_field_app_status + "='" + ui->combo_box_app_status_exist->currentText().toStdString() + "',";
    query += c_table_field_payment_status + "='" + ui->combo_box_pmt_status_exist->currentText().toStdString() + "',";
    query += c_table_field_camp + "='"+ ui->combo_box_camp_exist->currentText().toStdString()+ "', ";
    query += c_table_field_pmt_type + "='"+ payment_type_().toStdString()+ "',  ";
    query += c_table_field_waitlist + "='"+ waitlist_filter_result_().toStdString()+ "',  ";
    query += c_table_field_speciality_rank + "='" + ui->line_edit_rank_exist->text().toStdString() + "' ";
    query += " WHERE " + c_table_field_last_name + "='" + ui->line_edit_last_name_exist->text().toStdString() + "' AND " + "id = '" + ui->label_application_id_exist->text().toStdString() + "'";

    if (m_db_management->update_query(query))
    {
        QMessageBox msg_box;
        msg_box.setWindowTitle("FURS");
        msg_box.setText("Application updated successfully");
        msg_box.exec();
    }
    else
    {
        QMessageBox::warning( this,  tr("FURS"),  tr("FAILED TO SAVE DATA !!!") );
    }
}

void FURS_main_window::generate_letter()
{
    if (ui->combo_box_app_status_exist->currentText() == c_not_selected || ui->combo_box_app_status_exist->currentText() == c_in_progress)
    {
        QMessageBox::warning( this,  "FURS",  "FAILED TO GENERATE LETTER. INVALID APPLICATION STATUS !!!" );
        return;
    }

    if (existing_form_has_empty_fields())
    {
        QMessageBox::warning( this,  tr("FURS"),  tr("SOME OF THE REQUIRED(*) FIELDS ARE EMPTY !!!") );
        return;
    }

    Letter_information letter_info;
    letter_info.sender                    = "Future Rock Stars(FuRS)";
    letter_info.address_street            = "123 FURS city";
    letter_info.address_city              = "East of LA";
    letter_info.address_zipcode           = "12345";
    letter_info.address_state             = "California";
    letter_info.document_title            = ui->combo_box_app_status_exist->currentText();
    letter_info.clerk_name                = "CLERK";
    letter_info.date                      = QString(QDateTime::currentDateTime().toString());
    letter_info.role                      = ui->combo_box_instrument_exist->currentText();
    letter_info.camp                      = ui->combo_box_camp_exist->currentText();
    letter_info.receiver_last_name        = ui->line_edit_last_name_exist->text();
    letter_info.receiver_first_name       = ui->line_edit_first_name_exist->text();
    letter_info.receiver_address_street   = ui->line_edit_street_exist->text();
    letter_info.receiver_address_city     = ui->line_edit_city_exist->text();
    letter_info.receiver_address_state    = ui->combo_box_state_exist->currentText();
    letter_info.receiver_address_zip_code = ui->line_edit_zipcode_exist->text();
    letter_info.wait_list                 = waitlist_filter_result_();


    Generate_letter letter;
    letter.print_letter(letter_info);

    QFileInfo check_file(letter_info.letter_path());
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile())
    {
        QMessageBox::information( this,  "FURS",  "LETTER GENERATED SUCCESSFULLY." );
    }
    else
    {
        QMessageBox::warning( this,  "FURS",  "FAILED TO GENERATE LETTER !!!" );
    }
}

bool FURS_main_window::existing_form_has_empty_fields()
{
    if (ui->line_edit_last_name_exist->text().toStdString().empty()           ||
        ui->line_edit_first_name_exist->text().toStdString().empty()          ||
        ui->combo_box_gender_exist->currentText().toStdString().empty()       ||
        ui->line_edit_phone_exist->text().toStdString().empty()               ||
        ui->line_edit_street_exist->text().toStdString().empty()              ||
        ui->line_edit_city_exist->text().toStdString().empty()                ||
        ui->combo_box_state_exist->currentText().toStdString().empty()        ||
        ui->line_edit_zipcode_exist->text().toStdString().empty()             ||
        ui->combo_box_instrument_exist->currentText().toStdString().empty()   ||
        ui->combo_box_app_status_exist->currentText().toStdString().empty()   ||
        ui->combo_box_pmt_status_exist->currentText().toStdString().empty()   ||
        ui->line_edit_age_exist->text().toStdString().empty()                 ||
        ui->combo_box_camp_exist->currentText().toStdString().empty())
    {
        return true;
    }

    return false;
}

bool FURS_main_window::new_form_has_empty_fields()
{
    if (ui->line_edit_last_name->text().toStdString().empty()           ||
        ui->line_edit_first_name->text().toStdString().empty()          ||
        ui->combo_box_gender->currentText().toStdString().empty()       ||
        ui->line_edit_phone->text().toStdString().empty()               ||
        ui->line_edit_street->text().toStdString().empty()              ||
        ui->line_edit_city->text().toStdString().empty()                ||
        ui->combo_box_state->currentText().toStdString().empty()        ||
        ui->line_edit_zipcode->text().toStdString().empty()             ||
        ui->combo_box_instrument->currentText().toStdString().empty()   ||
        ui->combo_box_app_status->currentText().toStdString().empty()   ||
        ui->combo_box_pmt_status->currentText().toStdString().empty()   ||
        ui->line_edit_age->text().toStdString().empty()                ||
        ui->combo_box_camp->currentText().toStdString().empty())
    {
        return true;
    }

    return false;
}

QString FURS_main_window::payment_type_()
{
    if(ui->radio_button_card->isChecked() || ui->radio_button_card_exist->isChecked())
    {
        return c_pmt_card;
    }
    else if(ui->radio_button_check->isChecked() || ui->radio_button_check_exist->isChecked())
    {
        return c_pmt_check;
    }
    else
    {
        return c_pmt_cashier_check;
    }
}

QString FURS_main_window::waitlist_filter_result_()
{
    if(ui->check_box_waitlist->isChecked())
    {
        return "yes";
    }
    else if(!ui->check_box_waitlist->isChecked())
    {
        return "no";
    }
    else
    {
        return "no";
    }
}

void FURS_main_window::set_payment_type_radio_(QString payment_type)
{
    if (payment_type == c_pmt_card)
    {
        ui->radio_button_card_exist->setChecked(true);
    }
    else if (payment_type == c_pmt_check)
    {
        ui->radio_button_check_exist->setChecked(true);
    }
    else
    {
        ui->radio_button_cash_check_exist->setChecked(true);
    }
}

void FURS_main_window::set_waitlist_check_box_(QString waitlist)
{
    if (waitlist == "no")
    {
        ui->check_box_waitlist->setChecked(false);
    }
    else if (waitlist == "yes")
    {
        ui->check_box_waitlist->setChecked(true);
    }
    else
    {
        ui->check_box_waitlist->setChecked(false);
    }
}

void FURS_main_window::open_checkin_page()
{
    ui->furs_stacked_control->setCurrentIndex(3);

    connect(ui->button_cancel_check_in_applicant, SIGNAL(pressed()), this, SLOT(open_action_window()));
    connect(ui->button_check_in_applicant, SIGNAL(pressed()), this, SLOT(checkin_applicant()));
}

void FURS_main_window::checkin_applicant()
{
    if (!ui->check_box_equipment->isChecked() || !ui->check_box_clothes->isChecked() || !ui->check_box_forms->isChecked())
    {
        QMessageBox::warning( this,  tr("FURS"),  tr("Some required items are missing!!!") );
    }
    else
    {
        std::string query("UPDATE  " + c_table_applications + " SET ");
        query += c_table_field_check_in_status + "='Checked_in',";
        query += c_table_field_equipments + "='yes',";
        query += c_table_field_clothes + "='yes',";
        query += c_table_field_forms+ "='yes'";
        query += " WHERE " + c_table_field_last_name + "='" + ui->line_edit_last_name_checkin->text().toStdString() + "' AND " + "id = '" + ui->label_registration_id_value->text().toStdString() + "'";

        if (m_db_management->update_query(query))
        {
            // once checked in successfukky update the status and grey out the check boxes
            ui->label_checkin_status_value->setText("Checked_in");
            ui->button_check_in_applicant->setEnabled((false));

            ui->check_box_equipment->setEnabled(false);
            ui->check_box_clothes->setEnabled(false);
            ui->check_box_forms->setEnabled(false);

            QMessageBox msg_box;
            msg_box.setWindowTitle("FURS");
            msg_box.setText("Application updated successfully");
            msg_box.exec();
        }
        else
        {
            QMessageBox::warning( this,  tr("FURS"),  tr("FAILED TO SAVE DATA !!!") );
        }
    }
}

void FURS_main_window::open_dorms_page()
{
    connect(ui->button_dorms_exit, SIGNAL(pressed()), this, SLOT(open_action_window()));
    ui->furs_stacked_control->setCurrentIndex(4);

    //Assign the dorms
    assign_dorms_();
}

void FURS_main_window::assign_dorms_()
{
    // Reset Dorms
    /*ui->girls_dorm_1->clear();
    ui->girls_dorm_2->clear();
    ui->girls_dorm_3->clear();

    ui->boys_dorm_1->clear();
    ui->boys_dorm_2->clear();
    ui->boys_dorm_3->clear();

    //Set girls dorms: Get all the girls and sort by age
    std::string sql_query("select id, last_name, first_name, gender, age from applications where gender = 'Female' and app_status = 'Accepted'");
    std::vector<std::vector<std::string>> results;
    auto success = m_db_management->result_from_query(sql_query, results);

    if (success)
    {
        std::multimap<int, std::vector<std::string>> girls_data;
        for (auto res : results)
        {
            girls_data.insert(std::make_pair<int, std::vector<std::string>>(std::stoi(res[4]), {res[0], res[1], res[2],res[3]}));
        }

        auto size = girls_data.size();
        for (auto i = 0; i < size; i+=6)
        {
           ui->girls_dorm_1->addItem(QString(girls_data.begin()->second[1].c_str()) + "_" + QString(girls_data.begin()->second[0].c_str()));
           girls_data.erase(girls_data.begin());

           ui->girls_dorm_1->addItem(QString(girls_data.rbegin()->second[1].c_str()) + "_" + QString(girls_data.begin()->second[0].c_str()));
           girls_data.erase(--girls_data.end());

           ui->girls_dorm_2->addItem(QString(girls_data.begin()->second[1].c_str())+ "_" + QString(girls_data.begin()->second[0].c_str()));
           girls_data.erase(girls_data.begin());

           ui->girls_dorm_2->addItem(QString(girls_data.rbegin()->second[1].c_str())+ "_" + QString(girls_data.begin()->second[0].c_str()));
           girls_data.erase(--girls_data.end());

           ui->girls_dorm_3->addItem(QString(girls_data.begin()->second[1].c_str())+ "_" + QString(girls_data.begin()->second[0].c_str()));
           girls_data.erase(girls_data.begin());

           ui->girls_dorm_3->addItem(QString(girls_data.rbegin()->second[1].c_str())+ "_" + QString(girls_data.begin()->second[0].c_str()));
           girls_data.erase(--girls_data.end());
        }
    }
    else
    {
        qDebug() << "Failed to fetch data";
    }


    //Set boys dorms: Get all the girls and sort by age
    sql_query = "select id, last_name, first_name, gender, age from applications where gender = 'Male' and app_status = 'Accepted'";
    results.clear();
    success = m_db_management->result_from_query(sql_query, results);

    if (success)
    {
        std::multimap<int, std::vector<std::string>> boys_data;
        for (auto res : results)
        {
            boys_data.insert(std::make_pair<int, std::vector<std::string>>(std::stoi(res[4]), {res[0], res[1], res[2],res[3]}));
        }

        auto size = boys_data.size();
        for (auto i = 0; i < size; i+=6)
        {
           ui->boys_dorm_1->addItem(QString(boys_data.begin()->second[1].c_str())+ "_" + QString(boys_data.begin()->second[0].c_str()));
           boys_data.erase(boys_data.begin());

           ui->boys_dorm_1->addItem(QString(boys_data.rbegin()->second[1].c_str())+ QString(boys_data.begin()->second[0].c_str()));
           boys_data.erase(--boys_data.end());

           ui->boys_dorm_2->addItem(QString(boys_data.begin()->second[1].c_str())+ QString(boys_data.begin()->second[0].c_str()));
           boys_data.erase(boys_data.begin());

           ui->boys_dorm_2->addItem(QString(boys_data.rbegin()->second[1].c_str())+ QString(boys_data.begin()->second[0].c_str()));
           boys_data.erase(--boys_data.end());

           ui->boys_dorm_3->addItem(QString(boys_data.begin()->second[1].c_str())+ QString(boys_data.begin()->second[0].c_str()));
           boys_data.erase(boys_data.begin());

           ui->boys_dorm_3->addItem(QString(boys_data.rbegin()->second[1].c_str())+ QString(boys_data.begin()->second[0].c_str()));
           boys_data.erase(--boys_data.end());
        }
    }
    else
    {
        qDebug() << "Failed to fetch data";
    }*/

    ui->table_dorms->clear();

    QStringList labels;
    labels << tr("DORM_ID") << tr("LAST_NAME") << tr("FIRST_NAME") << tr("AGE") << tr("APPLICATION_ID");
    ui->table_dorms->setHorizontalHeaderLabels(labels);

    std::vector<int> girls_dorms = {1,2,3};
    std::vector<int> boys_dorms  = {4,5,6};

    // Girls Dorms
    std::vector<std::vector<std::string>> results;
    std::string sql_query("select last_name, first_name, id, age from applications where gender = 'Female' and app_status = 'Accepted'");
    auto success = m_db_management->result_from_query(sql_query, results);
    int j = 0;
    if (success)
    {
        ui->table_dorms->setRowCount(results.size() * 2);

        std::vector<std::pair<int, std::vector<std::string>>> girls_data;
        for (auto res : results)
        {
            girls_data.emplace_back(std::make_pair<int, std::vector<std::string>>(std::stoi(res[3]), {res[0], res[1], res[2]}));
        }

        std::sort(girls_data.begin(), girls_data.end());

        do
        {
            for (int i = 0; i < girls_dorms.size(); ++i)
            {
                auto val_begin = girls_data.begin();
                ui->table_dorms->setItem(j, 0, new QTableWidgetItem(QString(std::to_string(girls_dorms.at(i)).c_str())));
                ui->table_dorms->setItem(j, 1, new QTableWidgetItem(QString(val_begin->second[0].c_str())));
                ui->table_dorms->setItem(j, 2, new QTableWidgetItem(QString(val_begin->second[1].c_str())));
                ui->table_dorms->setItem(j, 3, new QTableWidgetItem(QString(std::to_string(val_begin->first).c_str())));
                ui->table_dorms->setItem(j, 4, new QTableWidgetItem(QString(val_begin->second[2].c_str())));
                girls_data.erase(val_begin);
                ++j;

                auto val_end = girls_data.rbegin();
                ui->table_dorms->setItem(j, 0, new QTableWidgetItem(QString(std::to_string(girls_dorms.at(i)).c_str())));
                ui->table_dorms->setItem(j, 1, new QTableWidgetItem(QString(val_end->second[0].c_str())));
                ui->table_dorms->setItem(j, 2, new QTableWidgetItem(QString(val_end->second[1].c_str())));
                ui->table_dorms->setItem(j, 3, new QTableWidgetItem(QString(std::to_string(val_end->first).c_str())));
                ui->table_dorms->setItem(j, 4, new QTableWidgetItem(QString(val_end->second[2].c_str())));
                girls_data.erase(--girls_data.end());
                ++j;
            }


        }
        while(girls_data.size() > 0);
    }
    else
    {
        qDebug() << "Failed to fetch data";
    }

    // Boys Dorms
    results.clear();
    std::string sql_query1("select last_name, first_name, id, age from applications where gender = 'Male' and app_status = 'Accepted'");
    success = m_db_management->result_from_query(sql_query1, results);
    if (success)
    {
        std::vector<std::pair<int, std::vector<std::string>>> boys_data;
        for (auto res : results)
        {
            boys_data.emplace_back(std::make_pair<int, std::vector<std::string>>(std::stoi(res[3]), {res[0], res[1], res[2]}));
        }

        std::sort(boys_data.begin(), boys_data.end());

        do
        {
            for (int i = 0; i < boys_dorms.size(); ++i)
            {
                auto val_begin = boys_data.begin();
                ui->table_dorms->setItem(j, 0, new QTableWidgetItem(QString(std::to_string(boys_dorms.at(i)).c_str())));
                ui->table_dorms->setItem(j, 1, new QTableWidgetItem(QString(val_begin->second[0].c_str())));
                ui->table_dorms->setItem(j, 2, new QTableWidgetItem(QString(val_begin->second[1].c_str())));
                ui->table_dorms->setItem(j, 3, new QTableWidgetItem(QString(std::to_string(val_begin->first).c_str())));
                ui->table_dorms->setItem(j, 4, new QTableWidgetItem(QString(val_begin->second[2].c_str())));
                boys_data.erase(val_begin);
                ++j;

                auto val_end = boys_data.rbegin();
                ui->table_dorms->setItem(j, 0, new QTableWidgetItem(QString(std::to_string(boys_dorms.at(i)).c_str())));
                ui->table_dorms->setItem(j, 1, new QTableWidgetItem(QString(val_end->second[0].c_str())));
                ui->table_dorms->setItem(j, 2, new QTableWidgetItem(QString(val_end->second[1].c_str())));
                ui->table_dorms->setItem(j, 3, new QTableWidgetItem(QString(std::to_string(val_end->first).c_str())));
                ui->table_dorms->setItem(j, 4, new QTableWidgetItem(QString(val_end->second[2].c_str())));
                boys_data.erase(--boys_data.end());
                ++j;
            }


        }
        while(boys_data.size() > 0);
    }
    else
    {
        qDebug() << "Failed to fetch data";
    }

    ui->table_dorms->sortByColumn(0, Qt::SortOrder::AscendingOrder);
}

void FURS_main_window::open_bands_page()
{
    ui->furs_stacked_control->setCurrentIndex(5);
    connect(ui->button_bands_exit, SIGNAL(pressed()), this, SLOT(open_action_window()));
    connect(ui->button_bands_reset, SIGNAL(pressed()), this, SLOT(reset_bands()));

    // Asign the bands
    assign_bands_();
}

void FURS_main_window::open_secondary_bands_page()
{
    ui->furs_stacked_control->setCurrentIndex(6);
    connect(ui->button_secondary_bands_exit, SIGNAL(pressed()), this, SLOT(open_action_window()));
    connect(ui->button_secondary_bands_reset, SIGNAL(pressed()), this, SLOT(reset_secondary_bands()));

    // Asign the bands
    assign_same_gender_bands_();
}

void FURS_main_window::assign_bands_()
{
    ui->table_bands->clear();
    ui->table_bands->setRowCount(0);
    QStringList labels;
    labels << tr("BAND") << tr("SPECIALITY") << tr("RANK") << tr("LAST_NAME") << tr("FIRST_NAME") << tr("GENDER") << tr("APPLICATION_ID");
    ui->table_bands->setHorizontalHeaderLabels(labels);

    ui->table_bands->setRowCount(48);

    std::vector<std::map<int, std::vector<std::string>>> vec_boys;
    std::vector<std::map<int, std::vector<std::string>>> vec_girls;

    // Get all the information for girls-----------------------------------------------------------------------------------------------------------------------------------------------
    // singer
    std::vector<std::vector<std::string>> results_girls;
    std::string sql_query_girls("select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Singer' and gender = 'Female' and app_status = 'Accepted' order by speciality, speciality_rank asc");
    auto success = m_db_management->result_from_query(sql_query_girls, results_girls);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_girls)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_girls.push_back(local_map);
    }

    // Guitarist
    success = false;
    results_girls.clear();
    sql_query_girls = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Guitarist' and gender = 'Female' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_girls, results_girls);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_girls)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_girls.push_back(local_map);
    }

    // Bassist
    success = false;
    results_girls.clear();
    sql_query_girls = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Bassist' and gender = 'Female' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_girls, results_girls);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_girls)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_girls.push_back(local_map);
    }

    // Drummer
    success = false;
    results_girls.clear();
    sql_query_girls = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Drummer' and gender = 'Female' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_girls, results_girls);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_girls)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_girls.push_back(local_map);
    }

    // Keyboardist
    success = false;
    results_girls.clear();
    sql_query_girls = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Keyboardist' and gender = 'Female' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_girls, results_girls);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_girls)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_girls.push_back(local_map);
    }

    // Instrumentalist
    success = false;
    results_girls.clear();
    sql_query_girls = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Instrumentalist' and gender = 'Female' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_girls, results_girls);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_girls)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_girls.push_back(local_map);
    }

    //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Get all the information for boys
    // singer
    success = false;
    std::vector<std::vector<std::string>> results_boys;
    std::string sql_query_boys("select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Singer' and gender = 'Male' and app_status = 'Accepted' order by speciality, speciality_rank asc");
    success = m_db_management->result_from_query(sql_query_boys, results_boys);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_boys)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_boys.push_back(local_map);
    }

    // Guitarist
    success = false;
    results_boys.clear();
    sql_query_boys = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Guitarist' and gender = 'Male' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_boys, results_boys);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_boys)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_boys.push_back(local_map);
    }

    // Bassist
    success = false;
    results_boys.clear();
    sql_query_boys = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Bassist' and gender = 'Male' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_boys, results_boys);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_boys)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_boys.push_back(local_map);
    }

    // Drummer
    success = false;
    results_boys.clear();
    sql_query_boys = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Drummer' and gender = 'Male' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_boys, results_boys);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_boys)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_boys.push_back(local_map);
    }

    // Keyboardist
    success = false;
    results_boys.clear();
    sql_query_boys = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Keyboardist' and gender = 'Male' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_boys, results_boys);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_boys)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_boys.push_back(local_map);
    }

    // Instrumentalist
    success = false;
    results_boys.clear();
    sql_query_boys = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Instrumentalist' and gender = 'Male' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_boys, results_boys);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_boys)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_boys.push_back(local_map);
    }

    // Set the rows for the tables -------------------------------------------------------------------------------------------------------------------------------------------
    int i = 0;
    // 114432
    {
        auto vec_boy  = vec_boys;
        auto vec_girl = vec_girls;

        //1
        auto pos = rand() % vec_boy.size();
        auto val1 = vec_boy[pos];
        auto val2 = vec_girl[pos];
        auto band_1 = val1[1];
        auto band_2 = val2[1];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_2[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_2[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_2[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_2[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_2[4].c_str())));
        ++i;

        //1
        pos = rand() % vec_girl.size();
        val1 = vec_girl[pos];
        val2 = vec_boy[pos];
        band_1 = val1[1];
        band_2 = val2[1];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_2[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_2[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_2[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_2[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_2[4].c_str())));
        ++i;

        //4
        pos = rand() % vec_boy.size();
        val1 = vec_boy[pos];
        val2 = vec_girl[pos];
        band_1 = val1[4];
        band_2 = val2[4];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_2[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_2[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_2[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_2[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_2[4].c_str())));
        ++i;

        //4
        pos = rand() % vec_girl.size();
        val1 = vec_girl[pos];
        val2 = vec_boy[pos];
        band_1 = val1[4];
        band_2 = val2[4];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_2[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_2[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_2[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_2[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_2[4].c_str())));
        ++i;

        //3
        pos = rand() % vec_boy.size();
        val1 = vec_boy[pos];
        val2 = vec_girl[pos];
        band_1 = val1[3];
        band_2 = val2[3];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_2[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_2[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_2[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_2[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_2[4].c_str())));
        ++i;

        //2
        pos = rand() % vec_girl.size();
        val1 = vec_girl[pos];
        val2 = vec_boy[pos];
        band_1 = val1[2];
        band_2 = val2[2];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_2[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_2[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_2[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_2[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_2[4].c_str())));
        ++i;
    }

    // 223314
    {
        auto vec_boy = vec_boys;
        auto vec_girl = vec_girls;

        //2
        auto pos = rand() % vec_boy.size();
        auto val1 = vec_boy[pos];
        auto val2 = vec_girl[pos];
        auto band_3 = val1[2];
        auto band_4 = val2[2];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_3[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_3[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_3[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_3[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_3[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_4[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_4[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_4[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_4[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_4[4].c_str())));
        ++i;

        //2
        pos = rand() % vec_girl.size();
        val1 = vec_girl[pos];
        val2 = vec_boy[pos];
        band_3 = val1[2];
        band_4 = val2[2];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_3[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_3[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_3[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_3[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_3[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_4[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_4[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_4[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_4[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_4[4].c_str())));
        ++i;

        //3
        pos = rand() % vec_boy.size();
        val1 = vec_boy[pos];
        val2 = vec_girl[pos];
        band_3 = val1[3];
        band_4 = val2[3];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_3[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_3[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_3[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_3[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_3[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_4[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_4[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_4[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_4[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_4[4].c_str())));
        ++i;

        //3
        pos = rand() % vec_girl.size();
        val1 = vec_girl[pos];
        val2 = vec_boy[pos];
        band_3 = val1[3];
        band_4 = val2[3];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_3[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_3[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_3[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_3[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_3[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_4[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_4[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_4[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_4[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_4[4].c_str())));
        ++i;

        //1
        pos = rand() % vec_boy.size();
        val1 = vec_boy[pos];
        val2 = vec_girl[pos];
        band_3 = val1[1];
        band_4 = val2[1];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_3[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_3[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_3[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_3[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_3[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_4[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_4[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_4[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_4[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_4[4].c_str())));
        ++i;

        //4
        pos = rand() % vec_girl.size();
        val1 = vec_girl[pos];
        val2 = vec_boy[pos];
        band_3 = val1[4];
        band_4 = val2[4];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_3[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_3[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_3[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_3[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_3[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_4[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_4[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_4[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_4[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_4[4].c_str())));
        ++i;
    }

    // 332241
    {
        auto vec_boy = vec_boys;
        auto vec_girl = vec_girls;

        //3
        auto pos = rand() % vec_boy.size();
        auto val1 = vec_boy[pos];
        auto val2 = vec_girl[pos];
        auto band_5 = val1[3];
        auto band_6 = val2[3];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("5")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_5[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_5[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_5[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_5[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_5[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("6")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_5[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_6[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_6[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_6[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_6[4].c_str())));
        ++i;

        //3
        pos = rand() % vec_girl.size();
        val1 = vec_girl[pos];
        val2 = vec_boy[pos];
        band_5 = val1[3];
        band_6 = val2[3];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("5")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_5[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_5[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_5[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_5[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_5[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("6")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_6[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_6[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_6[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_6[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_6[4].c_str())));
        ++i;

        //2
        pos = rand() % vec_boy.size();
        val1 = vec_boy[pos];
        val2 = vec_girl[pos];
        band_5 = val1[2];
        band_6 = val2[2];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("5")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_5[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_5[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_5[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_5[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_5[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("6")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_6[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_6[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_6[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_6[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_6[4].c_str())));
        ++i;

        //2
        pos = rand() % vec_girl.size();
        val1 = vec_girl[pos];
        val2 = vec_boy[pos];
        band_5 = val1[2];
        band_6 = val2[2];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("5")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_5[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_5[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_5[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_5[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_5[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("6")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_6[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_6[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_6[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_6[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_6[4].c_str())));
        ++i;

        //4
        pos = rand() % vec_boy.size();
        val1 = vec_boy[pos];
        val2 = vec_girl[pos];
        band_5 = val1[4];
        band_6 = val2[4];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("5")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_5[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_5[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_5[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_5[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_5[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("6")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_6[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_6[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_6[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_6[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_6[4].c_str())));
        ++i;

        //1
        pos = rand() % vec_girl.size();
        val1 = vec_girl[pos];
        val2 = vec_boy[pos];
        band_5 = val1[1];
        band_6 = val2[1];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("5")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_5[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_5[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_5[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_5[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_5[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("6")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_6[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_6[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_6[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_6[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_6[4].c_str())));
        ++i;
    }

    // 441123
    {
        auto vec_boy = vec_boys;
        auto vec_girl = vec_girls;

        //4
        auto pos = rand() % vec_boy.size();
        auto val1 = vec_boy[pos];
        auto val2 = vec_girl[pos];
        auto band_7 = val1[4];
        auto band_8 = val2[4];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("7")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_7[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_7[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_7[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_7[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_7[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("8")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_8[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_8[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_8[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_8[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_8[4].c_str())));
        ++i;

        //4
        pos = rand() % vec_girl.size();
        val1 = vec_girl[pos];
        val2 = vec_boy[pos];
        band_7 = val1[4];
        band_8 = val2[4];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("7")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_7[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_7[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_7[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_7[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_7[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("8")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_8[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_8[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_8[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_8[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_8[4].c_str())));
        ++i;

        //1
        pos = rand() % vec_boy.size();
        val1 = vec_boy[pos];
        val2 = vec_girl[pos];
        band_7 = val1[1];
        band_8 = val2[1];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("7")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_7[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_7[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_7[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_7[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_7[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("8")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_8[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_8[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_8[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_8[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_8[4].c_str())));
        ++i;

        //1
        pos = rand() % vec_girl.size();
        val1 = vec_girl[pos];
        val2 = vec_boy[pos];
        band_7 = val1[1];
        band_8 = val2[1];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("7")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_7[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_7[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_7[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_7[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_7[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("8")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_8[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_8[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_8[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_8[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_8[4].c_str())));
        ++i;

        //2
        pos = rand() % vec_boy.size();
        val1 = vec_boy[pos];
        val2 = vec_girl[pos];
        band_7 = val1[2];
        band_8 = val2[2];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("7")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_7[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_7[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_7[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_7[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_7[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("8")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_8[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_8[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_8[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_8[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_8[4].c_str())));
        ++i;

        //3
        pos = rand() % vec_girl.size();
        val1 = vec_girl[pos];
        val2 = vec_boy[pos];
        band_7 = val1[3];
        band_8 = val2[3];
        vec_boy.erase(vec_boy.begin() + pos);
        vec_girl.erase(vec_girl.begin() + pos);

        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("7")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_7[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_7[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_7[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_7[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_7[4].c_str())));
        ++i;
        ui->table_bands->setItem(i, 0, new QTableWidgetItem(QString("8")));
        ui->table_bands->setItem(i, 1, new QTableWidgetItem(QString(band_8[0].c_str())));
        ui->table_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_bands->setItem(i, 3, new QTableWidgetItem(QString(band_8[1].c_str())));
        ui->table_bands->setItem(i, 4, new QTableWidgetItem(QString(band_8[2].c_str())));
        ui->table_bands->setItem(i, 5, new QTableWidgetItem(QString(band_8[3].c_str())));
        ui->table_bands->setItem(i, 6, new QTableWidgetItem(QString(band_8[4].c_str())));
        ++i;
    }

    ui->table_bands->sortByColumn(0, Qt::SortOrder::AscendingOrder);
}

void FURS_main_window::assign_same_gender_bands_()
{
    ui->table_secondary_bands->clear();
    ui->table_secondary_bands->setRowCount(0);
    QStringList labels;
    labels << tr("BAND") << tr("SPECIALITY") << tr("RANK") << tr("LAST_NAME") << tr("FIRST_NAME") << tr("GENDER") << tr("APPLICATION_ID");
    ui->table_secondary_bands->setHorizontalHeaderLabels(labels);

    ui->table_secondary_bands->setRowCount(48);

    std::vector<std::map<int, std::vector<std::string>>> vec_boys;
    std::vector<std::map<int, std::vector<std::string>>> vec_girls;

    // Get all the information for girls-----------------------------------------------------------------------------------------------------------------------------------------------
    // singer
    std::vector<std::vector<std::string>> results_girls;
    std::string sql_query_girls("select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Singer' and gender = 'Female' and app_status = 'Accepted' order by speciality, speciality_rank asc");
    auto success = m_db_management->result_from_query(sql_query_girls, results_girls);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_girls)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_girls.push_back(local_map);
    }

    // Guitarist
    success = false;
    results_girls.clear();
    sql_query_girls = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Guitarist' and gender = 'Female' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_girls, results_girls);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_girls)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_girls.push_back(local_map);
    }

    // Bassist
    success = false;
    results_girls.clear();
    sql_query_girls = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Bassist' and gender = 'Female' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_girls, results_girls);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_girls)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_girls.push_back(local_map);
    }

    // Drummer
    success = false;
    results_girls.clear();
    sql_query_girls = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Drummer' and gender = 'Female' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_girls, results_girls);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_girls)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_girls.push_back(local_map);
    }

    // Keyboardist
    success = false;
    results_girls.clear();
    sql_query_girls = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Keyboardist' and gender = 'Female' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_girls, results_girls);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_girls)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_girls.push_back(local_map);
    }

    // Instrumentalist
    success = false;
    results_girls.clear();
    sql_query_girls = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Instrumentalist' and gender = 'Female' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_girls, results_girls);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_girls)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_girls.push_back(local_map);
    }

    //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Get all the information for boys
    // singer
    success = false;
    std::vector<std::vector<std::string>> results_boys;
    std::string sql_query_boys("select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Singer' and gender = 'Male' and app_status = 'Accepted' order by speciality, speciality_rank asc");
    success = m_db_management->result_from_query(sql_query_boys, results_boys);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_boys)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_boys.push_back(local_map);
    }

    // Guitarist
    success = false;
    results_boys.clear();
    sql_query_boys = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Guitarist' and gender = 'Male' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_boys, results_boys);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_boys)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_boys.push_back(local_map);
    }

    // Bassist
    success = false;
    results_boys.clear();
    sql_query_boys = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Bassist' and gender = 'Male' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_boys, results_boys);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_boys)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_boys.push_back(local_map);
    }

    // Drummer
    success = false;
    results_boys.clear();
    sql_query_boys = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Drummer' and gender = 'Male' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_boys, results_boys);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_boys)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_boys.push_back(local_map);
    }

    // Keyboardist
    success = false;
    results_boys.clear();
    sql_query_boys = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Keyboardist' and gender = 'Male' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_boys, results_boys);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_boys)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_boys.push_back(local_map);
    }

    // Instrumentalist
    success = false;
    results_boys.clear();
    sql_query_boys = "select speciality, speciality_rank, last_name, first_name, gender, id from applications where speciality = 'Instrumentalist' and gender = 'Male' and app_status = 'Accepted' order by speciality, speciality_rank asc";
    success = m_db_management->result_from_query(sql_query_boys, results_boys);
    if (success)
    {
        std::map<int, std::vector<std::string>> local_map;
        for (auto res: results_boys)
        {
            auto key = res.at(1);
            local_map.emplace(std::make_pair<int, std::vector<std::string>>(std::stoi(key), { res.at(0), res.at(2), res.at(3), res.at(4), res.at(5) }));
        }
        vec_boys.push_back(local_map);
    }

    // Set the rows for the tables -------------------------------------------------------------------------------------------------------------------------------------------
    int i = 0;
    // 114432
    {
        auto vec_boy  = vec_boys;
        auto vec_girl = vec_girls;

        //1
        auto pos = rand() % vec_boy.size();
        auto val = vec_boy[pos];
        auto band_1 = val[1];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band_1 = val[1];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("5")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;

        //1
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band_1 = val[1];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band_1 = val[1];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("5")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;

        //4
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band_1 = val[4];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band_1 = val[4];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("5")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;


        //4
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band_1 = val[4];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band_1 = val[4];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("5")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;


        //3
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band_1 = val[3];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band_1 = val[3];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("5")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;

        //2
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band_1 = val[2];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band_1 = val[2];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("5")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band_1[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band_1[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band_1[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band_1[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band_1[4].c_str())));
        ++i;
    }

    // 223314
    {
        auto vec_boy = vec_boys;
        auto vec_girl = vec_girls;

        //2
        auto pos = rand() % vec_boy.size();
        auto val = vec_boy[pos];
        auto band = val[2];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[2];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("6")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //2
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[2];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[2];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("6")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //3
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[3];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[3];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("6")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //3
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[3];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[3];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("6")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //1
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[1];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[1];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("6")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //4
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[4];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[4];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("6")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;
    }

    // 332241
    {
        auto vec_boy = vec_boys;
        auto vec_girl = vec_girls;

        //3
        auto pos = rand() % vec_boy.size();
        auto val = vec_boy[pos];
        auto band = val[3];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[3];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("7")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //3
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[3];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[3];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("7")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //2
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[2];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[2];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("7")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //2
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[2];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[2];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("7")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //4
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[4];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[4];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("7")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //1
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[1];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[1];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("7")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;
    }

    // 441123
    {
        auto vec_boy = vec_boys;
        auto vec_girl = vec_girls;

        //4
        auto pos = rand() % vec_boy.size();
        auto val = vec_boy[pos];
        auto band = val[4];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[4];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("8")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //4
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[4];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[4];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("8")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //1
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[1];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[1];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("8")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //1
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[1];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[1];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("8")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("1")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //2
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[2];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[2];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("8")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("2")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        //3
        pos = rand() % vec_boy.size();
        val = vec_boy[pos];
        band = val[3];
        vec_boy.erase(vec_boy.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("4")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;

        pos = rand() % vec_girl.size();
        val = vec_girl[pos];
        band = val[3];
        vec_girl.erase(vec_girl.begin() + pos);
        ui->table_secondary_bands->setItem(i, 0, new QTableWidgetItem(QString("8")));
        ui->table_secondary_bands->setItem(i, 1, new QTableWidgetItem(QString(band[0].c_str())));
        ui->table_secondary_bands->setItem(i, 2, new QTableWidgetItem(QString("3")));
        ui->table_secondary_bands->setItem(i, 3, new QTableWidgetItem(QString(band[1].c_str())));
        ui->table_secondary_bands->setItem(i, 4, new QTableWidgetItem(QString(band[2].c_str())));
        ui->table_secondary_bands->setItem(i, 5, new QTableWidgetItem(QString(band[3].c_str())));
        ui->table_secondary_bands->setItem(i, 6, new QTableWidgetItem(QString(band[4].c_str())));
        ++i;
    }

    ui->table_secondary_bands->sortByColumn(0, Qt::SortOrder::AscendingOrder);
}

void FURS_main_window::reset_bands()
{
    assign_bands_();
}

void FURS_main_window::reset_secondary_bands()
{
    assign_same_gender_bands_();
}

void FURS_main_window::update_existing_record_page(int /*state*/)
{
    refresh_existing_applications_list_(ui->table_widget_filter);
}
