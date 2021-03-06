#ifndef BROWSERWIDGET_H
#define BROWSERWIDGET_H

#include "initwidget.hpp"

class TwitchApi;

namespace Twitch {
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
    virtual ~BrowserWidget();

    virtual void init() override;

    void showTopGames();
    void searchStreamsByName(const QString&);

    void setCurrentBrowser(Browser*);
    Browser* currentBrowser() const;
    GameBrowser* gameBrowser() const;
    StreamBrowser* streamBrowser() const;

signals:
    void streamEntered(const Twitch::Stream&);

public slots:
    void searchStreamsByGame(const Twitch::Game&);

protected:
    virtual bool checkInitStatus() override;

private:
    Ui::BrowserWidget* m_ui;
    TwitchApi* m_api;

    void onGameAdded(BrowserItemWidget*);
    void onStreamAdded(BrowserItemWidget*);

    void onGameSelected(const Twitch::Game);
    void onStreamSelected();
};

#endif // BROWSERWIDGET_H
