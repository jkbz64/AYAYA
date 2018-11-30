#ifndef EMOTESCACHE_HPP
#define EMOTESCACHE_HPP

#include <QDir>
#include <QImage>
#include <QObject>
#include <QPair>
#include <QSet>
#include <TwitchQt/twitchbadge.hpp>
#include <TwitchQt/twitchemote.hpp>

class QTimer;

namespace Twitch {
class Api;
}

enum class EmotesBackend {
    TwitchEmotes,
    BTTV,
    FFZ
};

class ChatCache : public QObject {
    Q_OBJECT
public:
    explicit ChatCache(QObject* = nullptr);
    ~ChatCache();

    void initCache();
    void clearCache();
    bool isEmoteLoaded(const QString&);

    void loadGlobalEmotes();
    void loadGlobalBadges();
    void loadChannelBadges(const QString&);

    void loadEmotes(const QVector<Twitch::Emote>&);

signals:
    void clearedCache();

    void startedInitingCache();
    void endedInitingCache();

    void startedFetchingGlobalEmotes();
    void globalEmotesFetchProgress(EmotesBackend, const QString&, const QString&);
    void fetchedGlobalEmotes();

    void startedFetchingChannelEmotes();
    void fetchedChannelEmotes(const QString&);

    void queuedEmotes();
    void loadedEmote(const QPair<Twitch::Emote, QImage>&);
    void loadedBadge(const QPair<Twitch::Badge, QPair<QString, QImage>>&);

private:
    Twitch::Api* m_api;
    QDir ensureCacheDirectory();

    void processQueuedEmotes();

    bool m_inited;

    QSet<QString> m_loadedEmotes;
    QVector<Twitch::Emote> m_emotesQueue;

    void fetchGlobalEmotes();
    void fetchChannelEmotes(const QString&);

private slots:
    void onLoadedEmote(const QPair<Twitch::Emote, QImage>&);
    void onLoadedBadge(const QPair<Twitch::Badge, QPair<QString, QImage>>&);
};

#endif // EMOTESCACHE_HPP
