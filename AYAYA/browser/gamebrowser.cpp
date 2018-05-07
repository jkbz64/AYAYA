#include "gamebrowser.hpp"
#include "flowlayout.hpp"

#include "gameitemwidget.hpp"
#include <TwitchQt/twitchgame.hpp>

GameBrowser::GameBrowser(QWidget* parent)
    : Browser(parent)
{
    connect(this, &Browser::itemPressed, [this](BrowserItemWidget* item) {
        emit gameSelected(item->data().value<Twitch::Game>());
    });
}
