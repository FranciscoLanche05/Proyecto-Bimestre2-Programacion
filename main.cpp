#include "mainwindow.h"
#include <QSqlDatabase>
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("veterinaria.db");

    if (!db.open()) {
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
