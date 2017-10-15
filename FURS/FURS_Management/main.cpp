#include "furs_main_window.h"
#include <QApplication>

    #include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FURS_main_window w;
    w.show();
    return a.exec();
}
