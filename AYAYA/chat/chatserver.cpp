#include "chatserver.hpp"

#include <IrcCore/irccommand.h>
#include <IrcCore/ircconnection.h>

#include <iostream>

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
    connect(m_connection, &IrcConnection::messageReceived, this, &ChatServer::onMessageReceived);
    connect(m_connection, &IrcConnection::privateMessageReceived, this, &ChatServer::onPrivateMessageReceived);
}

const QString& ChatServer::currentChannel() const
{
    return m_currentChannel;
}

void ChatServer::joinChannel(const QString& channel)
{
    m_currentChannel = channel;
    m_connection->sendCommand(IrcCommand::createJoin(QString("#") + channel));
}

void ChatServer::onMessageReceived(IrcMessage* message)
{
    if (message->type() == IrcMessage::Type::Private)
        return;

    const QString& command = message->command();
    // TODO handle commands
}

void ChatServer::onPrivateMessageReceived(IrcPrivateMessage* message)
{

    // TODO Replace arguments with some kind of "rich message" class
    emit messageReceived(message->nick(), message->content());
}
