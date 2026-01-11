#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QSqlTableModel>      // <--- AÑADE ESTA LÍNEA
#include <QSortFilterProxyModel> // <--- AÑADE ESTA LÍNEA
#include <QMainWindow>

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
    QSqlTableModel *modelo;           // Para leer la base de datos
    QSortFilterProxyModel *proxyModel; // Para los filtros de búsqueda
};
#endif // MAINWINDOW_H
