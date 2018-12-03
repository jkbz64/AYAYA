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

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    enum class Theme {
        Default = 0,
        Dark
    };
    Q_ENUM(Theme)

    explicit MainWindow(QWidget* = nullptr);
    ~MainWindow();

    void init();

    void setTheme(Theme);
    const Theme& theme() const;

    MainNavigationBar* navigationBar() const;
    void setCurrentMainWidget(QWidget*) const;
    QWidget* currentMainWidget() const;
    BrowserWidget* browserWidget() const;
    StreamWidget* streamWidget() const;
    SettingsWidget* settingsWidget() const;

private:
    Ui::MainWindow* m_ui;
    Theme m_theme;
    int m_defaultMargin;
    int m_defaultSpacing;
    QVector<InitWidget*> m_initQueue;

    void setupInitWidget(InitWidget*);
    void initNextWidget();

    void removeMargins();
    void restoreDefaultMargins();

private slots:
    void onInitStarted();
    void onInitProgress(const QString&);
    void onEndedIniting();

    void onWatchPressed();
    void onBrowsePressed();
    void onSettingsPressed();

    void onStreamEntered(const Twitch::Stream&);

    void onEnteredFullscreenMode();
    void onLeftFullscreenMode();
};

#endif // MAINWINDOW_H
