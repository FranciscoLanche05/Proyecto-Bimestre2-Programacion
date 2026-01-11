#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Inicializamos tus herramientas
    modelo = new QSqlTableModel(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(modelo);
    modelo->setTable("Pacientes"); // Indica el nombre exacto de la tabla en tu DB
    modelo->select();              // Esta es la función LEER que recupera los datos

    // Aquí es donde "llamamos" a tu tabla de la interfaz
    // Suponiendo que en el diseño le pusiste 'tablaPacientes'
    ui->tablaPacientes->setModel(proxyModel);
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


void MainWindow::on_txtBuscar_textChanged(const QString &texto)
{
    proxyModel->setFilterKeyColumn(0);
    proxyModel->setFilterFixedString(texto);
}


void MainWindow::on_btnIrBuscar_clicked()
{
    // Cambia la vista a tu página de búsqueda y tabla
    ui->stackedWidget->setCurrentWidget(ui->pageTabla);

    // Función LEER: Actualiza la tabla con los datos más recientes
    modelo->select();
}


