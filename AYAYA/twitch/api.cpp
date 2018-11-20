#include "api.hpp"
#include <QDebug>
#include <QTimer>

Twitch::GamesReply* TwitchApi::getTopGames(int first)
{
    static Twitch::Games lastTopGames;
    if (m_lastTopGamesFetch.secsTo(QDateTime::currentDateTime()) > m_topGamesTimeout || lastTopGames.empty()) {
        auto reply = Twitch::Api::getTopGames(first);
        connect(reply, &Twitch::Reply::finished, [this, reply]() {
            if (reply->currentState() == Twitch::ReplyState::Success) {
                lastTopGames = reply->games();
                m_lastTopGamesFetch = QDateTime::currentDateTime();
            }
        });
        return reply;
    } else {
        auto reply = Twitch::Reply::fromData<Twitch::GamesReply>(this, QVariant::fromValue(lastTopGames));
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
