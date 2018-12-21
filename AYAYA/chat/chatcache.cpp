#include "chatcache.hpp"
#include <QDirIterator>
#include <QImage>
#include <QStandardPaths>
#include <TwitchQt/Twitch>

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

ChatCache::ChatCache(QObject* parent)
    : QObject(parent)
    , m_api(new Twitch::Api(this))
    , m_inited(false)
{
    connect(this, &ChatCache::queuedEmotes, this, &ChatCache::processQueuedEmotes, Qt::QueuedConnection);
    connect(this, &ChatCache::loadedEmote, this, &ChatCache::onLoadedEmote);
    ensureCacheDirectory();
}

ChatCache::~ChatCache()
{
}

QDir ChatCache::ensureCacheDirectory()
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

void ChatCache::initCache()
{
    auto cacheDirectory = ensureCacheDirectory();
    if (!m_inited)
        emit startedInitingCache();
    // Init cache by loading global emotes and global chat badges
    loadGlobalEmotes();
    loadGlobalBadges();
}

void ChatCache::clearCache()
{
    // Remove cache directory
    ensureCacheDirectory().removeRecursively();
    m_loadedEmotes.clear();
    m_inited = false;
    emit clearedCache();

    loadGlobalEmotes();
}

void ChatCache::processQueuedEmotes()
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
                const auto imageReply = m_api->getImage(QString(emote.url()).replace("{{size}}", "1"));
                connect(imageReply, &Twitch::ImageReply::finished, [this, imageReply, imagePath, emote]() {
                    if (imageReply->currentState() == Twitch::ReplyState::Success) {
                        auto image = imageReply->data().value<QImage>();
                        QFile imageFile(imagePath + ".png");
                        if (imageFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
                            image.save(&imageFile, "PNG");
                        emit loadedEmote(qMakePair(emote, image));
                    }
                    imageReply->deleteLater();
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

// Loads
void ChatCache::loadEmotes(const QVector<Twitch::Emote>& emotes)
{
    m_emotesQueue << emotes;
    emit queuedEmotes();
}

void ChatCache::loadGlobalEmotes()
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

void ChatCache::loadGlobalBadges()
{
    auto badgesReply = m_api->getGlobalBadges();

    connect(badgesReply, &Twitch::Reply::finished, [this, badgesReply]() {
        if (badgesReply->currentState() == Twitch::ReplyState::Success) {
            for (const auto& badge : badgesReply->badges()) {
                for (auto version = badge.m_versions.keyValueBegin(); version != badge.m_versions.keyValueEnd(); version++) {
                    auto imageReply = m_api->getImage((*version).second.m_imageUrl);
                    connect(imageReply, &Twitch::Reply::finished, [this, imageReply, versionName = (*version).first, badge]() {
                        if (imageReply->currentState() == Twitch::ReplyState::Success) {
                            emit loadedBadge(qMakePair(badge, qMakePair(versionName, imageReply->data().value<QImage>())));
                        }
                    });
                }
            }
        }
        badgesReply->deleteLater();
    });
}

void ChatCache::loadChannelEmotes(const QString& channel)
{
    fetchChannelEmotes(channel);
}

void ChatCache::loadChannelBadges(const QString& channel)
{
    auto channelBadgesReply = m_api->getChannelBadges(channel);
    connect(channelBadgesReply, &Twitch::Reply::finished, [this, channelBadgesReply]() {
        if (channelBadgesReply->currentState() == Twitch::ReplyState::Success) {
            if (channelBadgesReply->badges().empty())
                loadGlobalBadges();
            else {
                for (const auto& badge : channelBadgesReply->badges()) {
                    for (auto version = badge.m_versions.keyValueBegin(); version != badge.m_versions.keyValueEnd(); version++) {
                        auto imageReply = m_api->getImage((*version).second.m_imageUrl);
                        connect(imageReply, &Twitch::Reply::finished, [this, imageReply, versionName = (*version).first, badge]() {
                            if (imageReply->currentState() == Twitch::ReplyState::Success) {
                                emit loadedBadge(qMakePair(badge, qMakePair(versionName, imageReply->data().value<QImage>())));
                            }
                        });
                    }
                }
            }
        }
    });
}

bool ChatCache::isEmoteLoaded(const QString& code)
{
    return m_loadedEmotes.contains(code);
}
// Fetches
void ChatCache::fetchGlobalEmotes()
{
    emit startedFetchingGlobalEmotes();
    QDir cacheDirectory = ensureCacheDirectory();

    const auto globalEmotesReply = m_api->getTwitchEmotesGlobalEmotes();
    const auto bttvEmotesReply = m_api->getBTTVGlobalEmotes();
    const auto ffzEmotesReply = m_api->getFFZGlobalEmotes();

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
        globalEmotesReply->deleteLater();
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
        bttvEmotesReply->deleteLater();
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
        ffzEmotesReply->deleteLater();
    });

    connect(ffzEmotesReply, &Twitch::Reply::downloadProgress, [this](qint64 current, qint64 total) {
        const QString totalString = total != -1 ? QString::number(bytesToMB(total)) + QString("MB") : QString("?");
        emit globalEmotesFetchProgress(EmotesBackend::FFZ, QString::number(bytesToMB(current)) + "MB", totalString);
    });
}

void ChatCache::fetchChannelEmotes(const QString& channel)
{
    auto cacheDirectory = ensureCacheDirectory();

    const auto bttvEmotesReply = m_api->getBTTVSubscriberEmotesByChannel(channel);
    const auto ffzEmotesReply = m_api->getFFZSubscriberEmotesByChannel(channel);

    connect(bttvEmotesReply, &Twitch::Reply::finished, [this, bttvEmotesReply]() {
        if (bttvEmotesReply->currentState() == Twitch::ReplyState::Success) {
            auto emotes = bttvEmotesReply->emotes();
            m_emotesQueue << emotes;
            emit queuedEmotes();
        }
        bttvEmotesReply->deleteLater();
    });

    connect(ffzEmotesReply, &Twitch::Reply::finished, [this, ffzEmotesReply]() {
        if (ffzEmotesReply->currentState() == Twitch::ReplyState::Success) {
            auto emotes = ffzEmotesReply->emotes();
            m_emotesQueue << emotes;
            emit queuedEmotes();
        }
        ffzEmotesReply->deleteLater();
    });
}

// Slots
void ChatCache::onLoadedEmote(const QPair<Twitch::Emote, QImage>& pair)
{
    m_loadedEmotes.insert(pair.first.code());
}

void ChatCache::onLoadedBadge(const QPair<Twitch::Badge, QPair<QString, QImage>>& badge)
{
}
