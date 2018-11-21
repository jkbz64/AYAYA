#include "streamwidget.hpp"
#include "chat/chatview.hpp"
#include "player/playerwidget.hpp"
#include "ui_streamwidget.h"
#include <TwitchQt/twitchstream.hpp>
#include <TwitchQt/twitchuser.hpp>

namespace {
using PlayerBackend = PlayerWidget::Backend;
using ExtractorBackend = PlayerWidget::ExtractorBackend;
QString backendName(const PlayerBackend backend)
{
    switch (backend) {
    case PlayerBackend::Mpv:
        return QString("MPV");
        break;
    case PlayerBackend::Vlc:
        return QString("Vlc");
        break;
    case PlayerBackend::Null:
        return QString("Null");
        break;
    }

    return QString("Null");
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
    // Processing
    connect(player(), &PlayerWidget::playerStyleChanged, this, &StreamWidget::onPlayerStyleChanged);

    connect(streamSplitter(), &QSplitter::splitterMoved, this, &StreamWidget::onSplitterMoved);
}

StreamWidget::~StreamWidget()
{
    delete m_ui;
}

PlayerWidget* StreamWidget::player() const
{
    return m_ui->m_player;
}

ChatWidget* StreamWidget::chat() const
{
    return m_ui->m_chat;
}

QSplitter* StreamWidget::streamSplitter() const
{
    return m_ui->m_splitter;
}

bool StreamWidget::checkInitStatus()
{
    const auto cacheInited = isFulfilled("emoteCache");
    const auto backendInited = isFulfilled("backend");
    return cacheInited && backendInited;
}

void StreamWidget::init()
{
    chat()->chatView()->emotesCache()->initCache();
}

void StreamWidget::openStream(const Twitch::Stream& stream)
{
    player()->openStream(stream);
    chat()->joinChat(stream);
}

// Slots
void StreamWidget::onPlayerStyleChanged(PlayerStyle, PlayerStyle newStyle)
{
    if (newStyle == PlayerStyle::Theater) {
        // Reparent chat so it breaks from splitter
        chat()->hide();
        chat()->setParent(player());
        chat()->setMovable(true);
        chat()->show();
        chat()->followChat();

        emit enteredTheaterMode();
    } else if (newStyle == PlayerStyle::Fullscreen) {
        chat()->hide();
        emit enteredFullscreenMode();
    } else {
        chat()->setMovable(false);
        chat()->show();
        chat()->followChat();

        streamSplitter()->insertWidget(1, chat());

        emit leftFullscreenMode();
    }
}

void StreamWidget::onSplitterMoved()
{
    chat()->followChat();
}

void StreamWidget::onStartedBackendInit()
{
    emit initProgress("Initing video backend");
}

void StreamWidget::onBackendChanged(PlayerBackend backend)
{
    emit initProgress("Inited " + backendName(backend) + " backend");
    setRequirementFulfilled("backend");
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
