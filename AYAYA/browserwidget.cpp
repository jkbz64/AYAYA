#include "browserwidget.hpp"
#include "ui_browserwidget.h"

#include <TwitchQt/Twitch>

#include "browser/gameitemwidget.hpp"
#include "browser/streamitemwidget.hpp"

#include <QPointer>

BrowserWidget::BrowserWidget(QWidget* parent)
    : InitWidget(parent)
    , m_ui(new Ui::BrowserWidget)
    , m_settings()
    , m_api(new Twitch::Api("hqq61vsp32mvxsfhy6a9o7eemxdv5e", this))
{
    m_ui->setupUi(this);
    connect(gameBrowser(), &GameBrowser::gameSelected, this, &BrowserWidget::searchStreamsByGame);
    connect(gameBrowser(), &GameBrowser::itemAdded, this, &BrowserWidget::onGameAdded);
    connect(streamBrowser(), &StreamBrowser::itemAdded, this, &BrowserWidget::onStreamAdded);
}

BrowserWidget::~BrowserWidget()
{
    delete m_ui;
}

void BrowserWidget::init()
{
    showTopGames();
}

bool BrowserWidget::checkInitStatus()
{
    return isFulfilled("firstTopGamesFetch");
}

void BrowserWidget::showTopGames()
{
    setCurrentBrowser(gameBrowser());
    if (!m_lastTopGamesFetch.isValid() || m_lastTopGamesFetch.secsTo(QDateTime::currentDateTime()) > 60) {
        emit initProgress("Fetching Top Twitch Games");

        gameBrowser()->clear();

        auto reply = m_api->getTopGames(100);
        connect(reply, &Twitch::Reply::finished, [this, reply]() {
            if (reply->currentState() == Twitch::ReplyState::Success) {
                auto games = reply->data().value<Twitch::Games>();

                for (const Twitch::Game& game : games)
                    connect(gameBrowser()->addGame(game), &BrowserItemWidget::pressed, this, &BrowserWidget::onGameSelected);

                setRequirementFulfilled("firstTopGamesFetch");
                tryToEndInit();
            }
        });
        m_lastTopGamesFetch = QDateTime::currentDateTime();
    }
}

void BrowserWidget::searchStreamsByGame(const Twitch::Game& game)
{
    streamBrowser()->clear();
    setCurrentBrowser(streamBrowser());

    auto channelsReply = m_api->getStreamsByGameId(game.m_id);
    connect(channelsReply, &Twitch::Reply::finished, [this, channelsReply]() {
        auto streams = channelsReply->data().value<Twitch::Streams>();
        for (const Twitch::Stream& stream : streams)
            connect(streamBrowser()->addStream(stream), &BrowserItemWidget::pressed, this, &BrowserWidget::onStreamSelected);
    });
}

void BrowserWidget::onGameAdded(BrowserItemWidget* widget)
{
    QPointer<GameItemWidget> gameWidget = qobject_cast<GameItemWidget*>(widget);
    auto boxArtReply = m_api->getBoxArtByUrl(gameWidget->game().m_boxArtUrl, gameWidget->width(), gameWidget->height());
    connect(boxArtReply, &Twitch::Reply::finished, [boxArtReply, gameWidget]() {
        if (gameWidget)
            gameWidget->setBoxArt(QPixmap::fromImage(boxArtReply->data().value<QImage>()));
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

void BrowserWidget::onGameSelected()
{
    auto widget = qobject_cast<GameItemWidget*>(sender());
    searchStreamsByGame(widget->game());
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
    });
}

void BrowserWidget::setCurrentBrowser(Browser* browser)
{
    m_ui->m_browserStack->setCurrentWidget(browser);
}

Browser* BrowserWidget::currentBrowser() const
{
    return qobject_cast<Browser*>(m_ui->m_browserStack->currentWidget());
}

GameBrowser* BrowserWidget::gameBrowser() const
{
    return m_ui->m_gameBrowser;
}

StreamBrowser* BrowserWidget::streamBrowser() const
{
    return m_ui->m_streamBrowser;
}
