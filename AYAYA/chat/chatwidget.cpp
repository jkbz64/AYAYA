#include "chatwidget.hpp"
#include "chatclient.hpp"
#include "chatview.hpp"
#include "ui_chatwidget.h"

#include <TwitchQt/twitchuser.hpp>

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::ChatWidget)
    , m_chatClient(new ChatClient(this))
{
    m_ui->setupUi(this);
    connect(m_chatClient, &ChatClient::joined, this, &ChatWidget::onJoined);
    connect(m_chatClient, &ChatClient::disconnected, this, &ChatWidget::onDisconnected);
    connect(m_chatClient, &ChatClient::messageReceived, this, &ChatWidget::onMessageReceived);
}

ChatWidget::~ChatWidget()
{
    delete m_ui;
}

void ChatWidget::openChat(const Twitch::User& user)
{
    m_chatClient->joinChannel(user.m_login);
}

void ChatWidget::onMessageReceived(const QString& author, const QString& message)
{
    m_ui->m_chatView->addMessage(author + ": " + message);
}

void ChatWidget::onJoined()
{
    // TODO clear chat or do some shit
    m_ui->m_chatView->reset();
    m_ui->m_chatView->addMessage("You have joined #" + m_chatClient->currentChannel() + ".");
}

void ChatWidget::onDisconnected()
{
    // TODO retry until connected every x msec?
}

void ChatWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}

void ChatWidget::rejoin()
{
    /*if (m_chatClient->depart()) {
        m_chatClient->joinChannel(m_chatClient->currentChannel());
    }*/
}
