#include "api.hpp"
#include <QDebug>
#include <QPair>
#include <QTimer>
#include <TwitchQt/twitchgamereply.hpp>

Twitch::GamesReply* TwitchApi::getTopGames(int first)
{
    static QPair<QDateTime, Twitch::Games> lastTopGames;
    if (lastTopGames.first.secsTo(QDateTime::currentDateTime()) > m_topGamesTimeout || lastTopGames.second.empty()) {
        auto reply = Twitch::Api::getTopGames(first);
        connect(reply, &Twitch::Reply::finished, [this, reply]() {
            if (reply->currentState() == Twitch::ReplyState::Success)
                lastTopGames = qMakePair(QDateTime::currentDateTime(), reply->games());
        });
        return reply;
    } else {
        auto reply = Twitch::Reply::fromData<Twitch::GamesReply>(this, QVariant::fromValue(lastTopGames.second));
        QTimer::singleShot(0, reply, &Twitch::Reply::finished);
        return reply;
    }
}

Twitch::StreamsReply* TwitchApi::getStreamsByGameId(const QString& id, int first, const QString& p)
{
    static QMap<QString, QPair<QDateTime, Twitch::Streams>> lastTopStreams;
    if (lastTopStreams[id].first.secsTo(QDateTime::currentDateTime()) > m_topStreamsTimeout || lastTopStreams[id].second.empty()) {
        auto reply = Twitch::Api::getStreamsByGameId(id, first, p);
        connect(reply, &Twitch::Reply::finished, [this, id, reply]() {
            if (reply->currentState() == Twitch::ReplyState::Success)
                lastTopStreams[id] = qMakePair(QDateTime::currentDateTime(), reply->data().value<Twitch::Streams>());
        });
        return reply;
    } else {
        auto reply = Twitch::Reply::fromData<Twitch::StreamsReply>(this, QVariant::fromValue(lastTopStreams[id].second));
        QTimer::singleShot(0, reply, &Twitch::Reply::finished);
        return reply;
    }
}

qreal TwitchApi::topGamesTimeout() const
{
    return m_topGamesTimeout;
}

void TwitchApi::setTopGamesTimeout(const qreal& topGamesTimeout)
{
    m_topGamesTimeout = topGamesTimeout;
}

qreal TwitchApi::topStreamsTimeout() const
{
    return m_topStreamsTimeout;
}

void TwitchApi::setTopStreamsTimeout(const qreal& topStreamsTimeout)
{
    m_topStreamsTimeout = topStreamsTimeout;
}
