#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Twitch {
class User;
class Stream;
}

namespace Ui {
class MainWindow;
}
class MainWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void init();

private:
    Ui::MainWindow* m_ui;
    QVector<MainWidget*> m_initQueue;
    void setupInitWidget(MainWidget*);
    void initNextWidget();
private slots:

    void onInitProgress(const QString&);

    void onStreamEntered(const Twitch::User&, const Twitch::Stream&);

    void onEnteredTheaterMode();
    void onEnteredFullscreenMode();
    void onLeftTheaterMode();
    void onLeftFullscreenMode();
};

#endif // MAINWINDOW_H
