#ifndef FURS_MAIN_WINDOW_H
#define FURS_MAIN_WINDOW_H

#include <memory>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QTableWidgetItem>


namespace Ui {
class FURS_main_window;
}

class Application;
class Database_management;
class FURS_main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit FURS_main_window(QWidget *parent = 0);
    ~FURS_main_window();

public slots:
    void open_action_window();
    void open_applications_window();
    void add_record();
    void pull_record_existing(int, int);
    void pull_record_checkin(int, int);
    void tab_selected(int tab_index);
    void update_existing_record();
    void generate_letter();
    void open_checkin_page();
    void checkin_applicant();
    void open_dorms_page();
    void open_bands_page();
    void open_secondary_bands_page();
    void reset_bands();
    void reset_secondary_bands();
    void update_existing_record_page(int state);

private:
    void initialize_new_application_tab_();
    void initialize_existing_application_tab_();
    void initialize_checkin_tab_();
    void initialize_dorms_tab_();
    void initialize_bands_tab_();
    void update_existing_form(const std::vector<std::string>& results);
    void update_checkin_form(const std::vector<std::string>& results);
    void clear_new_application_form_();
    void refresh_existing_applications_list_(QTableWidget *table_widget_filter);

    bool existing_form_has_empty_fields();
    bool new_form_has_empty_fields();
    QString payment_type_();
    QString waitlist_filter_result_();
    void set_payment_type_radio_(QString payment_type);
    void set_waitlist_check_box_(QString waitlist);

    void assign_dorms_();

    void assign_bands_();
    void assign_same_gender_bands_();

private:
    friend class Furs_test;
    Ui::FURS_main_window *ui;
    std::shared_ptr<Database_management> m_db_management;
};

#endif // FURS_MAIN_WINDOW_H
