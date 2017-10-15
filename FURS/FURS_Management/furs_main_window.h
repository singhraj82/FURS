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
    void pull_record(int, int);
    void tab_selected(int tab_index);
    void update_existing_record();

private:
    void initialize_new_application_tab_();
    void initialize_existing_application_tab_();
    void update_existing_form(const std::vector<std::string>& results);
    void clear_new_application_form_();
    void refresh_existing_applications_list_();

private:
    Ui::FURS_main_window *ui;
    std::map<std::string, std::shared_ptr<Application>> m_applications; 
    std::shared_ptr<Database_management> m_db_management;
};

#endif // FURS_MAIN_WINDOW_H
