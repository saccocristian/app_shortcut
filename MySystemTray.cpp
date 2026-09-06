#include "MySystemTray.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QIcon>

MySystemTray::MySystemTray(QObject* parent) : QSystemTrayIcon(parent) {
    setIcon(QIcon("../icons/rocket.png"));
    setToolTip("Shortcuts");

    // Assegna il QMenu persistente (membro della classe)
    setContextMenu(&trayMenu);

    // Quando la tabella cambia, rigenera le voci del menu
    connect(&dialog, &FilesListDialog::shortcutsChanged, this, &MySystemTray::updateMenu);

    // Doppio click sull'icona per aprire la finestra di gestione
    connect(this, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            openDialog();
        }
    });

    // Popola il menu all'avvio con i dati letti dal Dialog
    updateMenu();
    show();
}

void MySystemTray::openDialog() {
    dialog.show();
    dialog.raise();
    dialog.activateWindow();
}

void MySystemTray::updateMenu() {
    trayMenu.clear();

    // 1. Aggiungi le voci dei file salvati
    auto shortcuts = dialog.getItems();
    if (shortcuts.empty()) {
        auto* emptyAction = trayMenu.addAction("Nessun file configurato");
        emptyAction->setEnabled(false);
    } else {
        for (const auto& [name, path] : shortcuts) {
            QAction* action = trayMenu.addAction(name);
            connect(action, &QAction::triggered, this, [path]() {
                // Apre il file o l'eseguibile con l'applicazione predefinita del sistema operativo
                QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            });
        }
    }

    trayMenu.addSeparator();

    // 2. Opzione per gestire i collegamenti
    QAction* manageAction = trayMenu.addAction("Gestisci Scorciatoie...");
    connect(manageAction, &QAction::triggered, this, &MySystemTray::openDialog);

    trayMenu.addSeparator();

    // 3. Opzione di uscita reale
    QAction* quitAction = trayMenu.addAction("Esci");
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}