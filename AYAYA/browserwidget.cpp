#include "browserwidget.hpp"
#include "browser/gameitemwidget.hpp"
#include "browser/streamitemwidget.hpp"
#include "twitch/api.hpp"
#include "ui_browserwidget.h"
#include <QMessageBox>
#include <QPointer>
#include <QTimer>

BrowserWidget::BrowserWidget(QWidget* parent)
    : InitWidget(parent)
    , m_ui(new Ui::BrowserWidget)
    , m_api(new TwitchApi("hqq61vsp32mvxsfhy6a9o7eemxdv5e", this))
{
    m_ui->setupUi(this);
    connect(gameBrowser(), &GameBrowser::gameSelected, this, &BrowserWidget::searchStreamsByGame);
    connect(gameBrowser(), &GameBrowser::itemAdded, this, &BrowserWidget::onGameAdded);
    connect(streamBrowser(), &StreamBrowser::itemAdded, this, &BrowserWidget::onStreamAdded);

    m_api->setTopGamesTimeout(30.0); // Update top games every 30 seconds
    m_api->setTopStreamsTimeout(20.0);
}

BrowserWidget::~BrowserWidget()
{
    delete m_ui;
}

GameBrowser* BrowserWidget::gameBrowser() const
{
    return m_ui->m_gameBrowser;
}

StreamBrowser* BrowserWidget::streamBrowser() const
{
    return m_ui->m_streamBrowser;
}

bool BrowserWidget::checkInitStatus()
{
    return isFulfilled("firstTopGamesFetch");
}

void BrowserWidget::init()
{
    showTopGames();
}

void BrowserWidget::showTopGames()
{
    setCurrentBrowser(gameBrowser());
    gameBrowser()->clear();
    streamBrowser()->setUpdatesEnabled(false);

    gameBrowser()->clear();
    emit initProgress("Fetching Top Twitch Games");
    auto topGamesReply = m_api->getTopGames(100);
    connect(topGamesReply, &Twitch::Reply::finished, [this, topGamesReply]() {
        if (topGamesReply->currentState() == Twitch::ReplyState::Success) {
            auto games = topGamesReply->games();
            for (const Twitch::Game& game : games)
                gameBrowser()->addGame(game);
        } else
            QMessageBox::warning(this, "No connection / Connection timed out", "Could not reach Twitch servers, check your internet connection or try again");
        topGamesReply->deleteLater();
        setRequirementFulfilled("firstTopGamesFetch");
        tryToEndInit();
    });

    QTimer::singleShot(100, [this]() {
        gameBrowser()->setUpdatesEnabled(true);
    });
}

void BrowserWidget::searchStreamsByGame(const Twitch::Game& game)
{
    setCurrentBrowser(streamBrowser());
    streamBrowser()->clear();
    streamBrowser()->setUpdatesEnabled(false);

    auto channelsReply = m_api->getStreamsByGameId(game.m_id);
    connect(channelsReply, &Twitch::Reply::finished, [this, channelsReply]() {
        if (channelsReply->currentState() == Twitch::ReplyState::Success) {
            auto streams = channelsReply->data().value<Twitch::Streams>();
            for (const Twitch::Stream& stream : streams)
                connect(streamBrowser()->addStream(stream), &BrowserItemWidget::pressed, this, &BrowserWidget::onStreamSelected);
        }
        channelsReply->deleteLater();
    });

    QTimer::singleShot(100, [this]() {
        streamBrowser()->setUpdatesEnabled(true);
    });
}

Browser* BrowserWidget::currentBrowser() const
{
    return qobject_cast<Browser*>(m_ui->m_browserStack->currentWidget());
}

void BrowserWidget::setCurrentBrowser(Browser* browser)
{
    m_ui->m_browserStack->setCurrentWidget(browser);
}

// Slots
void BrowserWidget::onGameAdded(BrowserItemWidget* widget)
{
    QPointer<GameItemWidget> gameWidget = qobject_cast<GameItemWidget*>(widget);
    auto boxArtReply = m_api->getBoxArtByUrl(gameWidget->game().m_boxArtUrl, gameWidget->width(), gameWidget->height());
    connect(boxArtReply, &Twitch::Reply::finished, [boxArtReply, gameWidget]() {
        if (boxArtReply->currentState() == Twitch::ReplyState::Success) {
            if (gameWidget) {
                if (boxArtReply->data().value<QImage>().isNull())
                    gameWidget->setBoxArt(QPixmap(":/images/404.jpg"));
                else
                    gameWidget->setBoxArt(QPixmap::fromImage(boxArtReply->data().value<QImage>()));
            }
        }
        boxArtReply->deleteLater();
    });
}

void BrowserWidget::onStreamAdded(BrowserItemWidget* widget)
{
    QPointer<StreamItemWidget> streamWidget = qobject_cast<StreamItemWidget*>(widget);
    auto boxArtReply = m_api->getBoxArtByUrl(streamWidget->stream().m_thumbnailUrl, streamWidget->width(), streamWidget->height());
    connect(boxArtReply, &Twitch::Reply::finished, [boxArtReply, streamWidget]() {
        if (streamWidget)
            streamWidget->setPreview(QPixmap::fromImage(boxArtReply->data().value<QImage>()));
        boxArtReply->deleteLater();
    });
}

void BrowserWidget::onGameSelected(const Twitch::Game game)
{
    searchStreamsByGame(game);
}

void BrowserWidget::onStreamSelected()
{
    auto widget = qobject_cast<StreamItemWidget*>(sender());
    const Twitch::Stream stream = widget->stream();
    emit streamEntered(widget->stream());
}
