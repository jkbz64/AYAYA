#ifndef GAMEITEMWIDGET_HPP
#define GAMEITEMWIDGET_HPP

#include "browseritemwidget.hpp"
#include <TwitchQt/twitchgame.hpp>

namespace Ui {
class GameItemWidget;
}

class GameItemWidget : public BrowserItemWidget {
    Q_OBJECT
public:
    explicit GameItemWidget(const Twitch::Game&, QWidget*);
    ~GameItemWidget();

    const Twitch::Game& game() const;

    void setBoxArt(const QPixmap&);
    void setViewerCount(int);

    const QPixmap& boxArt() const;
signals:
    void pressed();
    void hovered();

private:
    Ui::GameItemWidget* m_ui;
    const Twitch::Game m_game;
    int m_viewerCount;
};

#endif // GAMEITEMWIDGET_HPP
