#include "browserwidget.hpp"
#include "browser/gameitemwidget.hpp"
#include "browser/streamitemwidget.hpp"
#include "twitch/api.hpp"
#include "ui_browserwidget.h"
#include <QPointer>

BrowserWidget::BrowserWidget(QWidget* parent)
    : InitWidget(parent)
    , m_ui(new Ui::BrowserWidget)
    , m_api(new TwitchApi("hqq61vsp32mvxsfhy6a9o7eemxdv5e", this))
{
    m_ui->setupUi(this);
    connect(gameBrowser(), &GameBrowser::gameSelected, this, &BrowserWidget::searchStreamsByGame);
    connect(gameBrowser(), &GameBrowser::itemAdded, this, &BrowserWidget::onGameAdded);
    connect(streamBrowser(), &StreamBrowser::itemAdded, this, &BrowserWidget::onStreamAdded);

    m_api->setTopGamesTimeout(5.0); // Update top games every 30 seconds
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

    gameBrowser()->clear();
    emit initProgress("Fetching Top Twitch Games");
    auto topGamesReply = m_api->getTopGames(100);
    connect(topGamesReply, &Twitch::Reply::finished, [this, topGamesReply]() {
        if (topGamesReply->currentState() == Twitch::ReplyState::Success) {
            auto games = topGamesReply->games();
            for (const Twitch::Game& game : games)
                gameBrowser()->addGame(game);
            setRequirementFulfilled("firstTopGamesFetch");
            tryToEndInit();
        }
        topGamesReply->deleteLater();
    });
}

void BrowserWidget::searchStreamsByGame(const Twitch::Game& game)
{
    streamBrowser()->clear();
    setCurrentBrowser(streamBrowser());

    auto channelsReply = m_api->getStreamsByGameId(game.m_id);
    connect(channelsReply, &Twitch::Reply::finished, [this, channelsReply]() {
        if (channelsReply->currentState() == Twitch::ReplyState::Success) {
            auto streams = channelsReply->data().value<Twitch::Streams>();
            for (const Twitch::Stream& stream : streams)
                connect(streamBrowser()->addStream(stream), &BrowserItemWidget::pressed, this, &BrowserWidget::onStreamSelected);
        }
        channelsReply->deleteLater();
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

void BrowserWidget::onStreamAdded(BrowserItemWidget*)
{
    /*QPointer<GameItemWidget> gameWidget = qobject_cast<GameItemWidget*>(widget);
    auto boxArtReply = m_api->getBoxArtByUrl(gameWidget->game().m_boxArtUrl, gameWidget->width(), gameWidget->height());
    connect(boxArtReply, &Twitch::Reply::finished, [boxArtReply, gameWidget]() {
        if (gameWidget)
            gameWidget->setBoxArt(QPixmap::fromImage(boxArtReply->data().value<QImage>()));
    });*/
}

void BrowserWidget::onGameSelected(const Twitch::Game game)
{
    searchStreamsByGame(game);
}

void BrowserWidget::onStreamSelected()
{
    auto widget = qobject_cast<StreamItemWidget*>(sender());
    const Twitch::Stream stream = widget->stream();
    auto userReply = m_api->getUserById(stream.m_userId);

    connect(userReply, &Twitch::Reply::finished, [this, stream, userReply]() {
        if (userReply->currentState() == Twitch::ReplyState::Success) {
            auto user = userReply->data().value<Twitch::User>();
            emit streamEntered(user, stream);
        }
        userReply->deleteLater();
    });
}
