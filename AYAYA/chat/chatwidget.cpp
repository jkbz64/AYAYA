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

    connect(m_chatClient, &ChatClient::joined, this, &ChatWidget::onJoined);
    connect(m_chatClient, &ChatClient::disconnected, this, &ChatWidget::onDisconnected);
    connect(m_chatClient, &ChatClient::messageReceived, this, &ChatWidget::onMessageReceived);

    connect(m_emotesCache, &EmotesCache::loadedEmote, this, &ChatWidget::onEmoteLoaded);
}

ChatWidget::~ChatWidget()
{
    delete m_ui;
}

void ChatWidget::openChat(const Twitch::User& user)
{
    m_chatClient->joinChannel(user.m_login);
    m_emotesCache->loadChannelEmotes(user);
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

void ChatWidget::hideInput()
{
    m_ui->m_messageEdit->hide();
    m_ui->m_sendButton->hide();
}

void ChatWidget::onMessageReceived(const QString& author, const QString& message)
{
    auto editedMessage = message;
    const auto words = QSet<QString>::fromList(message.split(QRegExp("[\r\n\t ]+")));
    for (const auto& word : words) {
        if (m_emotesCache->isEmoteLoaded(word.simplified())) {
            editedMessage.replace(word, "<img src=\"" + word + "\" />");
        }
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

void ChatWidget::onEmoteLoaded(const QPair<Twitch::Emote, QImage>& emote)
{
    qDebug() << emote.first.code();
    m_ui->m_chatView->document()->addResource(QTextDocument::ImageResource, QUrl(emote.first.code()), QVariant(emote.second));
}
