#include <QDialog>
#include <memory>

namespace Ui {
    class FilesListDialog;
}
class FilesListDialog : public QDialog {
    Q_OBJECT

    public:
        FilesListDialog(QWidget *parent = nullptr);
        ~FilesListDialog() override;
    public slots:
        void addPath();
        void addRow();

    private:
        std::unique_ptr<Ui::FilesListDialog> ui;
        void initDialog();
};


