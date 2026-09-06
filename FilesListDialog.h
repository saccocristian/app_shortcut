#pragma once

#include <QDialog>
#include <memory>
#include <vector>
#include <utility>
#include <QString>

namespace Ui {
    class FilesListDialog;
}

class FilesListDialog : public QDialog {
    Q_OBJECT

public:
    explicit FilesListDialog(QWidget *parent = nullptr);
    ~FilesListDialog() override;

    // Restituisce la lista di coppie (Nome, Percorso) per la tray icon
    std::vector<std::pair<QString, QString>> getItems() const;

    signals:
        // Segnale emesso ogni volta che la tabella viene modificata (aggiunta, rimozione, caricamento)
        void shortcutsChanged();

public slots:
    void addPath();
    void addRow();
    void removeSelectedRows();

private:
    std::unique_ptr<Ui::FilesListDialog> ui;

    void initDialog();
    void saveToFile();
    void loadFromFile();
};