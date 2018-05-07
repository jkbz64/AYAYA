#include "emotescache.hpp"
#include <QDir>
#include <QStandardPaths>
#include <QTimer>

EmotesCache::EmotesCache(QObject* parent)
    : QObject(parent)
    , m_api(new Twitch::Api(this))
    , m_processInterval(500)
    , m_processTimer(new QTimer(this))
{
    connect(m_processTimer, &QTimer::timeout, this, &EmotesCache::processQueuedEmotes);
    initCache();
}

EmotesCache::~EmotesCache()
{
}

bool EmotesCache::cacheDirectoryExists()
{
    QDir cacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    bool exists = cacheDirectory.exists();
    if (!exists)
        cacheDirectory.mkpath(".");
    return exists;
}

void EmotesCache::initCache()
{
    auto globalEmotesReply = m_api->getGlobalEmotes();
    auto bttvEmotesReply = m_api->getBTTVGlobalEmotes();
    auto ffzEmotesReply = m_api->getFFZGlobalEmotes();

    connect(globalEmotesReply, &Twitch::Reply::finished, [this, globalEmotesReply]() {
        auto emotes = globalEmotesReply->toEmotes();
        m_emotesQueue << emotes;
        scheduleProcessing();
    });

    connect(bttvEmotesReply, &Twitch::Reply::finished, [this, bttvEmotesReply]() {
        auto emotes = bttvEmotesReply->toEmotes();
        m_emotesQueue << emotes;
        scheduleProcessing();
    });

    connect(ffzEmotesReply, &Twitch::Reply::finished, [this, ffzEmotesReply]() {
        auto emotes = ffzEmotesReply->toEmotes();
        m_emotesQueue << emotes;
        scheduleProcessing();
    });
}

void EmotesCache::scheduleProcessing()
{
    if (!m_processTimer->isActive()) {
        m_processTimer->setSingleShot(true);
        m_processTimer->setInterval(m_processInterval);
        m_processTimer->start();
    }
}

void EmotesCache::processQueuedEmotes()
{
    QDir cacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    for (const auto& emote : m_emotesQueue) {
        if (!cacheDirectory.exists(emote.m_id + QString(".png"))) {
            QString url = emote.m_url;
            url = url.replace("{{id}}", emote.m_id).replace("{{size}}", "1");
            auto imageReply = m_api->getImage(url);
            connect(imageReply, &Twitch::Reply::finished, [this, imageReply, emote, cacheDirectory]() {
                QFile imageFile(cacheDirectory.absolutePath() + "/" + emote.m_id + ".png");
                imageFile.open(QIODevice::WriteOnly);
                auto image = imageReply->data().value<QImage>();
                image.save(&imageFile, "PNG");
                emit addedEmote(qMakePair(emote.m_code, image));
            });
        }
    }
    m_emotesQueue.clear();
}
