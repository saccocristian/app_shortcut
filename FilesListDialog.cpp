#include "FilesListDialog.h"
#include "ui_FilesListDialog.h"

#include <QPushButton>
#include <QFileDialog>
#include <QTableWidgetItem>

FilesListDialog::FilesListDialog(QWidget* parent) : QDialog(parent) {
    ui = std::make_unique<Ui::FilesListDialog>();
    ui->setupUi(this);

    // reset dialog: svuota i campi
     initDialog();
    connect(ui->selectFileBtn, &QPushButton::clicked, this, &FilesListDialog::addPath);
    connect(ui->addBtn, &QPushButton::clicked, this, &FilesListDialog::addRow);
}

FilesListDialog::~FilesListDialog() = default;

void FilesListDialog::addPath() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Seleziona un file"), QString(),
            tr("All Files (*.*);;") // Filtri di estensione
        );

    if (filePath.isEmpty()) {
        return;
    }
    ui->path_lineEdit->setText(filePath);
}

void FilesListDialog::initDialog() {
    ui->name_lineEdit->setText("");
    ui->path_lineEdit->setText("");
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // logiche per fill tabella e file corrispettivo

}

void FilesListDialog::addRow() {
    QString name = ui->name_lineEdit->text().trimmed();
    QString path = ui->path_lineEdit->text().trimmed();

    if (name.isEmpty() || path.isEmpty()) {
        return;
    }

    // 1. Alloca la nuova riga in fondo
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    // 2. Colonna 0 (Name): inserisce il testo e attiva la Checkbox
    auto* itemName = new QTableWidgetItem(name);
    itemName->setFlags(itemName->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    itemName->setCheckState(Qt::Unchecked);
    ui->tableWidget->setItem(row, 0, itemName);

    // 3. Colonna 1 (Path): solo testo in sola lettura
    auto* itemPath = new QTableWidgetItem(path);
    itemPath->setFlags(itemPath->flags() & ~Qt::ItemIsEditable);
    ui->tableWidget->setItem(row, 1, itemPath);

    // 4. Svuota i campi
    ui->name_lineEdit->clear();
    ui->path_lineEdit->clear();
}
