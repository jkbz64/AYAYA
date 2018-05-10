#ifndef EMOTESCACHE_HPP
#define EMOTESCACHE_HPP

#include <QFile>
#include <QObject>
#include <TwitchQt/Twitch>

class QTimer;

class EmotesCache : public QObject {
    Q_OBJECT
public:
    explicit EmotesCache(QObject* parent = nullptr);
    ~EmotesCache();

    void initCache();
    void clearCache();
    bool hasEmote(const QString&);

    void fetchGlobalEmotes();
    void fetchChannelEmotes(const QString&);

    void forceProcessing();

signals:
    void startedInitingCache();
    void initProgress(int);
    void endedInitingCache();

    void startedProcessing();
    void processProgress(int, int);
    void endedProcessing();
    void emoteCached(QPair<Twitch::Emote, QImage>);

private:
    Twitch::Api* m_api;

    QVector<Twitch::Emote> m_emotesQueue;
    int m_processInterval;
    QTimer* m_processTimer;
    void scheduleProcessing();
    void processQueuedEmotes();

    QHash<QString, QImage> m_cachedEmotes;
    QDir ensureCache();
    bool isCached(const Twitch::Emote&);
    QFile m_cacheFile;
    void cacheEmote(const Twitch::Emote&, const QImage&);
    void appendEmoteToCacheFile(const Twitch::Emote&);
};

#endif // EMOTESCACHE_HPP
