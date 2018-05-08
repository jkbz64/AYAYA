#ifndef CHATWIDGET_HPP
#define CHATWIDGET_HPP

#include <QGraphicsView>

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
    QHash<QString, bool> m_emotes;
    ChatClient* m_chatClient;
    void rejoin();

    void onEmoteAdded(QPair<QString, QImage>);

    void onJoined();
    void onMessageReceived(const QString&, const QString&);
    void onDisconnected();
};

#endif // CHATWIDGET_HPP
