#ifndef API_HPP
#define API_HPP

#include <TwitchQt/Twitch>

class TwitchApi : public Twitch::Api {
    Q_OBJECT
public:
    using Twitch::Api::Api;

    virtual Twitch::GamesReply* getTopGames(int) override;

    qreal topGamesTimeout() const;
    void setTopGamesTimeout(const qreal& topGamesTimeout);

private:
    qreal m_topGamesTimeout = 0.0;
    QDateTime m_lastTopGamesFetch = QDateTime::currentDateTime();
};

#endif // TWITCHAPI_HPP
