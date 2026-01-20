#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QRegularExpression>
#include <QMenu>
#include <QAction>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
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
    //Añadimos el slot para el botón "Consultar Pacientes" de tu interfaz
    void on_btnConsultar_clicked();
    void on_tabledatos_customContextMenuRequested(const QPoint &pos);

    void on_actionAcerca_de_VetSystem_Pro_triggered();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *modelo;
    const QString nombreArchivo = "pacientes.txt";

    // 2. Añadimos la definición del método lógico de lectura
    // Esto quita el error "Out-of-line definition" en el archivo .cpp
    void cargarDatosDesdeArchivo();

    //filtro para la busqueda de datos por nombre
    QSortFilterProxyModel *proxyModel;
};

#endif // MAINWINDOW_H
