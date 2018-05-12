#include "streamwidget.hpp"
#include "ui_streamwidget.h"

#include <QMessageBox>
#include <QSettings>

#include <TwitchQt/twitchstream.hpp>
#include <TwitchQt/twitchuser.hpp>

#include "player/controlswidget.hpp"
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
    connect(chat()->cache(), &EmotesCache::startedInitingCache, this, &StreamWidget::onStartedInitingCache);
    connect(chat()->cache(), &EmotesCache::endedInitingCache, this, &StreamWidget::onEndedInitingCache);
    // Glboal emotes
    connect(chat()->cache(), &EmotesCache::startedFetchingGlobalEmotes, this, &StreamWidget::onStartedFetchingGlobalEmotes);
    connect(chat()->cache(), &EmotesCache::globalEmotesFetchProgress, this, &StreamWidget::onGlobalEmotesFetchProgress);
    connect(chat()->cache(), &EmotesCache::fetchedGlobalEmotes, this, &StreamWidget::onFetchedGlobalEmotes);
    // Processing
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

bool StreamWidget::checkInitStatus()
{
    const auto cacheInited = isFulfilled("emoteCache");
    auto emotesDownloaded = false;
    if (initSettings().contains("lastGlobalFetchDate")) {
        QDateTime lastGlobalFetchDate = initSettings().value("lastGlobalFetchDate").toDateTime();
        emotesDownloaded = lastGlobalFetchDate.daysTo(QDateTime::currentDateTime()) <= 1;
    }
    const auto backendInited = isFulfilled("backend");
    qDebug() << cacheInited << emotesDownloaded << backendInited;
    return cacheInited && emotesDownloaded && backendInited;
}

void StreamWidget::init()
{
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
    // For now let's just show the TwitchEmotes progress because it is bigger
    if (emotesBackend == EmotesBackend::TwitchEmotes) {
        emit initProgress("Downloading TwitchEmotes: " + current + " / " + total);
    }
}

void StreamWidget::onFetchedGlobalEmotes()
{
    initSettings().setValue("lastGlobalFetchDate", QDateTime::currentDateTime());
}
