#ifndef GAMEITEMWIDGET_HPP
#define GAMEITEMWIDGET_HPP

#include "browseritemwidget.hpp"

namespace Twitch {
class Game;
}

class GameItemWidget : public BrowserItemWidget {
    Q_OBJECT
public:
    explicit GameItemWidget(const Twitch::Game&);
    GameItemWidget(const Twitch::Game&, QWidget*);

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

#endif // GAMEITEMWIDGET_HPP
