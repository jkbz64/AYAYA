#include "emotescache.hpp"
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTimer>

namespace {
QString emotePrefix(const Twitch::Emote& emote)
{
    switch (emote.emoteType()) {
    case Twitch::EmoteType::TwitchEmotes:
        return QString("TwitchEmotes/");
        break;
    case Twitch::EmoteType::FFZ:
        return QString("FFZ/");
        break;
    case Twitch::EmoteType::BTTV:
        return QString("BTTV/");
        break;
    }
    return QString("");
}
qint64 bytesToMB(qint64 bytes)
{
    return bytes / 1000000;
}
JSON createEmotesDocument(const Twitch::Emotes& emotes)
{
    JSON document = JSON{};
    document["emotes"] = emotes;
    return document;
}
}

EmotesCache::EmotesCache(QObject* parent)
    : QObject(parent)
    , m_api(new Twitch::Api(this))
    , m_inited(false)
{
    connect(this, &EmotesCache::queuedEmotes, this, &EmotesCache::processQueuedEmotes, Qt::QueuedConnection);
    connect(this, &EmotesCache::loadedEmote, this, &EmotesCache::onLoadedEmote);
    ensureCacheDirectory();
}

EmotesCache::~EmotesCache()
{
}

QDir EmotesCache::ensureCacheDirectory()
{
    QDir cacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if (!cacheDirectory.exists()) {
        cacheDirectory.mkpath(".");
        cacheDirectory.mkdir("TwitchEmotes");
        cacheDirectory.mkdir("BTTV");
        cacheDirectory.mkdir("FFZ");
    }
    return cacheDirectory;
}

void EmotesCache::initCache()
{
    auto cacheDirectory = ensureCacheDirectory();
    if (!m_inited)
        emit startedInitingCache();
    // Init cache by loading global emotes
    loadGlobalEmotes();
}

void EmotesCache::clearCache()
{
    // Remove cache directory
    ensureCacheDirectory().removeRecursively();
    m_loadedEmotes.clear();
    m_inited = false;
    emit clearedCache();
}

void EmotesCache::loadEmotes(const QVector<Twitch::Emote>& emotes)
{
    m_emotesQueue << emotes;
    emit queuedEmotes();
}

void EmotesCache::loadGlobalEmotes()
{
    auto cacheDirectory = ensureCacheDirectory();
    QFile globalTwitchEmotes(cacheDirectory.absoluteFilePath("TwitchEmotes/global.json"));
    QFile globalBTTVEmotes(cacheDirectory.absoluteFilePath("BTTV/global.json"));
    QFile globalFFZEmotes(cacheDirectory.absoluteFilePath("FFZ/global.json"));

    if (globalTwitchEmotes.exists() && globalBTTVEmotes.exists() && globalFFZEmotes.exists()) {
        if (globalTwitchEmotes.open(QIODevice::ReadOnly) && globalBTTVEmotes.open(QIODevice::ReadOnly) && globalFFZEmotes.open(QIODevice::ReadOnly)) {
            Twitch::Emotes twitchEmotes = JSON::parse(globalTwitchEmotes.readAll())["emotes"];
            Twitch::Emotes bttvEmotes = JSON::parse(globalBTTVEmotes.readAll())["emotes"];
            Twitch::Emotes FFZEmotes = JSON::parse(globalFFZEmotes.readAll())["emotes"];
            m_emotesQueue << twitchEmotes;
            m_emotesQueue << bttvEmotes;
            m_emotesQueue << FFZEmotes;
            emit queuedEmotes();
        }
    } else {
        QFile::remove(cacheDirectory.absoluteFilePath("TwitchEmotes/global.json"));
        QFile::remove(cacheDirectory.absoluteFilePath("BTTV/global.json"));
        QFile::remove(cacheDirectory.absoluteFilePath("FFZ/global.json"));
        fetchGlobalEmotes();
    }

    if (!m_inited) {
        emit endedInitingCache();
        m_inited = true;
    }
}

void EmotesCache::loadChannelEmotes(const QString& channel)
{
    fetchChannelEmotes(channel);
}

bool EmotesCache::isEmoteLoaded(const QString& code)
{
    return m_loadedEmotes.contains(code);
}

void EmotesCache::onLoadedEmote(const QPair<Twitch::Emote, QImage>& pair)
{
    m_loadedEmotes.insert(pair.first.code());
}

void EmotesCache::processQueuedEmotes()
{
    auto cacheDirectory = ensureCacheDirectory();
    if (!m_emotesQueue.empty()) {
        const auto emote = m_emotesQueue.front();
        if (!isEmoteLoaded(emote.code())) {
            const auto prefix = emotePrefix(emote);
            const auto imagePath = cacheDirectory.absoluteFilePath(prefix + emote.id());
            // Check if emote's image exsits on disk
            if (QFile::exists(imagePath))
                emit loadedEmote(qMakePair(emote, QImage(imagePath)));
            else {
                // Download image and cache it
                auto imageReply = m_api->getImage(QString(emote.url()).replace("{{size}}", "1"));
                connect(imageReply, &Twitch::ImageReply::finished, [this, imageReply, imagePath, emote]() {
                    if (imageReply->currentState() == Twitch::ReplyState::Success) {
                        auto image = imageReply->data().value<QImage>();
                        QFile imageFile(imagePath + ".png");
                        if (imageFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
                            image.save(&imageFile, "PNG");
                        emit loadedEmote(qMakePair(emote, image));
                    } else {
                    }
                });
            }
        }
        // Pop currently processed emote from queue
        m_emotesQueue.pop_front();
        // Re-process until queue empty in non-blocking way
        if (!m_emotesQueue.empty())
            emit queuedEmotes();
    } else
        emit endedInitingCache();
}

void EmotesCache::fetchGlobalEmotes()
{
    emit startedFetchingGlobalEmotes();
    QDir cacheDirectory = ensureCacheDirectory();

    auto globalEmotesReply = m_api->getTwitchEmotesGlobalEmotes();
    auto bttvEmotesReply = m_api->getBTTVGlobalEmotes();
    auto ffzEmotesReply = m_api->getFFZGlobalEmotes();

    connect(globalEmotesReply, &Twitch::Reply::finished, [this, cacheDirectory, globalEmotesReply]() {
        if (globalEmotesReply->currentState() == Twitch::ReplyState::Success) {
            auto emotes = globalEmotesReply->emotes();
            QFile emotesFile(cacheDirectory.absoluteFilePath("TwitchEmotes/global.json"));
            if (emotesFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
                emotesFile.write(createEmotesDocument(emotes).dump().data());
            m_emotesQueue << emotes;
            emit queuedEmotes();
            emit fetchedGlobalEmotes();
        }
    });

    connect(bttvEmotesReply, &Twitch::Reply::finished, [this, cacheDirectory, bttvEmotesReply]() {
        if (bttvEmotesReply->currentState() == Twitch::ReplyState::Success) {
            auto emotes = bttvEmotesReply->emotes();
            QFile emotesFile(cacheDirectory.absoluteFilePath("BTTV/global.json"));
            if (emotesFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
                emotesFile.write(createEmotesDocument(emotes).dump().data());
            emotesFile.close();
            m_emotesQueue << emotes;
            emit queuedEmotes();
            emit fetchedGlobalEmotes();
        }
    });

    connect(bttvEmotesReply, &Twitch::Reply::downloadProgress, [this](qint64 current, qint64 total) {
        const QString totalString = total != -1 ? QString::number(bytesToMB(total)) + QString("MB") : QString("?");
        emit globalEmotesFetchProgress(EmotesBackend::BTTV, QString::number(bytesToMB(current)) + "MB", totalString);
    });

    connect(ffzEmotesReply, &Twitch::Reply::finished, [this, cacheDirectory, ffzEmotesReply]() {
        if (ffzEmotesReply->currentState() == Twitch::ReplyState::Success) {
            auto emotes = ffzEmotesReply->emotes();
            QFile emotesFile(cacheDirectory.absoluteFilePath("FFZ/global.json"));
            if (emotesFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
                emotesFile.write(createEmotesDocument(emotes).dump().data());
            emotesFile.close();
            m_emotesQueue << emotes;
            emit queuedEmotes();
            emit fetchedGlobalEmotes();
        }
    });

    connect(ffzEmotesReply, &Twitch::Reply::downloadProgress, [this](qint64 current, qint64 total) {
        const QString totalString = total != -1 ? QString::number(bytesToMB(total)) + QString("MB") : QString("?");
        emit globalEmotesFetchProgress(EmotesBackend::FFZ, QString::number(bytesToMB(current)) + "MB", totalString);
    });
}

void EmotesCache::fetchChannelEmotes(const QString& channel)
{
    auto cacheDirectory = ensureCacheDirectory();

    auto bttvEmotesReply = m_api->getBTTVSubscriberEmotesByChannel(channel);
    auto ffzEmotesReply = m_api->getFFZSubscriberEmotesByChannel(channel);

    connect(bttvEmotesReply, &Twitch::Reply::finished, [this, bttvEmotesReply]() {
        if (bttvEmotesReply->currentState() == Twitch::ReplyState::Success) {
            auto emotes = bttvEmotesReply->emotes();
            m_emotesQueue << emotes;
            emit queuedEmotes();
        }
    });

    connect(ffzEmotesReply, &Twitch::Reply::finished, [this, ffzEmotesReply]() {
        if (ffzEmotesReply->currentState() == Twitch::ReplyState::Success) {
            auto emotes = ffzEmotesReply->emotes();
            m_emotesQueue << emotes;
            emit queuedEmotes();
        }
    });
}
