#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Salir_triggered()
{
    this->close();
}

/*Funcion para ligar el boton de registrar paciente a la pantalla registrarPaciente en el StackedWidget*/
void MainWindow::on_registrarPaciente_clicked()
{
    /*Peligro no cambiar esta linea del codigo es la que cambia la pagina a la de registrar paciente*/
    ui->stackedWidget->setCurrentWidget(ui->pageRegistro);
}

 /*--------------------------------------------------------------------------------------------*/


// Funcion para mostra mensaje de funcio del boton guardar //
void MainWindow::on_buttonGuardar_clicked()
{
    // 1. Capturamos los datos
    QString nombre = ui->txtNombre->text();
    QString especie = ui->txtEspecie->text();
    QString raza = ui->txtRaza->text();
    int edad = ui->spnEdad->value();
    QString dueno = ui->txtDuenio->text();
    QString telefono = ui->txtTelefono->text();

    // 2. Validamos que no estén vacíos
    if(nombre.isEmpty() || dueno.isEmpty()){
        QMessageBox::warning(this, "Faltan datos", "El nombre y el dueño son obligatorios.");
        return;
    }

    // 3. Mostramos confirmación (Para probar que funciona)
    QMessageBox::information(this, "Éxito",
                             "Se guardará la mascota: " + nombre + "\nDueño: " + dueno);

    // 4. Limpiamos
    ui->txtNombre->clear();
    ui->txtDuenio->clear();
    ui->spnEdad->setValue(0);

}

