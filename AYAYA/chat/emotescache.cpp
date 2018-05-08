#include "emotescache.hpp"
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTimer>

namespace {
QString emoteTypeToString(Twitch::Emote::EmoteType type)
{
    switch (type) {
    case Twitch::Emote::EmoteType::TwitchEmotes:
        return QString("TwitchEmotes/");
        break;
    case Twitch::Emote::EmoteType::FFZ:
        return QString("FFZ/");
        break;
    case Twitch::Emote::EmoteType::BTTV:
        return QString("BTTV/");
        break;
    }
    return QString("");
}
}

EmotesCache::EmotesCache(QObject* parent)
    : QObject(parent)
    , m_api(new Twitch::Api(this))
    , m_processInterval(500)
    , m_processTimer(new QTimer(this))
{
    connect(m_processTimer, &QTimer::timeout, this, &EmotesCache::processQueuedEmotes);
    validateCacheDirectory();
}

EmotesCache::~EmotesCache()
{
}

bool EmotesCache::validateCacheDirectory()
{
    QDir cacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    bool exists = cacheDirectory.exists();
    if (!exists) {
        cacheDirectory.mkpath(".");
        cacheDirectory.mkdir("TwitchEmotes");
        cacheDirectory.mkdir("BTTV");
        cacheDirectory.mkdir("FFZ");
    }
    QFile cachedEmotesFile(cacheDirectory.absoluteFilePath("cachedEmotes.csv"));
    if (!cachedEmotesFile.exists()) {
        cachedEmotesFile.open(QIODevice::WriteOnly);
        cachedEmotesFile.close();
    }

    if (!cachedEmotesFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(0, "Warning", R"(Failed to write cache file.
              AYAYA might not have permissions to write on path, emotes won't probably work. )"
                + QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    }

    return exists;
}

void EmotesCache::initCache()
{
    QDir cacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    QFile cachedEmotesFile(cacheDirectory.absoluteFilePath("cachedEmotes.csv"));
    if (cachedEmotesFile.open(QIODevice::ReadOnly)) {
        while (!cachedEmotesFile.atEnd()) {
            auto emotePair = cachedEmotesFile.readLine().split(',');
            auto emotePath = emotePair.first();
            auto code = emotePair.last().simplified();
            emit addedEmote(qMakePair(code, QImage(cacheDirectory.absoluteFilePath(emotePath) + ".png")));
        }
    }

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

void EmotesCache::fetchChannelEmotes(const QString& channel)
{
    // FIXME fill it when we know the endpoint
    //auto twitchEmotesReply = m_api->getTwitchSubscriberEmotesByChannel(channel);
    auto bttvEmotesReply = m_api->getBTTVSubscriberEmotesByChannel(channel);
    auto ffzEmotesReply = m_api->getFFZSubscriberEmotesByChannel(channel);

    /*connect(twitchEmotesReply, &Twitch::Reply::finished, [this, twitchEmotesReply]() {
        auto emotes = twitchEmotesReply->toEmotes();
        //m_emotesQueue << emotes;
        scheduleProcessing();
    });*/

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

void EmotesCache::processQueuedEmotes()
{
    qDebug() << "processing";
    QDir cacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    QFile cachedEmotesFile(cacheDirectory.absoluteFilePath("cachedEmotes.csv"));
    if (cachedEmotesFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        for (const auto& emote : m_emotesQueue) {
            QString prefix = emoteTypeToString(emote.m_type);
            if (!cacheDirectory.exists(prefix + emote.m_id + QString(".png"))) {
                QString url = emote.m_url;
                url = url.replace("{{id}}", emote.m_id).replace("{{size}}", "1");
                auto imageReply = m_api->getImage(url);
                connect(imageReply, &Twitch::Reply::finished, [this, imageReply, prefix, emote, cacheDirectory]() {
                    QFile imageFile(cacheDirectory.absolutePath() + "/" + prefix + emote.m_id + ".png");
                    imageFile.open(QIODevice::WriteOnly);
                    auto image = imageReply->data().value<QImage>();
                    image.save(&imageFile, "PNG");
                    emit addedEmote(qMakePair(emote.m_code, image));
                });
                cachedEmotesFile.write(QString(prefix + emote.m_id + "," + emote.m_code + "\n").toUtf8());
            }
        }
    }
    m_emotesQueue.clear();
}

void EmotesCache::scheduleProcessing()
{
    m_processTimer->setSingleShot(true);
    m_processTimer->setInterval(m_processInterval);
    if (!m_processTimer->isActive())
        m_processTimer->start();
}
