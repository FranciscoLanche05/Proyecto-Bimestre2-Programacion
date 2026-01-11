#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // verificar si el archivo existe o crearlo.
    QFile archivo("pacientes.txt");
    if (!archivo.exists()) {
        if (archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
            archivo.close(); // Crea el archivo vacío si no existe
        }
    }

    MainWindow w;
    w.show();
    return a.exec();
}

