#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Título e Icono de la ventana
    this->setWindowTitle("VetSystem Pro");
    this->setWindowIcon(QIcon(":/recursos/logo.png")); // Asegúrate que la ruta sea correcta

    // 2. Textos de ayuda (Placeholders)
    // Esto hace que aparezca el texto gris "Ej: Firulais" cuando la caja está vacía
    ui->txtNombre->setPlaceholderText("Ej: Firulais");
    ui->txtEspecie->setPlaceholderText("Ej: Perro, Gato...");
    ui->txtRaza->setPlaceholderText("Ej: Labrador");
    ui->txtDuenio->setPlaceholderText("Nombre del propietario");
    ui->txtTelefono->setPlaceholderText("Ej: 0991234567");

    // Configuración para la tabla (Mejora visual extra)
    ui->spnEdad->setValue(0); // Empezar en 0

    // Inicializamos el modelo para la tabla
    modelo = new QStandardItemModel(this);

    // Definimos los nombres de las columnas
    QStringList titulos;
    titulos << "Nombre" << "Especie" << "Raza" << "Edad" << "Dueño" << "Teléfono";

    // Ponemos los títulos en el modelo
    modelo->setHorizontalHeaderLabels(titulos);

    // Conectamos el modelo a la tabla visual
    ui->tableView->setModel(modelo);

    // AJUSTES VISUALES DE LA TABLA
    // Que las columnas se estiren para llenar el espacio
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Que no se pueda escribir directamente en la tabla (solo lectura)
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
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

    // 3. LIMPIEZA DE CAMPOS
        ui->txtNombre->clear();
        ui->txtEspecie->clear(); // O setCurrentIndex(0) si es combo
        ui->txtRaza->clear();
        ui->spnEdad->setValue(0);
        ui->txtDuenio->clear();
        ui->txtTelefono->clear();

        // Poner el cursor al inicio
        ui->txtNombre->setFocus();

        QMessageBox::information(this, "Éxito", "Paciente guardado correctamente.");
    }
}

// Implementación de las funciones de búsqueda para evitar errores de compilación
void MainWindow::on_txtBuscar_textChanged(const QString &texto)
{
}

void MainWindow::cargarDatosDesdeArchivo()
{
    // 1. Limpiar el modelo para evitar duplicidad de datos al recargar
    modelo->removeRows(0, modelo->rowCount());

    // 2. Definir y validar la existencia del archivo
    QFile archivo("pacientes.txt");

    if (!archivo.exists()) {
        QMessageBox::information(this, "Aviso", "No se encontraron registros previos.");
        return;
    }

    // 3. Lógica de lectura línea por línea
    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream entrada(&archivo);

        while (!entrada.atEnd()) {
            QString linea = entrada.readLine();
            QStringList campos = linea.split(";");

            // Validamos que la línea sea íntegra (6 columnas según el registro)
            if (campos.size() == 6) {
                QList<QStandardItem *> fila;
                for (const QString &valor : campos) {
                    fila.append(new QStandardItem(valor));
                }
                modelo->appendRow(fila);
            }
        }
        archivo.close();
    } else {
        QMessageBox::critical(this, "Error", "No se pudo acceder al archivo de datos.");
    }
}
void MainWindow::on_btnIrBuscar_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageTabla);
    cargarDatosDesdeArchivo();
}

void MainWindow::on_btnConsultar_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageTabla);
    cargarDatosDesdeArchivo();
}



