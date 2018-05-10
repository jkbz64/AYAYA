#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>

namespace Twitch {
class User;
class Stream;
}

namespace Ui {
class MainWindow;
}
class InitWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void init();

private:
    Ui::MainWindow* m_ui;
    QVector<InitWidget*> m_initQueue;
    void setupInitWidget(InitWidget*);
    void initNextWidget();

private slots:

    void onInitStarted();
    void onInitProgress(const QString&);
    void onEndedIniting();

    void onStreamEntered(const Twitch::User&, const Twitch::Stream&);

    void onEnteredTheaterMode();
    void onEnteredFullscreenMode();
    void onLeftWindowMode();
};

#endif // MAINWINDOW_H
