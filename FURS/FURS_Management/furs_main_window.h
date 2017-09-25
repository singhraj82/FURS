#ifndef FURS_MAIN_WINDOW_H
#define FURS_MAIN_WINDOW_H

#include <QMainWindow>
#include <memory>
#include <QListWidgetItem>

namespace Ui {
class FURS_main_window;
}

class Application;
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
    void pull_record(QListWidgetItem* item);

private:
    void initialize_new_application_tab_();
    void initialize_existing_application_tab_();
    void update_existing_form(const std::shared_ptr<Application>& application);
    void clear_new_application_form_();

private:
    Ui::FURS_main_window *ui;
    std::map<std::string, std::shared_ptr<Application>> m_applications;
};

#endif // FURS_MAIN_WINDOW_H
