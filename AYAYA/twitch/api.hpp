#ifndef API_HPP
#define API_HPP

#include <TwitchQt/Twitch>

class TwitchApi : public Twitch::Api {
    Q_OBJECT
public:
    using Twitch::Api::Api;

    virtual Twitch::GamesReply* getTopGames(int) override;
    virtual Twitch::StreamsReply* getStreamsByGameId(const QString&, int = 30, const QString& = "") override;

    qreal topGamesTimeout() const;
    void setTopGamesTimeout(const qreal& topGamesTimeout);

    qreal topStreamsTimeout() const;
    void setTopStreamsTimeout(const qreal& topStreamsTimeout);

private:
    qreal m_topGamesTimeout = 0.0;
    qreal m_topStreamsTimeout = 0.0;
};

#endif // TWITCHAPI_HPP
