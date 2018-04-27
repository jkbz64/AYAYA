#ifndef GAMEWIDGET_HPP
#define GAMEWIDGET_HPP

#include <QFrame>

#include <TwitchQt/twitchgame.hpp>

class GameWidget : public QFrame {
    Q_OBJECT
public:
    GameWidget(const Twitch::Game&, QWidget*);

    void setBoxArt(const QPixmap&);
    void setViewerCount(int);

    const QPixmap& boxArt() const;
    const Twitch::Game& data() const;
signals:
    void pressed();
    void hovered();

protected:
    virtual void paintEvent(QPaintEvent*) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

private:
    Twitch::Game m_data;
    QPixmap m_boxArt;
    int m_viewerCount;
};

#endif // GAMEWIDGET_HPP
