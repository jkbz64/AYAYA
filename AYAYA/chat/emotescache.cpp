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
Twitch::Emotes getSubscriberEmotesForUser(const JSON& json, const QString& id)
{
    Twitch::Emotes emotes = json[id.toStdString()]["emotes"];
    return emotes;
}
}

EmotesCache::EmotesCache(QObject* parent)
    : QObject(parent)
    , m_api(new Twitch::Api(this))
    , m_inited(false)
{
    connect(this, &EmotesCache::queuedEmotes, this, &EmotesCache::processQueuedEmotes, Qt::QueuedConnection);
    connect(this, &EmotesCache::fetchedGlobalEmotes, this, &EmotesCache::loadGlobalEmotes);
    connect(this, &EmotesCache::loadedEmote, this, &EmotesCache::onLoadedEmote);
    ensureCacheDirectory();
}

EmotesCache::~EmotesCache()
{
}

void EmotesCache::initCache()
{
    auto cacheDirectory = ensureCacheDirectory();
    if (!m_inited)
        emit startedInitingCache();
    // Init cache by loading global emotes
    if (!loadGlobalEmotes()) {
        // Remove old global files
        QFile::remove(cacheDirectory.absoluteFilePath("TwitchEmotes/global.json"));
        QFile::remove(cacheDirectory.absoluteFilePath("BTTV/global.json"));
        QFile::remove(cacheDirectory.absoluteFilePath("FFZ/global.json"));
        // Subscriber emotes
        QFile::remove(cacheDirectory.absoluteFilePath("TwitchEmotes/subscriber.json"));

        // Fetch global emotes
        fetchGlobalEmotes();

        // FIXME Fetch and load subscriber json file into memory
        auto subscriberEmotesReply = m_api->getTwitchSubscriberEmotes();
        connect(subscriberEmotesReply, &Twitch::Reply::finished, this, [this, cacheDirectory, subscriberEmotesReply]() {
            if (subscriberEmotesReply->currentState() == Twitch::ReplyState::Success) {
                QFile subscriberFile(cacheDirectory.absoluteFilePath("TwitchEmotes/subscriber.json"));
                if (subscriberFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
                    subscriberFile.write(JSON(subscriberEmotesReply->emotes()).dump().data());
                subscriberFile.close();
            }
            subscriberEmotesReply->deleteLater();
            emit fetchedGlobalEmotes();
        });
        connect(subscriberEmotesReply, &Twitch::Reply::downloadProgress, [this](qint64 current, qint64 total) {
            const QString totalString = total != -1 ? QString::number(bytesToMB(total)) + QString("MB") : QString("?");
            emit globalEmotesFetchProgress(EmotesBackend::TwitchEmotes, QString::number(bytesToMB(current)) + "MB", totalString);
        });
    } else {
        QFile subscriberFile(cacheDirectory.absoluteFilePath("TwitchEmotes/subscriber.json"));
        if (subscriberFile.open(QIODevice::ReadOnly))
            m_globalSubscriberEmotesJSON = JSON::parse(subscriberFile.readAll());
    }
}

void EmotesCache::clearCache()
{
    // Remove cache directory
    ensureCacheDirectory().removeRecursively();
    m_loadedEmotes.clear();
    m_inited = false;
    emit clearedCache();
}

bool EmotesCache::isEmoteLoaded(const QString& code)
{
    return m_loadedEmotes.contains(code);
}

void EmotesCache::fetchGlobalEmotes()
{
    emit startedFetchingGlobalEmotes();
    QDir cacheDirectory = ensureCacheDirectory();

    auto globalEmotesReply = m_api->getGlobalEmotes();
    auto bttvEmotesReply = m_api->getBTTVGlobalEmotes();
    auto ffzEmotesReply = m_api->getFFZGlobalEmotes();

    connect(globalEmotesReply, &Twitch::Reply::finished, [this, cacheDirectory, globalEmotesReply]() {
        if (globalEmotesReply->currentState() == Twitch::ReplyState::Success) {
            auto emotes = globalEmotesReply->emotes();
            QFile emotesFile(cacheDirectory.absoluteFilePath("TwitchEmotes/global.json"));
            if (emotesFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
                emotesFile.write(createEmotesDocument(emotes).dump().data());
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

void EmotesCache::loadChannelEmotes(const Twitch::User& user)
{
    auto cacheDirectory = ensureCacheDirectory();

    // Load the TwitchEmotes from local cache
    if (m_globalSubscriberEmotesJSON.find(user.m_id.toStdString()) != m_globalSubscriberEmotesJSON.end()) {
        Twitch::Emotes channelEmotes = m_globalSubscriberEmotesJSON[user.m_id.toStdString()];
        m_emotesQueue << channelEmotes;
        emit queuedEmotes();
    }

    fetchChannelEmotes(user.m_login);
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

bool EmotesCache::loadGlobalEmotes()
{
    auto cacheDirectory = ensureCacheDirectory();
    QFile globalTwitchEmotes(cacheDirectory.absoluteFilePath("TwitchEmotes/global.json"));
    QFile globalBTTVEmotes(cacheDirectory.absoluteFilePath("BTTV/global.json"));
    QFile globalFFZEmotes(cacheDirectory.absoluteFilePath("FFZ/global.json"));

    QFile subsciberTwitchEmotes(cacheDirectory.absoluteFilePath("TwitchEmotes/subscriber.json"));

    if (globalTwitchEmotes.exists() && globalBTTVEmotes.exists() && globalFFZEmotes.exists() && subsciberTwitchEmotes.exists()) {
        if (globalTwitchEmotes.open(QIODevice::ReadOnly) && globalBTTVEmotes.open(QIODevice::ReadOnly) && globalFFZEmotes.open(QIODevice::ReadOnly)) {
            Twitch::Emotes twitchEmotes = JSON::parse(globalTwitchEmotes.readAll())["emotes"];
            Twitch::Emotes bttvEmotes = JSON::parse(globalBTTVEmotes.readAll())["emotes"];
            Twitch::Emotes FFZEmotes = JSON::parse(globalFFZEmotes.readAll())["emotes"];
            m_emotesQueue << twitchEmotes;
            m_emotesQueue << bttvEmotes;
            m_emotesQueue << FFZEmotes;
            emit queuedEmotes();
            if (!m_inited) {
                emit endedInitingCache();
                m_inited = true;
            }
        }
        return true;
    }
    return false;
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
                    if (imageFile.open(QIODevice::WriteOnly))
                        image.save(&imageFile, "PNG");
                    emit loadedEmote(qMakePair(emote, image));
                } else {
                    m_emotesQueue << emote;
                    emit queuedEmotes();
                }
            });
        }
        // Pop currently processed emote from queue
        m_emotesQueue.pop_front();
        // Re-process until queue empty in non-blocking way
        if (!m_emotesQueue.empty())
            emit queuedEmotes();
    } else
        emit endedInitingCache();
}
