#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QHeaderView>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("VetSystem Pro");
    this->setWindowIcon(QIcon(":/recursos/logo.png"));

    ui->txtNombre->setPlaceholderText("Ej: Firulais");
    ui->txtEspecie->setPlaceholderText("Ej: Perro, Gato...");
    ui->txtRaza->setPlaceholderText("Ej: Labrador");
    ui->txtDuenio->setPlaceholderText("Nombre del propietario");
    ui->txtTelefono->setPlaceholderText("Ej: 0991234567");

    ui->spnEdad->setValue(0);

    modelo = new QStandardItemModel(this);
    QStringList titulos;
    titulos << "Nombre" << "Especie" << "Raza" << "Edad" << "Dueño" << "Teléfono";
    modelo->setHorizontalHeaderLabels(titulos);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(modelo);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);

    ui->tabledatos->setModel(proxyModel);
    ui->tabledatos->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->tabledatos, &QTableView::customContextMenuRequested,
            this, &MainWindow::on_tabledatos_customContextMenuRequested);

    ui->tabledatos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tabledatos->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// --- NUEVA FUNCIÓN PARA SINCRONIZAR EL ARCHIVO ---
void MainWindow::actualizarArchivoDesdeModelo()
{
    QFile archivo("pacientes.txt");
    // Abrir en modo WriteOnly para sobrescribir el archivo con los datos actuales del modelo
    if (archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&archivo);
        for (int i = 0; i < modelo->rowCount(); ++i) {
            QStringList campos;
            for (int j = 0; j < modelo->columnCount(); ++j) {
                campos << modelo->item(i, j)->text();
            }
            out << campos.join(";") << "\n";
        }
        archivo.close();
    }
}

void MainWindow::on_registrarPaciente_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageRegistro);
}

void MainWindow::on_buttonGuardar_clicked()
{
    QString nombre = ui->txtNombre->text();
    QString especie = ui->txtEspecie->text();
    QString raza = ui->txtRaza->text();
    int edad = ui->spnEdad->value();
    QString dueno = ui->txtDuenio->text();
    QString telefono = ui->txtTelefono->text();

    if(nombre.isEmpty() || dueno.isEmpty()){
        QMessageBox::warning(this, "Faltan datos", "El nombre y el dueño son obligatorios.");
        return;
    }

    QRegularExpression regexTelefono("^[0-9]{10}$");
    if(!regexTelefono.match(telefono).hasMatch()){
        QMessageBox::warning(this, "Error en Teléfono", "El teléfono debe contener 10 números.");
        return;
    }

    QFile archivo("pacientes.txt");
    if (archivo.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&archivo);
        out << nombre << ";" << especie << ";" << raza << ";"
            << edad << ";" << dueno << ";" << telefono << "\n";
        archivo.close();

        // Limpiar campos
        ui->txtNombre->clear();
        ui->txtEspecie->clear();
        ui->txtRaza->clear();
        ui->spnEdad->setValue(0);
        ui->txtDuenio->clear();
        ui->txtTelefono->clear();
        ui->txtNombre->setFocus();

        QMessageBox::information(this, "Éxito", "Paciente guardado correctamente.");

        // Recargar la tabla si ya estaba abierta
        cargarDatosDesdeArchivo();
    }
}

void MainWindow::on_txtBuscar_textChanged(const QString &texto)
{
    QRegularExpression regex(texto, QRegularExpression::CaseInsensitiveOption);
    proxyModel->setFilterRegularExpression(regex);
}

void MainWindow::cargarDatosDesdeArchivo()
{
    modelo->removeRows(0, modelo->rowCount());
    QFile archivo("pacientes.txt");

    if (!archivo.exists()) return;

    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream entrada(&archivo);
        while (!entrada.atEnd()) {
            QString linea = entrada.readLine();
            QStringList campos = linea.split(";");
            if (campos.size() == 6) {
                QList<QStandardItem *> fila;
                for (const QString &valor : campos) {
                    fila.append(new QStandardItem(valor));
                }
                modelo->appendRow(fila);
            }
        }
        archivo.close();

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

void MainWindow::on_tabledatos_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->tabledatos->indexAt(pos);
    if (!index.isValid()) return;

    QMenu *menu = new QMenu(this);
    QAction *accionEditar = new QAction("✏️ Editar", this);
    QAction *accionEliminar = new QAction("❌ Eliminar", this);

    menu->addAction(accionEditar);
    menu->addAction(accionEliminar);

    // LÓGICA EDITAR
    connect(accionEditar, &QAction::triggered, [this, index]() {
        QModelIndex indiceReal = proxyModel->mapToSource(index);
        int fila = indiceReal.row();

        // Cargar datos al formulario
        ui->txtNombre->setText(modelo->item(fila, 0)->text());
        ui->txtEspecie->setText(modelo->item(fila, 1)->text());
        ui->txtRaza->setText(modelo->item(fila, 2)->text());
        ui->spnEdad->setValue(modelo->item(fila, 3)->text().toInt());
        ui->txtDuenio->setText(modelo->item(fila, 4)->text());
        ui->txtTelefono->setText(modelo->item(fila, 5)->text());

        // Eliminar el registro viejo del modelo y del archivo
        modelo->removeRow(fila);
        actualizarArchivoDesdeModelo();

        ui->stackedWidget->setCurrentWidget(ui->pageRegistro);
        QMessageBox::information(this, "Modo Edición", "Modifica los datos y presiona Guardar.");
    });

    // LÓGICA ELIMINAR
    connect(accionEliminar, &QAction::triggered, [this, index]() {
        if (QMessageBox::question(this, "Eliminar", "¿Seguro que deseas borrar este paciente?") == QMessageBox::Yes) {
            QModelIndex indiceReal = proxyModel->mapToSource(index);
            modelo->removeRow(indiceReal.row());

            // Sincronizar con el archivo .txt
            actualizarArchivoDesdeModelo();
            QMessageBox::information(this, "Eliminado", "Registro borrado del archivo.");
        }
    });

    menu->popup(ui->tabledatos->viewport()->mapToGlobal(pos));
}

void MainWindow::on_action_Salir_triggered() { this->close(); }

void MainWindow::on_actionAcerca_de_VetSystem_Pro_triggered()
{
    QMessageBox::about(this, "Créditos", "Sistema Veterinario v1.0\nDesarrollado por Francisco Lanche y Sebastian Villagomes.");
}
