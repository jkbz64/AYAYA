#include "browserwidget.hpp"
#include "ui_browserwidget.h"

#include <TwitchQt/Twitch>

#include "browser/gamewidget.hpp"
#include "browser/streamwidget.hpp"

BrowserWidget::BrowserWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::BrowserWidget)
    , m_settings()
    , m_api(new Twitch::Api("hqq61vsp32mvxsfhy6a9o7eemxdv5e", this))
{
    m_ui->setupUi(this);
    connect(m_ui->m_streamBrowser, &StreamBrowser::streamSelected, this, &BrowserWidget::onStreamSelected);
    connect(m_ui->m_gameBrowser, &GameBrowser::gameSelected, this, &BrowserWidget::searchStreamsByGame);

    /*  connect(m_ui->m_gameBrowser, &GameBrowser::updateRequested,
        this, static_cast<void (BrowserWidget::*)(GameWidget*)>(&BrowserWidget::updateWidget), Qt::DirectConnection);
    connect(m_ui->m_streamBrowser, &StreamBrowser::updateRequested,
        this, static_cast<void (BrowserWidget::*)(StreamWidget*)>(&BrowserWidget::updateWidget), Qt::DirectConnection);*/
}

void BrowserWidget::showTopGames()
{
    m_ui->m_gameBrowser->clear();
    m_ui->m_browserStack->setCurrentWidget(m_ui->m_gameBrowser);

    auto reply = m_api->getTopGames(100);
    connect(reply, &Twitch::Reply::finished, [this, reply]() {
        if (reply->currentState() == Twitch::ReplyState::Success) {
            auto games = reply->data().value<Twitch::Games>();
            for (const Twitch::Game& game : games) {
                m_ui->m_gameBrowser->addItem(new GameWidget(game));
            }

            // Temp workaround
            auto widgets = m_ui->m_gameBrowser->browserItems();
            QVector<GameWidget*> gameWidgets;
            for (auto widget : widgets)
                gameWidgets.push_back(qobject_cast<GameWidget*>(widget));
            performUpdate(gameWidgets);
        }
    });
}

void BrowserWidget::searchStreamsByGame(const Twitch::Game& game)
{
    m_ui->m_streamBrowser->clear();
    m_ui->m_browserStack->setCurrentWidget(m_ui->m_streamBrowser);

    auto channelsReply = m_api->getStreamsByGameId(game.m_id);
    connect(channelsReply, &Twitch::Reply::finished, [this, channelsReply]() {
        auto streams = channelsReply->data().value<Twitch::Streams>();
        for (const Twitch::Stream& stream : streams) {
            m_ui->m_streamBrowser->addItem(new StreamWidget(stream));
        }
    });
}

void BrowserWidget::onStreamSelected(const Twitch::Stream& stream)
{
    auto userReply = m_api->getUserById(stream.m_userId);

    connect(userReply, &Twitch::Reply::finished, [this, stream, userReply]() {
        auto user = userReply->data().value<Twitch::User>();
        emit streamEntered(user, stream);
    });
}

void BrowserWidget::performUpdate(QVector<GameWidget*> widgets)
{
    for (QPointer<GameWidget> widget : widgets) {
        if (widget->boxArt().isNull()) {
            auto boxArtReply = m_api->getBoxArtByUrl(widget->data().value<Twitch::Game>().m_boxArtUrl, widget->width(), widget->height());
            connect(boxArtReply, &Twitch::Reply::finished, [boxArtReply, widget]() {
                if (widget) {
                    widget->setBoxArt(QPixmap::fromImage(boxArtReply->data().value<QImage>()));
                    widget->update();
                }
            });
        }
    }
}

void BrowserWidget::performUpdate(QVector<StreamWidget*> widgets)
{
    /*QStringList ids;
    QHash<qulonglong, QPointer<StreamWidget>> streamWidgets;
    for (StreamWidget* widget : widgets) {
        ids << QString::number(widget->data().m_userId);
        streamWidgets.insert(ids.back().toULongLong(), widget);
    }

    auto reply = m_api->getStreamsByUserIds(ids);
    connect(reply, &Twitch::Reply::finished, [this, streamWidgets, reply]() {
        auto streams = reply->data().value<Twitch::Streams>();
        for (const Twitch::Stream& stream : streams) {
            if (streamWidgets.find(stream.m_userId) != streamWidgets.end()) {
                auto widget = streamWidgets[stream.m_userId];
                // Update data
                if (widget) {
                    widget->m_data = stream;
                    // Preview
                    auto thumbnailReply = m_api->getBoxArtByUrl(stream.m_thumbnailUrl, widget->width(), widget->height());
                    connect(thumbnailReply, &Twitch::Reply::finished, [widget, thumbnailReply]() {
                        if (widget) {
                            widget->setPreview(QPixmap::fromImage(thumbnailReply->data().value<QImage>()));
                            widget->update();
                        }
                    });
                }
            }
        }
    });*/
}
