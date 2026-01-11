#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

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

void MainWindow::on_registrarPaciente_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageRegistro);
}

void MainWindow::on_buttonGuardar_clicked()
{
    // 1. Capturamos los datos
    QString nombre = ui->txtNombre->text();
    QString especie = ui->txtEspecie->text();
    QString raza = ui->txtRaza->text();
    int edad = ui->spnEdad->value();
    QString dueno = ui->txtDuenio->text();
    QString telefono = ui->txtTelefono->text();

    // 2. Validamos campos obligatorios
    if(nombre.isEmpty() || dueno.isEmpty()){
        QMessageBox::warning(this, "Faltan datos", "El nombre y el dueño son obligatorios.");
        return;
    }

    // 3. Lógica de registro en archivo TXT
    QFile archivo("pacientes.txt");
    if (archivo.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&archivo);
        out << nombre << ";" << especie << ";" << raza << ";"
            << edad << ";" << dueno << ";" << telefono << "\n";
        archivo.close();

        QMessageBox::information(this, "Éxito",
                                 "Se guardó la mascota: " + nombre + "\nDueño: " + dueno);

        // --- FUNCIÓN DE LIMPIEZA DE CAMPOS TRAS REGISTRO ---
        ui->txtNombre->clear();
        ui->txtEspecie->clear();
        ui->txtRaza->clear();
        ui->txtDuenio->clear();
        ui->txtTelefono->clear();
        ui->spnEdad->setValue(0);
        ui->txtNombre->setFocus(); // Opcional: pone el cursor en el primer campo
    } else {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo para guardar.");
    }
}

// Implementación de las funciones de búsqueda para evitar errores de compilación
void MainWindow::on_txtBuscar_textChanged(const QString &texto)
{
}

void MainWindow::on_btnIrBuscar_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageTabla);
}


