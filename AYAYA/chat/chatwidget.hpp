#ifndef CHATWIDGET_HPP
#define CHATWIDGET_HPP

#include <QGraphicsView>

#include <TwitchQt/twitchemote.hpp>
#include <TwitchQt/twitchuser.hpp>

class EmotesCache;
class ChatClient;
class ChatView;

namespace Ui {
class ChatWidget;
}

class ChatWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChatWidget(QWidget*);
    virtual ~ChatWidget();

    void openChat(const Twitch::User&);

    bool isFollowingChat();
    void followChat();

protected:
    virtual void resizeEvent(QResizeEvent*) override;

private:
    Ui::ChatWidget* m_ui;
    EmotesCache* m_emotesCache;
    ChatClient* m_chatClient;
    void rejoin();

    // Emote Cache slots
    void onEmoteCached(QPair<Twitch::Emote, QImage>);

    // Chat Client slots
    void onJoined();
    void onMessageReceived(const QString&, const QString&);
    void onDisconnected();
};

#endif // CHATWIDGET_HPP
