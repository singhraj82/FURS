#include "furs_main_window.h"
#include "ui_furs_main_window.h"
#include "constants.h"
#include "application.h"
#include <QMessageBox>

FURS_main_window::FURS_main_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FURS_main_window)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    // Always open main page when application launches.
    ui->furs_stacked_control->setCurrentIndex(0);

    connect(ui->login_button, SIGNAL(pressed()), this, SLOT(open_action_window()));
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

    connect(ui->button_save, SIGNAL(pressed()), this, SLOT(add_record()));
    connect(ui->button_cancel, SIGNAL(pressed()), this, SLOT(open_action_window()));
}

void FURS_main_window::initialize_existing_application_tab_()
{
    ui->combo_box_camp_exist->insertItems(0, camps);
    ui->combo_box_gender_exist->insertItems(0, gender);
    ui->combo_box_state_exist->insertItems(0, states);
    ui->combo_box_app_status_exist->insertItems(0, application_status);
    ui->combo_box_instrument_exist->insertItems(0, speciality);
    ui->combo_box_pmt_status_exist->insertItems(0, payment_status);

    connect(ui->list_widget_existing_apps, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(pull_record(QListWidgetItem*)));

    connect(ui->button_apply_existing, SIGNAL(pressed()), this, SLOT(save_and_open_action_window()));
    //connect(ui->button_cancel_existing, SIGNAL(pressed()), this, SLOT(open_action_window()));
}

// Actions stack window
void FURS_main_window::open_action_window()
{
    ui->furs_stacked_control->setCurrentIndex(1);
}

// Applications stack window
void FURS_main_window::open_applications_window()
{
    ui->furs_stacked_control->setCurrentIndex(2);
}

void FURS_main_window::add_record()
{
    auto application = std::make_shared<Application>();
    application->set_first_name(ui->line_edit_first_name->text().toStdString());
    application->set_last_name(ui->line_edit_last_name->text().toStdString());
    application->set_gender(ui->combo_box_gender->currentText().toStdString());
    application->set_speciality(ui->combo_box_instrument->currentText().toStdString());

    application->set_street_address(ui->line_edit_street->text().toStdString());
    application->set_state(ui->combo_box_state->currentText().toStdString());
    application->set_city(ui->line_edit_city->text().toStdString());
    application->set_zipcode(ui->line_edit_zipcode->text().toStdString());
    application->set_phone(ui->line_edit_phone->text().toStdString());

    application->set_application_status(ui->combo_box_app_status->currentText().toStdString());
    application->set_payment_status(ui->combo_box_pmt_status->currentText().toStdString());
    application->set_camp(ui->combo_box_camp->currentText().toStdString());

    m_applications.emplace(std::pair<std::string, std::shared_ptr<Application>>(ui->line_edit_first_name->text().toStdString(), application));
    ui->list_widget_existing_apps->addItem(ui->line_edit_first_name->text());

    // After saving the information reset the form to default values
    clear_new_application_form_();

    QMessageBox msg_box;
    msg_box.setWindowTitle("FURS");
    msg_box.setText("Application saved successfully");
    msg_box.exec();
}

void FURS_main_window::pull_record(QListWidgetItem* item)
{
    auto itr = m_applications.find(item->text().toStdString());
    if (itr != m_applications.end())
    {
        update_existing_form(itr->second);
    }
}

void FURS_main_window::update_existing_form(const std::shared_ptr<Application>& record)
{
    ui->line_edit_first_name_exist->setText(QString(record->first_name().c_str()));
    ui->line_edit_last_name_exist->setText(QString(record->last_name().c_str()));
    ui->combo_box_gender_exist->setCurrentText(QString(record->gender().c_str()));
    ui->combo_box_instrument_exist->setCurrentText(QString(record->speciality().c_str()));
    ui->line_edit_street_exist->setText(QString(record->street_address().c_str()));
    ui->combo_box_state_exist->setCurrentText(QString(record->state().c_str()));
    ui->line_edit_city_exist->setText(QString(record->city().c_str()));
    ui->line_edit_zipcode_exist->setText(QString(record->zipcode().c_str()));
    ui->line_edit_phone_exist->setText(QString(record->phone().c_str()));
    ui->combo_box_app_status_exist->setCurrentText(QString(record->application_status().c_str()));
    ui->combo_box_pmt_status_exist->setCurrentText(QString(record->payment_status().c_str()));
    ui->combo_box_camp_exist->setCurrentText(QString(record->camp().c_str()));
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
}


