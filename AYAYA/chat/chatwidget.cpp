#include "chatwidget.hpp"

#include <TwitchQt/twitchuser.hpp>

#include <IrcCore/irccommand.h>
#include <IrcCore/ircconnection.h>

ChatWidget::ChatWidget(QWidget* parent = nullptr)
    : QScrollArea(parent)
    , m_connection(new IrcConnection(this))
{
    connect(m_connection, &IrcConnection::connected, []() {
        qDebug() << "OMEGALUL";
    });

    connect(m_connection, &IrcConnection::disconnected, []() {

    });
}

ChatWidget::~ChatWidget()
{
}

void ChatWidget::openChat(const Twitch::User& user)
{
    m_connection->setHost(QString("irc.chat.twitch.tv"));
    m_connection->setUserName(QString("justinfan12345"));
    m_connection->setNickName(QString("justinfan12345"));
    m_connection->setRealName(QString("justinfan12345"));
    m_connection->sendCommand(IrcCommand::createJoin(user.m_login));
    m_connection->open();
}
