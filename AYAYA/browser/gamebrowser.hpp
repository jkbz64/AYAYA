#ifndef GAMEBROWSER_H
#define GAMEBROWSER_H

#include "browser.hpp"

namespace Twitch {
class Game;
}

class GameWidget;

class GameBrowser : public Browser {
    Q_OBJECT
public:
    explicit GameBrowser(QWidget* = 0);
signals:
    void gameSelected(const Twitch::Game&);
    void updateRequested(GameWidget*);
};

#endif // GAMEBROWSER_H
