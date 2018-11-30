#ifndef CHATWIDGET_HPP
#define CHATWIDGET_HPP

#include <QWidget>

namespace Twitch {
class Emote;
class Badge;
class Stream;
}

class ChatCache;
class ChatClient;
class ChatView;

class TwitchMessage;

class QSizeGrip;

namespace Ui {
class ChatWidget;
}

class ChatWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChatWidget(QWidget*);
    ~ChatWidget();

    void joinChat(const Twitch::Stream&);

    bool isFollowingChat();
    void followChat();

    void setMovable(bool);
    bool isMovable() const;

    ChatClient* client() const;
    ChatView* chatView() const;
    QSizeGrip* sizeGrip() const;

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

    QSizeGrip* m_sizeGrip;

private slots:
    void onJoinedChannel(const QString&);
    void onDisconnected();
};

#endif // CHATWIDGET_HPP
