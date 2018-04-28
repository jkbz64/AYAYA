#ifndef GAMEWIDGET_HPP
#define GAMEWIDGET_HPP

#include "browseritemwidget.hpp"
#include <TwitchQt/twitchgame.hpp>

class GameWidget : public BrowserItemWidget {
    Q_OBJECT
public:
    explicit GameWidget(const Twitch::Game&);
    GameWidget(const Twitch::Game&, QWidget*);

    void setBoxArt(const QPixmap&);
    void setViewerCount(int);

    const QPixmap& boxArt() const;
signals:
    void pressed();
    void hovered();

protected:
    virtual void paintEvent(QPaintEvent*) override;

private:
    QPixmap m_boxArt;
    int m_viewerCount;
};

#endif // GAMEWIDGET_HPP
