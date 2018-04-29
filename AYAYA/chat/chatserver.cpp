#include "chatserver.hpp"

#include <IrcCore/irccommand.h>
#include <IrcCore/ircconnection.h>

namespace {
QString anon()
{
    return QString("justinfan02137");
}
}

ChatServer::ChatServer(QObject* parent)
    : QObject(parent)
    , m_connection(new IrcConnection("irc.chat.twitch.tv", this))
{
    m_connection->setPort(6667);
    m_connection->setUserName(anon());
    m_connection->setNickName(anon());
    m_connection->setRealName(anon());
    m_connection->open();

    connect(m_connection, &IrcConnection::connected, [this]() { emit joined(); });
    connect(m_connection, &IrcConnection::disconnected, [this]() { emit disconnected(); });
}

void ChatServer::joinChannel(const QString& channel)
{
    m_connection->sendCommand(IrcCommand::createJoin(QString("#") + channel));
}

void ChatServer::onPrivateMessageReceived(IrcPrivateMessage* message)
{
}

void ChatServer::onMessageReceived(IrcMessage* message)
{
}
