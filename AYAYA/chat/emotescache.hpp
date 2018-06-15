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
    bool isEmoteLoaded(const QString&);
    void processQueuedEmotes();

    void loadGlobalEmotes();
    void loadChannelEmotes(const QString&);
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

private:
    Twitch::Api* m_api;
    QDir ensureCacheDirectory();

    bool m_inited;

    QSet<QString> m_loadedEmotes;
    QVector<Twitch::Emote> m_emotesQueue;

    void fetchGlobalEmotes();
    void fetchChannelEmotes(const QString&);

private slots:
    void onLoadedEmote(const QPair<Twitch::Emote, QImage>&);
};

#endif // EMOTESCACHE_HPP
