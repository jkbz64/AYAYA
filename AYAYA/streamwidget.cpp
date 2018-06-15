#include "streamwidget.hpp"
#include "ui_streamwidget.h"

#include <QMessageBox>
#include <QSettings>

#include <TwitchQt/twitchstream.hpp>
#include <TwitchQt/twitchuser.hpp>

#include "chat/chatview.hpp"
#include "player/playerwidget.hpp"

namespace {
QString backendName(const PlayerBackend backend)
{
    switch (backend) {
    case PlayerBackend::MPV:
        return QString("MPV");
        break;
    }
}
}

StreamWidget::StreamWidget(QWidget* parent)
    : InitWidget(parent)
    , m_ui(new Ui::StreamWidget)
{
    m_ui->setupUi(this);

    // Init signals
    // Backend
    connect(player(), &PlayerWidget::startedBackendInit, this, &StreamWidget::onStartedBackendInit);
    connect(player(), &PlayerWidget::backendChanged, this, &StreamWidget::onBackendChanged);
    // Init-Cache
    connect(chat()->chatView()->emotesCache(), &EmotesCache::startedInitingCache, this, &StreamWidget::onStartedInitingCache);
    connect(chat()->chatView()->emotesCache(), &EmotesCache::endedInitingCache, this, &StreamWidget::onEndedInitingCache);
    // Glboal emotes
    connect(chat()->chatView()->emotesCache(), &EmotesCache::startedFetchingGlobalEmotes, this, &StreamWidget::onStartedFetchingGlobalEmotes);
    connect(chat()->chatView()->emotesCache(), &EmotesCache::globalEmotesFetchProgress, this, &StreamWidget::onGlobalEmotesFetchProgress);
    connect(chat()->chatView()->emotesCache(), &EmotesCache::fetchedGlobalEmotes, this, &StreamWidget::onFetchedGlobalEmotes);
    // Processing
    connect(player(), &PlayerWidget::playerStyleChanged, this, &StreamWidget::onPlayerStyleChanged);

    connect(m_ui->splitter, &QSplitter::splitterMoved, this, &StreamWidget::onSplitterMoved);

    layout()->setSpacing(0);
    layout()->setMargin(0);
}

StreamWidget::~StreamWidget()
{
    delete m_ui;
}

bool StreamWidget::checkInitStatus()
{
    const auto cacheInited = isFulfilled("emoteCache");
    const auto backendInited = isFulfilled("backend");
    return cacheInited && backendInited;
}

void StreamWidget::init()
{
    player()->setBackend(PlayerBackend::MPV);
    chat()->chatView()->emotesCache()->initCache();
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

void StreamWidget::onPlayerStyleChanged(PlayerStyle, PlayerStyle newStyle)
{
    if (newStyle == PlayerStyle::Theater) {
        // Reparent chat so it breaks from splitter
        chat()->hide();
        chat()->setParent(player());
        chat()->show();

        chat()->hideInput();
        chat()->setMovable(true);

        emit enteredTheaterMode();
    } else if (newStyle == PlayerStyle::Fullscreen) {
        chat()->hide();

        emit enteredFullscreenMode();
    } else {
        chat()->show();
        chat()->showInput();
        chat()->setMovable(false);

        m_ui->splitter->insertWidget(1, chat());

        emit leftFullscreenMode();
    }
}

void StreamWidget::onStartedBackendInit()
{
    emit initProgress("Initing video backend");
}

void StreamWidget::onBackendChanged(PlayerBackend backend)
{
    emit initProgress("Inited " + backendName(backend) + " backend");
    setRequirementFulfilled("backend");
    // TODO Backend specific actions
    tryToEndInit();
}

void StreamWidget::onStartedInitingCache()
{
    emit initProgress("Initing emotes cache...");
}

void StreamWidget::onCacheInitProgress(int count)
{
    emit initProgress("Loaded " + QString::number(count) + "emotes");
}

void StreamWidget::onEndedInitingCache()
{
    emit initProgress("Initialized emotes cache!");
    setRequirementFulfilled("emoteCache");

    tryToEndInit();
}

void StreamWidget::onStartedFetchingGlobalEmotes()
{
    emit initProgress("Fetching global emotes");
}

void StreamWidget::onGlobalEmotesFetchProgress(EmotesBackend emotesBackend, const QString& current, const QString& total)
{
}

void StreamWidget::onFetchedGlobalEmotes()
{
    initSettings().setValue("lastGlobalFetchDate", QDateTime::currentDateTime());
}
