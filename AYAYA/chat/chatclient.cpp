#include "chatclient.hpp"

#include <IrcCore/irccommand.h>
#include <IrcCore/ircconnection.h>

namespace {
QString anon()
{
    return QString("justinfan02137");
}
}

ChatClient::ChatClient(QObject* parent)
    : QObject(parent)
    , m_connection(nullptr)
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

    m_connection->sendCommand(IrcCommand::createCapability("REQ", "twitch.tv/membership"));
    m_connection->sendCommand(IrcCommand::createCapability("REQ", "twitch.tv/commands"));
    m_connection->sendCommand(IrcCommand::createCapability("REQ", "twitch.tv/tags"));
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
    if (m_connection && !m_currentChannel.isEmpty())
        m_connection->sendCommand(IrcCommand::createPart(QString("#") + m_currentChannel));
}

#include <QRegularExpression>

void ChatClient::onMessageReceived(IrcMessage* message)
{
    const auto messageType = message->type();

    if (messageType == IrcMessage::Type::Private) {
        auto privateMessage = qobject_cast<IrcPrivateMessage*>(message);
        // First group is the emote id, second and third is the first range of the emote text to be replaced
        // At this point we only need to know the ID and emote Code, our replacing algorithm in the chat view is going to repalce all the stuff
        QRegularExpression reg("(\\d+?):(\\d*)-(\\d*)");
        QRegularExpressionMatchIterator i = reg.globalMatch(privateMessage->tag("emotes").toString());
        QVector<Twitch::Emote> emotes;
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            const auto id = match.captured(1);
            const auto startIndex = match.captured(2).toInt();
            const auto endIndex = match.captured(3).toInt();
            emotes.push_back(Twitch::Emote::createEmote<Twitch::TwitchEmotes::Emote>(id.toInt(), privateMessage->content().mid(startIndex, 1 + endIndex - startIndex), -1, QString()));
        }
        emit messageReceived(TwitchMessage{
            message->nick(),
            privateMessage->content(),
            QColor(privateMessage->tag("color").toString()),
            privateMessage->tag("mod").toInt() == 1,
            privateMessage->tag("subscriber").toInt() == 1,
            emotes });
    } else if (messageType == IrcMessage::Type::Join) {
        m_currentChannel = qobject_cast<IrcJoinMessage*>(message)->channel().mid(1);
        emit joinedChannel(m_currentChannel);
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
    emit disconnected();
}
