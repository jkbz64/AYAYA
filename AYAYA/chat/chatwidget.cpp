#include "chatwidget.hpp"
#include "chatclient.hpp"
#include "chatview.hpp"
#include "emotescache.hpp"
#include "ui_chatwidget.h"

#include <QScrollBar>
#include <TwitchQt/twitchuser.hpp>

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::ChatWidget)
    , m_emotesCache(new EmotesCache(this))
    , m_chatClient(new ChatClient(this))
{
    m_ui->setupUi(this);

    connect(m_emotesCache, &EmotesCache::emoteCached, this, &ChatWidget::onEmoteCached);

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
    m_emotesCache->fetchChannelEmotes(user.m_login);
}

bool ChatWidget::isFollowingChat()
{
    auto scrollBar = m_ui->m_chatView->verticalScrollBar();
    return scrollBar->value() == scrollBar->maximum();
}

void ChatWidget::followChat()
{
    auto scrollBar = m_ui->m_chatView->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

EmotesCache* ChatWidget::cache() const
{
    return m_emotesCache;
}

ChatClient* ChatWidget::client() const
{
    return m_chatClient;
}

void ChatWidget::onMessageReceived(const QString& author, const QString& message)
{
    auto editedMessage = message;
    QStringList words = message.split(QRegExp("[\r\n\t ]+"), QString::SkipEmptyParts);
    for (const auto& word : words) {
        if (m_emotesCache->hasEmote(word.simplified()))
            editedMessage.replace(word, "<img src=\"" + word + "\" />");
    }
    m_ui->m_chatView->addMessage(author + ": " + editedMessage);
}

void ChatWidget::onJoined()
{
    m_ui->m_chatView->setEnabled(true);
    m_ui->m_chatView->reset();
    m_ui->m_chatView->addMessage("You have joined #" + m_chatClient->currentChannel() + ".");
}

void ChatWidget::onDisconnected()
{
    m_ui->m_chatView->setEnabled(false);
    // TODO retry until connected every x msec?
}

void ChatWidget::rejoin()
{
    // TODO rejoining strategy
}

void ChatWidget::onEmoteCached(QPair<Twitch::Emote, QImage> emote)
{
    m_ui->m_chatView->document()->addResource(QTextDocument::ImageResource, QUrl(emote.first.m_code), QVariant(emote.second));
}
