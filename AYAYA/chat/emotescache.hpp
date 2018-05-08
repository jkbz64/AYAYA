#ifndef EMOTESCACHE_HPP
#define EMOTESCACHE_HPP

#include <QObject>
#include <TwitchQt/Twitch>

class QTimer;

class EmotesCache : public QObject {
    Q_OBJECT
public:
    explicit EmotesCache(QObject* parent = nullptr);
    ~EmotesCache();

    void initCache();
    void fetchChannelEmotes(const QString&);
    void clearCache();

signals:
    void addedEmote(QPair<QString, QImage>);

private:
    Twitch::Api* m_api;
    bool validateCacheDirectory();
    QHash<QString, QImage> m_cachedEmotes;
    // Processing
    QVector<Twitch::Emote> m_emotesQueue;
    int m_processInterval;
    QTimer* m_processTimer;
    void scheduleProcessing();
    void processQueuedEmotes();
};

#endif // EMOTESCACHE_HPP
