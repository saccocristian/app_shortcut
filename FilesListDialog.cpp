#include "FilesListDialog.h"
#include "ui_FilesListDialog.h"

#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QTableWidgetItem>
#include <QItemSelectionModel>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>

namespace {
    // Percorso del file shortcuts.json nella cartella dati applicativa
    QString getStorageFilePath() {
        QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(appDataDir);
        return appDataDir + "/shortcuts.json";
    }
}

FilesListDialog::FilesListDialog(QWidget* parent) : QDialog(parent) {
    ui = std::make_unique<Ui::FilesListDialog>();
    ui->setupUi(this);

    initDialog();
    loadFromFile();

    connect(ui->selectFileBtn, &QPushButton::clicked, this, &FilesListDialog::addPath);
    connect(ui->addBtn, &QPushButton::clicked, this, &FilesListDialog::addRow);
    connect(ui->removeBtn, &QPushButton::clicked, this, &FilesListDialog::removeSelectedRows);
}

FilesListDialog::~FilesListDialog() {
    saveToFile();
}

void FilesListDialog::initDialog() {
    ui->name_lineEdit->clear();
    ui->path_lineEdit->clear();

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void FilesListDialog::addPath() {
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Seleziona un file"),
        QString(),
        tr("All Files (*.*)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    ui->path_lineEdit->setText(filePath);

    // Se il nome è vuoto, propone il nome del file
    if (ui->name_lineEdit->text().trimmed().isEmpty()) {
        ui->name_lineEdit->setText(QFileInfo(filePath).fileName());
    }
}

void FilesListDialog::addRow() {
    QString name = ui->name_lineEdit->text().trimmed();
    QString path = ui->path_lineEdit->text().trimmed();

    if (name.isEmpty() || path.isEmpty()) {
        return;
    }

    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    auto* itemName = new QTableWidgetItem(name);
    itemName->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    ui->tableWidget->setItem(row, 0, itemName);

    auto* itemPath = new QTableWidgetItem(path);
    itemPath->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    ui->tableWidget->setItem(row, 1, itemPath);

    ui->name_lineEdit->clear();
    ui->path_lineEdit->clear();

    saveToFile();
    emit shortcutsChanged();
}

void FilesListDialog::removeSelectedRows() {
    auto* selectionModel = ui->tableWidget->selectionModel();
    if (!selectionModel || !selectionModel->hasSelection()) {
        return;
    }

    auto selectedRows = selectionModel->selectedRows(0);
    if (selectedRows.isEmpty()) {
        return;
    }

    // Ordinamento inverso per rimuovere senza invalidare gli indici
    std::sort(selectedRows.begin(), selectedRows.end(), [](const QModelIndex& a, const QModelIndex& b) {
        return a.row() > b.row();
    });

    ui->tableWidget->setUpdatesEnabled(false);
    for (const auto& index : selectedRows) {
        ui->tableWidget->removeRow(index.row());
    }
    ui->tableWidget->setUpdatesEnabled(true);

    saveToFile();
    emit shortcutsChanged();
}

std::vector<std::pair<QString, QString>> FilesListDialog::getItems() const {
    std::vector<std::pair<QString, QString>> items;
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QTableWidgetItem* nameItem = ui->tableWidget->item(row, 0);
        QTableWidgetItem* pathItem = ui->tableWidget->item(row, 1);
        if (nameItem && pathItem) {
            items.emplace_back(nameItem->text(), pathItem->text());
        }
    }
    return items;
}

void FilesListDialog::saveToFile() {
    QJsonArray jsonArray;

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QTableWidgetItem* nameItem = ui->tableWidget->item(row, 0);
        QTableWidgetItem* pathItem = ui->tableWidget->item(row, 1);

        if (nameItem && pathItem) {
            QJsonObject itemObj;
            itemObj["name"] = nameItem->text();
            itemObj["path"] = pathItem->text();
            jsonArray.append(itemObj);
        }
    }

    QJsonDocument doc(jsonArray);
    QFile file(getStorageFilePath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void FilesListDialog::loadFromFile() {
    QFile file(getStorageFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        return;
    }

    QJsonArray jsonArray = doc.array();
    ui->tableWidget->setRowCount(0);

    for (const QJsonValue& value : jsonArray) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject obj = value.toObject();
        QString name = obj["name"].toString();
        QString path = obj["path"].toString();

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        auto* itemName = new QTableWidgetItem(name);
        itemName->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->tableWidget->setItem(row, 0, itemName);

        auto* itemPath = new QTableWidgetItem(path);
        itemPath->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->tableWidget->setItem(row, 1, itemPath);
    }

    emit shortcutsChanged();
}