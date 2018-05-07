#ifndef CHATWIDGET_HPP
#define CHATWIDGET_HPP

#include <QGraphicsView>

namespace Twitch {
class User;
};

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
    virtual void onJoined();
    virtual void onMessageReceived(const QString&, const QString&);
    virtual void onDisconnected();

    virtual void resizeEvent(QResizeEvent*) override;

private:
    Ui::ChatWidget* m_ui;
    ChatClient* m_chatClient;
    void rejoin();
};

#endif // CHATWIDGET_HPP
