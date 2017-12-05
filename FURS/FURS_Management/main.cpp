#include "furs_main_window.h"
#include <QApplication>
#include <QDebug>
#include <QtTest/QtTest>
#include "furs_test.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef QT_TESTS
    Furs_test test1;
    QTest::qExec(&test1, argc, argv);
#endif

    FURS_main_window w;
    w.show();
    return a.exec();
}
