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

    // initialization
    initialize_new_application_tab_();
    initialize_existing_application_tab_();
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

    ui->combo_box_camp_exist->insertItems(0, camps);
    ui->combo_box_gender_exist->insertItems(0, gender);
    ui->combo_box_state_exist->insertItems(0, states);
    ui->combo_box_app_status_exist->insertItems(0, application_status);
    ui->combo_box_instrument_exist->insertItems(0, speciality);
    ui->combo_box_pmt_status_exist->insertItems(0, payment_status);

    // Get the latest list of applications from database
    refresh_existing_applications_list_();

    connect(ui->registration_tab_widget, SIGNAL(currentChanged(int)), this, SLOT(tab_selected(int)));
    connect(ui->button_apply_existing, SIGNAL(pressed()), this, SLOT(save_and_open_action_window()));
    connect(ui->button_cancel_existing, SIGNAL(pressed()), this, SLOT(open_action_window()));
    connect(ui->button_apply_existing, SIGNAL(pressed()), this, SLOT(update_existing_record()));
    connect(ui->button_letter_existing, SIGNAL(pressed()), this, SLOT(generate_letter()));

    connect(ui->table_widget_filter, SIGNAL(cellClicked(int, int)), this, SLOT(pull_record(int, int)));
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

void FURS_main_window::pull_record(int row, int col)
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
    if(tab_index == 1)
    {
        refresh_existing_applications_list_();
    }
}

void FURS_main_window::refresh_existing_applications_list_()
{
    // Set the information for table
    ui->table_widget_filter->clear();
    QStringList labels;
    labels << tr("LAST_NAME") << tr("APPLICATION_ID");
    ui->table_widget_filter->setHorizontalHeaderLabels(labels);

    std::vector<std::vector<std::string>> results;
    auto success = m_db_management->result_from_query("select " + c_table_field_last_name + "," + c_table_field_id + " from applications", results);
    if (success)
    {
        ui->table_widget_filter->setRowCount((results.size()));
        for (size_t i = 0; i < results.size(); ++i)
        {
            auto values = results[i];
            ui->table_widget_filter->setItem(i, 0, new QTableWidgetItem(QString(values[0].c_str())));
            ui->table_widget_filter->setItem(i, 1, new QTableWidgetItem(QString(values[1].c_str())));

            /*auto *item = new QTableWidgetItem(QString(values[0].c_str()));
            item->setStatusTip(QString(values[1].c_str()));
            ui->table_widget_filter->setItem(i, 0, item);*/
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
    query += c_table_field_camp + "='"+ ui->combo_box_camp_exist->currentText().toStdString()+ "' ";
    query += " WHERE " + c_table_field_last_name + "='" + ui->line_edit_last_name_exist->text().toStdString() + "'";

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
