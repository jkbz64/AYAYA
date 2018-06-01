#ifndef GAMEBROWSER_H
#define GAMEBROWSER_H

#include "browser.hpp"
#include <QGraphicsView>
#include <TwitchQt/twitchgame.hpp>

class GameWidget;
class GameItemWidget;

class GameBrowser : public Browser {
    Q_OBJECT
public:
    explicit GameBrowser(QWidget* = 0);

    GameItemWidget* addGame(const Twitch::Game&);

    int rowCount() const;

    void setMinItemSize(int);
    int minItemSize() const;
    int currentItemSize() const;

signals:
    void gameSelected(const Twitch::Game&);
    void updateRequested(GameWidget*);

protected:
    virtual void resizeEvent(QResizeEvent*) override;

private:
    int m_rowCount;
    int m_minItemSize;
    int m_currentItemSize;

    void fixLayout();
};

#endif // GAMEBROWSER_H
