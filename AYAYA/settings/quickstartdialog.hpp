#ifndef QUICKSTARTDIALOG_HPP
#define QUICKSTARTDIALOG_HPP

#include <QDialog>

namespace Ui {
class QuickstartDialog;
}

class QuickstartDialog : public QDialog {
    Q_OBJECT
public:
    QuickstartDialog(QWidget* = nullptr);

private:
    Ui::QuickstartDialog* m_ui;

private slots:
    void onYtdlPressed();
    void onStreamlinkPressed();

    void checkValidity();
};

#endif // QUICKSTARTDIALOG_HPP
