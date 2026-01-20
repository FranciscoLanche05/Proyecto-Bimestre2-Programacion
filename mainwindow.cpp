#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QHeaderView>
//Libreria para la busqueda avanzada
#include <QRegularExpression>

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
    modelo->setHorizontalHeaderLabels(titulos);

    //Inicializamos el Filtro (Proxy)
    proxyModel = new QSortFilterProxyModel(this);

    //Conectamos el Proxy a los Datos
    proxyModel->setSourceModel(modelo);

    //Configuramos el filtro para que ignore mayúsculas/minúsculas
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1); // -1 significa buscar en TODAS las columnas

    //¡IMPORTANTE! La tabla visual ahora mira al PROXY, no al modelo directo
    ui->tabledatos->setModel(proxyModel);

    //Le decimos a la tabla que permita menús personalizados
    ui->tabledatos->setContextMenuPolicy(Qt::CustomContextMenu);

    //Conectamos el clic derecho con nuestra función visual
    connect(ui->tabledatos, &QTableView::customContextMenuRequested,
            this, &MainWindow::on_tabledatos_customContextMenuRequested);

    //Ajustes visuales
    ui->tabledatos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tabledatos->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Salir_triggered()
{
    //Preguntar antes de cerrar
    QMessageBox::StandardButton respuesta;
    respuesta = QMessageBox::question(this, "Salir",
                                      "¿Estás seguro que deseas salir del sistema?",
                                      QMessageBox::Yes | QMessageBox::No);

    if (respuesta == QMessageBox::Yes) {
        this->close();
    }
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

    //Validar Teléfono
    //Explicación del Regex "^[0-9]{10}$":
    //^      -> Inicio del texto
    //[0-9]  -> Solo permite números del 0 al 9
    //{10}   -> Debe tener EXACTAMENTE 10 caracteres
    //$      -> Fin del texto
    QRegularExpression regexTelefono("^[0-9]{10}$");

    if(!regexTelefono.match(telefono).hasMatch()){
        QMessageBox::warning(this, "Error en Teléfono",
                             "El teléfono debe contener exactamente 10 números.\nEjemplo: 0991234567");
        ui->txtTelefono->setFocus(); //Pone el cursor ahí para que corrija
        return; //Detiene la función, NO guarda nada
    }

    //Validar Edad
    if (edad <= 0) {
        QMessageBox::warning(this, "Dato Incorrecto", "La edad debe ser mayor a 0.");
        ui->spnEdad->setFocus();
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

    // 4. LIMPIEZA DE CAMPOS
        ui->txtNombre->clear();
        ui->txtEspecie->clear(); // O setCurrentIndex(0) si es combo
        ui->txtRaza->clear();
        ui->spnEdad->setValue(0);
        ui->txtDuenio->clear();
        ui->txtTelefono->clear();

        // Poner el cursor al inicio
        ui->txtNombre->setFocus();

        QMessageBox::information(this, "Éxito", "Paciente guardado correctamente.");
    }else{
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo para guardar.");
    }
}

// Implementación de las funciones de búsqueda para evitar errores de compilación
void MainWindow::on_txtBuscar_textChanged(const QString &texto)
{
    // Creamos una expresión regular para buscar coincidencias inteligentes
    QRegularExpression regex(texto, QRegularExpression::CaseInsensitiveOption);

    // Le aplicamos el filtro al proxy. La tabla se actualiza sola.
    proxyModel->setFilterRegularExpression(regex);
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
    //Cambio de pagina y recarga de los datos
    ui->stackedWidget->setCurrentWidget(ui->pageTabla);
    cargarDatosDesdeArchivo();
}

void MainWindow::on_btnConsultar_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageTabla);
    cargarDatosDesdeArchivo();
}

// funcion para mostrar la parte visual del click derecho
void MainWindow::on_tabledatos_customContextMenuRequested(const QPoint &pos)
{
    //Validar: ¿El usuario hizo clic sobre una fila o en el espacio blanco?
    QModelIndex index = ui->tabledatos->indexAt(pos);
    if (!index.isValid()) {
        return; // Si clicó en blanco, no mostramos nada
    }

    //Crear el objeto Menú
    QMenu *menu = new QMenu(this);

    //Crear las acciones (Iconos y Texto)
    //Nota: Puedes usar emojis para que se vea moderno sin cargar imágenes extra
    QAction *accionEditar = new QAction("✏️ Editar", this);
    QAction *accionEliminar = new QAction("❌ Eliminar", this);

    //Agregar las acciones al menú
    menu->addAction(accionEditar);
    menu->addAction(accionEliminar);


    //Conectar la acción EDITAR
    connect(accionEditar, &QAction::triggered, [this, index]() {

        //TRADUCCIÓN (IMPORTANTE POR EL BUSCADOR):
        // Como usamos un filtro (Proxy), la fila 1 visual puede ser la fila 50 real.
        // Convertimos el índice visual al real:
        QModelIndex indiceReal = proxyModel->mapToSource(index);
        int fila = indiceReal.row();

        //OBTENER DATOS extraemos la informacion de esa fila
        // (Asumimos el orden: 0=Nombre, 1=Especie, 2=Raza, 3=Edad, 4=Dueño, 5=Teléfono)
        QString nombre = modelo->item(fila, 0)->text();
        QString especie = modelo->item(fila, 1)->text();
        QString raza = modelo->item(fila, 2)->text();
        QString edad = modelo->item(fila, 3)->text();
        QString dueno = modelo->item(fila, 4)->text();
        QString telefono = modelo->item(fila, 5)->text();

        //RELLENAR FORMULARIO (Mandamos los datos a la página de registro)
        ui->txtNombre->setText(nombre);
        ui->txtEspecie->setText(especie);
        ui->txtRaza->setText(raza);
        ui->spnEdad->setValue(edad.toInt());
        ui->txtDuenio->setText(dueno);
        ui->txtTelefono->setText(telefono);

        //BORRAR LA FILA VIEJA (Truco para simplificar)
        //Para evitar duplicados (el viejo + el editado), borramos el viejo de la tabla.
        //Cuando el usuario le dé a "Guardar", se creará el nuevo corregido.
        modelo->removeRow(fila);

        //VIAJAR A LA PÁGINA DE REGISTRO
        ui->stackedWidget->setCurrentWidget(ui->pageRegistro);

        //AVISO AL USUARIO
        QMessageBox::information(this, "Modo Edición",
                                 "Se cargaron los datos de " + nombre + ".\nModifícalos y dale a 'Guardar'.");
    });

    //Conectar la acción ELIMINAR
    connect(accionEliminar, &QAction::triggered, [this, index]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Eliminar", "¿Seguro que quieres borrar este registro?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // Traducir índice y borrar
            QModelIndex indiceReal = proxyModel->mapToSource(index);
            modelo->removeRow(indiceReal.row());
            //Aquí faltaría que el Estudiante B guarde el cambio en el TXT.
        }
    });

    //Mostrar el menú en la posición exacta del mouse
    menu->popup(ui->tabledatos->viewport()->mapToGlobal(pos));
}



void MainWindow::on_actionAcerca_de_VetSystem_Pro_triggered()
{
    QString mensaje = "Sistema Veterinario v1.0\n\n";
    mensaje += "Desarrollado por:\n";
    mensaje += "- Francisco Lanche (Diseño e Interfaz)\n";
    mensaje += "- Sebastian Villagomes (Lógica y gestion de datos)\n\n";
    mensaje += "- Proyecto Final, Programacion";

    QMessageBox::about(this, "Créditos", mensaje);

}

