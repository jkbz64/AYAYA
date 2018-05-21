#include "chatwidget.hpp"
#include "chatclient.hpp"
#include "chatview.hpp"
#include "emotescache.hpp"
#include "ui_chatwidget.h"

#include <QScrollBar>
#include <TwitchQt/twitchuser.hpp>

#include <QMouseEvent>

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::ChatWidget)
    , m_emotesCache(new EmotesCache(this))
    , m_chatClient(new ChatClient(this))
    , m_isMovable(false)
    , m_isMoving(false)
    , m_offset(0, 0)
{
    m_ui->setupUi(this);
    setMouseTracking(true);

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

void ChatWidget::showInput()
{
    m_ui->m_messageEdit->show();
    m_ui->m_sendButton->show();
}

void ChatWidget::hideInput()
{
    m_ui->m_messageEdit->hide();
    m_ui->m_sendButton->hide();
}

#include <QSplitter>

void ChatWidget::setMovable(bool boolean)
{
    m_isMovable = boolean;
    if (isMovable()) {
        m_ui->m_chatView->setAttribute(Qt::WA_TransparentForMouseEvents);
        m_ui->m_chatView->setStyleSheet("background-color: rgba(0, 0, 0, 0.5);");
        m_ui->m_chatView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_ui->m_chatView->setFocusPolicy(Qt::NoFocus);
    } else {
        m_ui->m_chatView->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        m_ui->m_chatView->setStyleSheet("");
    }
}

bool ChatWidget::isMovable() const
{
    return m_isMovable;
}

void ChatWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isMovable()) {
        m_isMoving = true;
        m_offset = event->pos();
    }
    QWidget::mousePressEvent(event);
}

void ChatWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isMoving && isMovable()) {
        move(mapToParent(event->pos() - m_offset));
    }
    QWidget::mouseMoveEvent(event);
}

void ChatWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isMoving && isMovable()) {
        m_isMoving = false;
    }
    QWidget::mouseReleaseEvent(event);
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
    m_ui->m_chatView->document()->addResource(QTextDocument::ImageResource, QUrl(emote.first.code()), QVariant(emote.second));
}
