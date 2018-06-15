#ifndef CHATWIDGET_HPP
#define CHATWIDGET_HPP

#include <QGraphicsView>

#include <TwitchQt/twitchemote.hpp>
#include <TwitchQt/twitchuser.hpp>

class EmotesCache;
class ChatClient;
class ChatView;

class TwitchMessage;

namespace Ui {
class ChatWidget;
}

class ChatWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChatWidget(QWidget*);
    ~ChatWidget();

    void openChat(const Twitch::User&);

    bool isFollowingChat();
    void followChat();

    ChatClient* client() const;
    ChatView* chatView() const;

    void showInput();
    void hideInput();

    void setMovable(bool);
    bool isMovable() const;

protected:
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent*) override;
    virtual void wheelEvent(QWheelEvent*) override;

private:
    Ui::ChatWidget* m_ui;
    ChatClient* m_chatClient;

    float m_opacity;
    bool m_isMovable;
    bool m_isMoving;
    QPoint m_offset;

    // Emote Cache slots
    void onEmoteLoaded(const QPair<Twitch::Emote, QImage>&);

private slots:
    void onJoinedChannel(const QString&);
    void onDisconnected();
};

#endif // CHATWIDGET_HPP
