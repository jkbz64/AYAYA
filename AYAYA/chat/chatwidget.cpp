#include "chatwidget.hpp"

#include "chatserver.hpp"
#include <QTimer>
#include <TwitchQt/twitchuser.hpp>

ChatWidget::ChatWidget(QWidget* parent)
    : QScrollArea(parent)
    , m_chatServer(new ChatServer(this))
{
    connect(m_chatServer, &ChatServer::joined, this, &ChatWidget::onJoined);
    connect(m_chatServer, &ChatServer::disconnected, this, &ChatWidget::onDisconnected);
}

ChatWidget::~ChatWidget()
{
}

void ChatWidget::openChat(const Twitch::User& user)
{
    m_chatServer->joinChannel(user.m_login);
}

void ChatWidget::onJoined()
{
    // TODO clear chat or do some shit
}

void ChatWidget::onDisconnected()
{
    // TODO retry until connected every x msec?
}

void ChatWidget::rejoin()
{
    // Maybe depart first?
    m_chatServer->joinChannel(m_chatServer->currentChannel());
}
