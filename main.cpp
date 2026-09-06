#include <QApplication>
#include "MySystemTray.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Evita la chiusura dell'app quando si chiude la finestra del dialogo
    QApplication::setQuitOnLastWindowClosed(false);

    MySystemTray my_system_tray;

    return a.exec();
}