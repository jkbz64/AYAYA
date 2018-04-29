#include "chatwidget.hpp"

#include <TwitchQt/twitchuser.hpp>

#include <IrcCore/irccommand.h>
#include <IrcCore/ircconnection.h>

ChatWidget::ChatWidget(QWidget* parent)
    : QScrollArea(parent)
{
}

ChatWidget::~ChatWidget()
{
}

void ChatWidget::openChat(const Twitch::User& user)
{
}
