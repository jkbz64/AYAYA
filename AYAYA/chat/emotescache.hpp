#ifndef EMOTESCACHE_HPP
#define EMOTESCACHE_HPP

#include <QFile>
#include <QObject>
#include <TwitchQt/Twitch>

class QTimer;

enum class EmotesBackend {
    TwitchEmotes,
    BTTV,
    FFZ
};

class EmotesCache : public QObject {
    Q_OBJECT
public:
    explicit EmotesCache(QObject* parent = nullptr);
    ~EmotesCache();

    void initCache();
    void clearCache();
    bool hasEmote(const QString&);
    void processQueuedEmotes();

    void fetchGlobalEmotes();
    void fetchChannelEmotes(const QString&);

signals:
    void clearedCache();

    void startedInitingCache();
    void endedInitingCache();

    void startedFetchingGlobalEmotes();
    void globalEmotesFetchProgress(EmotesBackend, const QString&, const QString&);
    void fetchedGlobalEmotes();
    void fetchedChannelEmotes(const QString&);

    void queuedEmotes();

private:
    Twitch::Api* m_api;
    QDir ensureCacheDirectory();

    bool m_inited;
    QVector<Twitch::Emote> m_emotesQueue;
    bool loadGlobalEmotes();
    void loadEmotes(const Twitch::Emotes&);
};

#endif // EMOTESCACHE_HPP
