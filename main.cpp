#include <QApplication>
#include "FilesListDialog.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    FilesListDialog dialog;
    dialog.show();
    return a.exec();
}
