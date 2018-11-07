#ifndef QUICKSTARTDIALOG_HPP
#define QUICKSTARTDIALOG_HPP

#include "../mainwindow.hpp"
#include "../player/playerwidget.hpp"
#include "../player/streamextractor.hpp"
#include <QDialog>

namespace Ui {
class QuickstartDialog;
}

using Theme = MainWindow::Theme;

class QuickstartDialog : public QDialog {
    Q_OBJECT
public:
    QuickstartDialog(QWidget* = nullptr);
    ~QuickstartDialog();

    Theme selectedTheme() const;
    PlayerWidget::Backend selectedPlayerBackend() const;
    PlayerWidget::ExtractorBackend selectedExtractorBackend() const;

private:
    Ui::QuickstartDialog* m_ui;
};

#endif // QUICKSTARTDIALOG_HPP
