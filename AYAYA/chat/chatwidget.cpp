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

    connect(m_emotesCache, &EmotesCache::addedEmote, this, &ChatWidget::onEmoteAdded);
    m_emotesCache->initCache();

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
    m_chatClient->joinChannel("smietnik_dzielnicowy");
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

void ChatWidget::onMessageReceived(const QString& author, const QString& message)
{
    auto editedMessage = message;
    QStringList words = message.split(QRegExp("[\r\n\t ]+"), QString::SkipEmptyParts);
    for (const auto& word : words) {
        qDebug() << word;
        if (m_emotes.find(word.simplified()) != m_emotes.end())
            editedMessage.replace(word, "<img src=\"" + word + "\" />");
    }
    m_ui->m_chatView->addMessage(author + ": " + editedMessage);
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

void ChatWidget::onEmoteAdded(QPair<QString, QImage> emote)
{
    qDebug() << emote.first;
    m_ui->m_chatView->document()->addResource(QTextDocument::ImageResource, QUrl(emote.first), QVariant(emote.second));
    m_emotes.insert(emote.first.simplified(), true);
}
