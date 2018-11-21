#include "chatwidget.hpp"
#include "chatclient.hpp"
#include "chatview.hpp"
#include "emotescache.hpp"
#include "ui_chatwidget.h"
#include <QMouseEvent>
#include <QScrollBar>
#include <QSizeGrip>
#include <TwitchQt/twitchstream.hpp>

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::ChatWidget)
    , m_chatClient(new ChatClient(this))
    , m_opacity(0.5)
    , m_isMovable(false)
    , m_isMoving(false)
    , m_offset(0, 0)
    , m_sizeGrip(new QSizeGrip(this))
{
    m_ui->setupUi(this);
    setMouseTracking(true);

    connect(client(), &ChatClient::joinedChannel, this, &ChatWidget::onJoinedChannel);
    connect(client(), &ChatClient::joinedChannel, chatView(), &ChatView::onJoinedChannel);
    connect(client(), &ChatClient::disconnected, this, &ChatWidget::onDisconnected);

    connect(client(), &ChatClient::messageReceived, chatView(), &ChatView::onMessageReceived);

    qobject_cast<QVBoxLayout*>(layout())->addWidget(sizeGrip(), 0, Qt::AlignBottom | Qt::AlignRight);
    m_sizeGrip->hide();
}

ChatWidget::~ChatWidget()
{
    delete m_ui;
}

void ChatWidget::joinChat(const Twitch::Stream& stream)
{
    client()->joinChannel(stream.m_userName);
}

bool ChatWidget::isFollowingChat()
{
    const auto scrollBar = m_ui->m_chatView->verticalScrollBar();
    return scrollBar->value() == scrollBar->maximum();
}

void ChatWidget::followChat()
{
    const auto scrollBar = m_ui->m_chatView->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

ChatView* ChatWidget::chatView() const
{
    return m_ui->m_chatView;
}

QSizeGrip* ChatWidget::sizeGrip() const
{
    return m_sizeGrip;
}

ChatClient* ChatWidget::client() const
{
    return m_chatClient;
}

void ChatWidget::setMovable(bool boolean)
{
    m_isMovable = boolean;
    if (isMovable()) {
        setWindowFlag(Qt::SubWindow);
        m_sizeGrip->show();
        m_ui->m_chatView->setAttribute(Qt::WA_TransparentForMouseEvents);
        m_ui->m_chatView->setStyleSheet("background-color: rgba(0, 0, 0, 0.5);");
        m_ui->m_chatView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_ui->m_chatView->setFocusPolicy(Qt::NoFocus);
        followChat();
    } else {
        setWindowFlag(Qt::Widget);
        m_sizeGrip->hide();
        m_ui->m_chatView->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        m_ui->m_chatView->setStyleSheet("");
        followChat();
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
    if (m_isMoving && isMovable())
        move(mapToParent(event->pos() - m_offset));
    QWidget::mouseMoveEvent(event);
}

void ChatWidget::wheelEvent(QWheelEvent* event)
{
    if (isMovable()) {
        if (qApp->keyboardModifiers().testFlag(Qt::ControlModifier)) {
            m_opacity += event->delta() / 1000.f;
            chatView()->setStyleSheet("background-color: rgba(0, 0, 0, " + QString::number(m_opacity) + ");");
        } else {
            chatView()->verticalScrollBar()->setValue(chatView()->verticalScrollBar()->value() - event->delta());
        }
    }
    QWidget::wheelEvent(event);
}

void ChatWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isMoving && isMovable()) {
        m_isMoving = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void ChatWidget::onJoinedChannel(const QString& /*channel*/)
{
    chatView()->setEnabled(true);
}

void ChatWidget::onDisconnected()
{
    // It sometimes crashes when closing, fix this
    // chatView()->setEnabled(false);
}

void ChatWidget::onEmoteLoaded(const QPair<Twitch::Emote, QImage>& emote)
{
    m_ui->m_chatView->document()->addResource(QTextDocument::ImageResource, QUrl(emote.first.code()), QVariant(emote.second));
}
