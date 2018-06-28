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
class SettingsWidget;
class MainNavigationBar;

namespace Ui {
class MainWindow;
}

enum class Theme {
    Default,
    Dark
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void setTheme(Theme);
    const Theme& theme() const;

    void init();

    MainNavigationBar* navigationBar() const;
    void setCurrentMainWidget(QWidget*) const;
    QWidget* currentMainWidget() const;
    BrowserWidget* browserWidget() const;
    StreamWidget* streamWidget() const;
    SettingsWidget* settingsWidget() const;

private:
    Ui::MainWindow* m_ui;
    Theme m_theme;
    QVector<InitWidget*> m_initQueue;

    void setupInitWidget(InitWidget*);
    void initNextWidget();

    int m_defaultMargin;
    int m_defaultSpacing;

    void removeMargins();
    void restoreDefaultMargins();

    QWidget* m_previousWidget;

private slots:

    void onInitStarted();
    void onInitProgress(const QString&);
    void onEndedIniting();

    void onBrowsePressed();
    void onSettingsPressed();

    void onStreamEntered(const Twitch::User&, const Twitch::Stream&);

    void onEnteredFullscreenMode();
    void onLeftFullscreenMode();
};

#endif // MAINWINDOW_H
