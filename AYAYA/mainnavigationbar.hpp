#ifndef MAINNAVIGATIONBAR_HPP
#define MAINNAVIGATIONBAR_HPP

#include <QWidget>

namespace Ui {
class MainNavigationBar;
}

class QPushButton;

class MainNavigationBar : public QWidget {
    Q_OBJECT

public:
    explicit MainNavigationBar(QWidget* parent = 0);
    ~MainNavigationBar();

    QPushButton* watchButton() const;
    QPushButton* browseButton() const;
    QPushButton* settingsButton() const;
    QPushButton* quitButton() const;

signals:
    void watchButtonPressed();
    void browseButtonPressed();
    void settingsButtonPressed();
    void quitButtonPressed();

private:
    Ui::MainNavigationBar* m_ui;
};

#endif // MAINNAVIGATIONBAR_HPP
