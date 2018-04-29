#ifndef CHATWIDGET_HPP
#define CHATWIDGET_HPP

#include <QScrollArea>

namespace Twitch {
class User;
};

class ChatServer;

class ChatWidget : public QScrollArea {
    Q_OBJECT
public:
    explicit ChatWidget(QWidget*);
    virtual ~ChatWidget();
    void openChat(const Twitch::User&);

protected:
    virtual void onJoined();
    virtual void onDisconnected();

private:
    ChatServer* m_chatServer;

    void rejoin();
};

#endif // CHATWIDGET_HPP
