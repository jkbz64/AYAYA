#include "streamwidget.hpp"
#include "ui_streamwidget.h"

#include <TwitchQt/twitchstream.hpp>
#include <TwitchQt/twitchuser.hpp>

#include "chat/emotescache.hpp"
#include "player/controlswidget.hpp"

StreamWidget::StreamWidget(QWidget* parent)
    : MainWidget(parent)
    , m_ui(new Ui::StreamWidget)
{
    m_ui->setupUi(this);

    connect(player(), &PlayerWidget::playerStyleChanged, this, &StreamWidget::onPlayerStyleChanged);
    connect(this, &StreamWidget::enteredTheaterMode, player(), &PlayerWidget::resetStream);
    connect(this, &StreamWidget::enteredFullscreenMode, player(), &PlayerWidget::resetStream);
    connect(this, &StreamWidget::leftWindowMode, player(), &PlayerWidget::resetStream);

    connect(m_ui->splitter, &QSplitter::splitterMoved, this, &StreamWidget::onSplitterMoved);
}

StreamWidget::~StreamWidget()
{
    delete m_ui;
}

void StreamWidget::init()
{
    connect(m_ui->m_chat->cache(), &EmotesCache::startedInitingCache, [this]() {
        emit initProgress("Initing Stream Widget");
    });

    connect(chat()->cache(), &EmotesCache::initProgress, [this](int count) {
        emit initProgress("Loading cached emotes: " + QString::number(count));
    });

    connect(chat()->cache(), &EmotesCache::endedInitingCache, [this]() {
        emit endedIniting();
    });

    chat()->cache()->initCache();
    player()->setBackend(PlayerBackend::MPV);
}

void StreamWidget::initialize(const Twitch::User& user, const Twitch::Stream& stream)
{
    m_ui->m_player->openStream(user.m_login);
    m_ui->m_chat->openChat(user);
}

PlayerWidget* StreamWidget::player() const
{
    return m_ui->m_player;
}

ChatWidget* StreamWidget::chat() const
{
    return m_ui->m_chat;
}

void StreamWidget::onSplitterMoved()
{
    m_ui->m_chat->followChat();
}

void StreamWidget::onPlayerStyleChanged(PlayerStyle oldStyle, PlayerStyle newStyle)
{
    if (oldStyle == PlayerStyle::Normal) {
        setWindowFlag(Qt::Window);
        if (newStyle == PlayerStyle::Theater) {
            setWindowTitle("AYAYA - Theater Mode");
            showMaximized();
            emit enteredTheaterMode();
        } else {
            setWindowTitle("AYAYA - Fullscreen");
            showFullScreen();
            emit enteredFullscreenMode();
        }
    } else {
        if (newStyle == PlayerStyle::Fullscreen) {
            showFullScreen();
            emit enteredFullscreenMode();
        } else if (newStyle == PlayerStyle::Theater) {
            showMaximized();
            emit enteredTheaterMode();
        } else {
            setWindowFlag(Qt::Widget);
            showNormal();
            emit leftWindowMode();
        }
    }
}
