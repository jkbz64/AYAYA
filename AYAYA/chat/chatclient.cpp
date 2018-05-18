#include "chatclient.hpp"

#include <IrcCore/irccommand.h>
#include <IrcCore/ircconnection.h>

#include <QTimer>

namespace {
QString anon()
{
    return QString("justinfan02137");
}
}

ChatClient::ChatClient(QObject* parent)
    : QObject(parent)
    , m_connection(nullptr)
    , m_retryTimer(new QTimer(this))
    , m_currentChannel()
{
}

ChatClient::~ChatClient()
{
    delete m_connection;
}

void ChatClient::setupConnection()
{
    if (m_connection) {
        m_connection->disconnect();
        m_connection->deleteLater();
    }

    m_connection = new IrcConnection("irc.chat.twitch.tv", this);

    connect(m_connection, &IrcConnection::connected, this, &ChatClient::onConnected);
    connect(m_connection, &IrcConnection::disconnected, this, &ChatClient::onDisconnected);
    connect(m_connection, &IrcConnection::messageReceived, this, &ChatClient::onMessageReceived);

    m_connection->setPort(6667);
    m_connection->setUserName(anon());
    m_connection->setNickName(anon());
    m_connection->setRealName(anon());
    m_connection->open();
}

const QString& ChatClient::currentChannel() const
{
    return m_currentChannel;
}

void ChatClient::joinChannel(const QString& channel)
{
    bool shouldDelay = false;
    if (!m_connection) {
        setupConnection();
        shouldDelay = true;
    }

    if (!channel.isEmpty()) {
        depart();

        if (shouldDelay) {
            QObject* delayer = new QObject(this);
            connect(this, &ChatClient::connected, [this, channel, delayer]() {
                m_connection->sendCommand(IrcCommand::createJoin(QString("#") + channel));
                delayer->deleteLater();
            });
        } else
            m_connection->sendCommand(IrcCommand::createJoin(QString("#") + channel));
    }
}

void ChatClient::depart()
{
    if (m_connection && !m_currentChannel.isEmpty()) {
        m_connection->sendCommand(IrcCommand::createPart(QString("#") + m_currentChannel));
    }
}

void ChatClient::onMessageReceived(IrcMessage* message)
{
    const auto messageType = message->type();

    if (messageType == IrcMessage::Type::Private) {
        auto privateMessage = qobject_cast<IrcPrivateMessage*>(message);
        emit messageReceived(message->nick(), privateMessage->content());
    } else if (messageType == IrcMessage::Type::Join) {
        m_currentChannel = qobject_cast<IrcJoinMessage*>(message)->channel().mid(1);
        emit joined();
    } else if (messageType == IrcMessage::Type::Part) {
        m_currentChannel.clear();
        emit departed();
    }

    const QString& command = message->command();
    // TODO handle commands
    message->deleteLater();
}

void ChatClient::onConnected()
{
    emit connected();
}

void ChatClient::onDisconnected()
{
    // TODO Retry until connected
    emit disconnected();
}
