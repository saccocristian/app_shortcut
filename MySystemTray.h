#pragma once

#include <QSystemTrayIcon>
#include <QMenu>
#include "FilesListDialog.h"

class MySystemTray : public QSystemTrayIcon {
    Q_OBJECT

public:
    explicit MySystemTray(QObject* parent = nullptr);

public slots:
    void updateMenu();
    void openDialog();

private:
    FilesListDialog dialog;
    QMenu trayMenu;
};