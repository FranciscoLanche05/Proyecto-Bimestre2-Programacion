#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QStringListModel>
#include <QSortFilterProxyModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_Salir_triggered();
    void on_registrarPaciente_clicked();
    void on_buttonGuardar_clicked();
    void on_txtBuscar_textChanged(const QString &texto);
    void on_btnIrBuscar_clicked();

private:
    Ui::MainWindow *ui;
    QStringListModel *modelo;
    QSortFilterProxyModel *proxyModel;

    // Ruta del archivo
    const QString nombreArchivo = "pacientes.txt";
};

#endif // MAINWINDOW_H

