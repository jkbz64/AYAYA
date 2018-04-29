#ifndef CHATWIDGET_HPP
#define CHATWIDGET_HPP

#include <QScrollArea>

namespace Twitch {
class User;
}

class IrcConnection;

class ChatWidget : public QScrollArea {
    Q_OBJECT
public:
    explicit ChatWidget(QWidget*);
    ~ChatWidget();
    void openChat(const Twitch::User&);
};

#endif // CHATWIDGET_HPP
