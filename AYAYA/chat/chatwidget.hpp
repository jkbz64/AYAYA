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
    ~ChatWidget();

    void openChat(const Twitch::User&);

    bool isFollowingChat();
    void followChat();

    EmotesCache* cache() const;
    ChatClient* client() const;

    void hideInput();

private:
    Ui::ChatWidget* m_ui;
    EmotesCache* m_emotesCache;
    ChatClient* m_chatClient;

    // Emote Cache slots
    void onEmoteLoaded(const QPair<Twitch::Emote, QImage>&);

    // Chat Client slots
    void onJoined();
    void onMessageReceived(const QString&, const QString&);
    void onDisconnected();

    void rejoin();
};

#endif // CHATWIDGET_HPP
