#ifndef GAMEITEMWIDGET_HPP
#define GAMEITEMWIDGET_HPP

#include "browseritemwidget.hpp"
#include <TwitchQt/twitchgame.hpp>

class QPropertyAnimation;

namespace Ui {
class GameItemWidget;
}

class GameItemWidget : public BrowserItemWidget {
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)
public:
    explicit GameItemWidget(const Twitch::Game&, QWidget*);
    ~GameItemWidget();

    const Twitch::Game& game() const;

    void setBoxArt(const QPixmap&);
    void setViewerCount(int);

    const QPixmap& boxArt() const;

    void setColor(QColor color);
    const QColor& color();

protected:
    virtual void enterEvent(QEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

signals:
    void pressed();
    void hovered();

private:
    Ui::GameItemWidget* m_ui;
    const Twitch::Game m_game;
    int m_viewerCount;
    QPropertyAnimation* m_highlightAnimation;
    QColor m_currentColor;
};

#endif // GAMEITEMWIDGET_HPP
