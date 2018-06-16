#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Twitch {
class User;
class Stream;
}

class InitWidget;
class MainWidget;
class BrowserWidget;
class StreamWidget;
class MainNavigationBar;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void init();

    MainNavigationBar* navigationBar() const;
    void setCurrentMainWidget(QWidget*) const;
    QWidget* currentMainWidget() const;
    BrowserWidget* browserWidget() const;
    StreamWidget* streamWidget() const;

private:
    Ui::MainWindow* m_ui;
    QVector<InitWidget*> m_initQueue;

    void setupInitWidget(InitWidget*);
    void initNextWidget();

    int m_defaultMargin;
    int m_defaultSpacing;

    void removeMargins();
    void restoreDefaultMargins();

private slots:

    void onInitStarted();
    void onInitProgress(const QString&);
    void onEndedIniting();

    void onBrowsePressed();

    void onStreamEntered(const Twitch::User&, const Twitch::Stream&);

    void onEnteredFullscreenMode();
    void onLeftFullscreenMode();
};

#endif // MAINWINDOW_H
