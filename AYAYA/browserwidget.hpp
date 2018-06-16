#ifndef BROWSERWIDGET_H
#define BROWSERWIDGET_H

#include "initwidget.hpp"
#include <QDateTime>

namespace Twitch {
class Api;
class User;
class Stream;
class Game;
}

class BrowserItemWidget;
class GameItemWidget;
class StreamItemWidget;
class Browser;
class GameBrowser;
class StreamBrowser;

namespace Ui {
class BrowserWidget;
}

class BrowserWidget : public InitWidget {
    Q_OBJECT
public:
    explicit BrowserWidget(QWidget* parent = nullptr);
    ~BrowserWidget();

    virtual void init() override;

    void showTopGames();
    void searchStreamsByName(const QString&);

    void setCurrentBrowser(Browser*);
    Browser* currentBrowser() const;
    GameBrowser* gameBrowser() const;
    StreamBrowser* streamBrowser() const;

signals:
    void streamEntered(const Twitch::User&, const Twitch::Stream&);

public slots:
    void searchStreamsByGame(const Twitch::Game&);

private slots:
    void onGameAdded(BrowserItemWidget*);
    void onStreamAdded(BrowserItemWidget*);

    void onGameSelected();
    void onStreamSelected();

protected:
    virtual bool checkInitStatus() override;

private:
    Ui::BrowserWidget* m_ui;
    QSettings m_settings;
    Twitch::Api* m_api;

    QDateTime m_lastTopGamesFetch;
};

#endif // BROWSERWIDGET_H
