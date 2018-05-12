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
Twitch::EmoteType prefixToEmoteType(const QString& prefix)
{
    if (prefix == "TwitchEmotes/")
        return Twitch::EmoteType::TwitchEmotes;
    else if (prefix == "FFZ/")
        return Twitch::EmoteType::FFZ;
    else if (prefix == "BTTV/")
        return Twitch::EmoteType::BTTV;
    throw std::runtime_error("Invalid prefix");
}
QString emoteTypeToUrl(Twitch::EmoteType type)
{
    switch (type) {
    case Twitch::EmoteType::TwitchEmotes:
        return QString(Twitch::TwitchEmotes::Emote::urlTemplate());
        break;
    case Twitch::EmoteType::BTTV:
        return QString(Twitch::BTTV::Emote::urlTemplate());
        break;
    case Twitch::EmoteType::FFZ:
        return QString(Twitch::FFZ::Emote::urlTemplate());
        break;
    }
}
QString emoteExtension(const Twitch::Emote& emote)
{
    switch (emote.imageType()) {
    case Twitch::Emote::ImageType::PNG:
        return QString(".png");
        break;
    case Twitch::Emote::ImageType::GIF:
        return QString(".gif");
        break;
    }
    return QString("");
}
void saveEmoteImage(const QImage& image, QFile& imageFile, const Twitch::Emote& emote)
{
    switch (emote.imageType()) {
    case Twitch::Emote::ImageType::PNG:
        image.save(&imageFile, "PNG");
        break;
    case Twitch::Emote::ImageType::GIF:
        // image.save(&imageFile, "GIF"); It seems Qt does not support saving gif so we will be loading them
        break;
    }
}
qint64 bytesToMB(qint64 bytes)
{
    return bytes / 1000000;
}
}

EmotesCache::EmotesCache(QObject* parent)
    : QObject(parent)
    , m_api(new Twitch::Api(this))
    , m_inited(false)
{
    //connect(this, &EmotesCache::queuedEmotes, this, &EmotesCache::processQueuedEmotes, Qt::QueuedConnection);
    connect(this, &EmotesCache::fetchedGlobalEmotes, this, &EmotesCache::loadGlobalEmotes, Qt::UniqueConnection);
    ensureCacheDirectory();
}

EmotesCache::~EmotesCache()
{
}

void EmotesCache::initCache()
{
    if (!m_inited)
        emit startedInitingCache();
    // Init cache by loading global emotes
    if (!loadGlobalEmotes())
        fetchGlobalEmotes();
}

void EmotesCache::fetchGlobalEmotes()
{
    emit startedFetchingGlobalEmotes();
    QDir cacheDirectory = ensureCacheDirectory();

    // Remove old global files
    QFile::remove(cacheDirectory.absoluteFilePath("TwitchEmotes/global.json"));
    QFile::remove(cacheDirectory.absoluteFilePath("BTTV/global.json"));
    QFile::remove(cacheDirectory.absoluteFilePath("FFZ/global.json"));

    auto globalEmotesReply = m_api->getGlobalEmotes();
    auto bttvEmotesReply = m_api->getBTTVGlobalEmotes();
    auto ffzEmotesReply = m_api->getFFZGlobalEmotes();

    connect(globalEmotesReply, &Twitch::Reply::finished, [this, cacheDirectory, globalEmotesReply]() {
        if (globalEmotesReply->currentState() == Twitch::ReplyState::Success) {
            auto emotes = globalEmotesReply->emotes();
            QFile emotesFile(cacheDirectory.absoluteFilePath("TwitchEmotes/global.json"));
            if (emotesFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
                emotesFile.write(JSON(emotes).dump().data());
            emit fetchedGlobalEmotes();
        }
    });

    connect(globalEmotesReply, &Twitch::Reply::downloadProgress, [this](qint64 current, qint64 total) {
        const QString totalString = total != -1 ? QString::number(bytesToMB(total)) + QString("MB") : QString("?");
        emit globalEmotesFetchProgress(EmotesBackend::TwitchEmotes, QString::number(bytesToMB(current)) + "MB", totalString);
    });

    connect(bttvEmotesReply, &Twitch::Reply::finished, [this, cacheDirectory, bttvEmotesReply]() {
        if (bttvEmotesReply->currentState() == Twitch::ReplyState::Success) {
            auto emotes = bttvEmotesReply->emotes();
            QFile emotesFile(cacheDirectory.absoluteFilePath("BTTV/global.json"));
            if (emotesFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
                emotesFile.write(JSON(emotes).dump().data());
            emotesFile.close();
            loadGlobalEmotes();
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
                emotesFile.write(JSON(emotes).dump().data());
            emotesFile.close();
            loadGlobalEmotes();
        }
    });

    connect(ffzEmotesReply, &Twitch::Reply::downloadProgress, [this](qint64 current, qint64 total) {
        const QString totalString = total != -1 ? QString::number(bytesToMB(total)) + QString("MB") : QString("?");
        emit globalEmotesFetchProgress(EmotesBackend::FFZ, QString::number(bytesToMB(current)) + "MB", totalString);
    });
}

void EmotesCache::fetchChannelEmotes(const QString& channel)
{
    auto bttvEmotesReply = m_api->getBTTVSubscriberEmotesByChannel(channel);
    auto ffzEmotesReply = m_api->getFFZSubscriberEmotesByChannel(channel);

    connect(bttvEmotesReply, &Twitch::Reply::finished, [this, bttvEmotesReply]() {
        auto emotes = bttvEmotesReply->emotes();
        m_emotesQueue << emotes;
    });

    connect(ffzEmotesReply, &Twitch::Reply::finished, [this, ffzEmotesReply]() {
        auto emotes = ffzEmotesReply->emotes();
        m_emotesQueue << emotes;
    });
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
    if (globalTwitchEmotes.exists() && globalBTTVEmotes.exists() && globalFFZEmotes.exists()) {
        if (globalTwitchEmotes.open(QIODevice::ReadOnly) && globalBTTVEmotes.open(QIODevice::ReadOnly) && globalFFZEmotes.open(QIODevice::ReadOnly)) {
            Twitch::Emotes twitchEmotes = JSON::parse(globalTwitchEmotes.readAll());
            Twitch::Emotes bttvEmotes = JSON::parse(globalBTTVEmotes.readAll());
            Twitch::Emotes FFZEmotes = JSON::parse(globalFFZEmotes.readAll());
            m_emotesQueue << twitchEmotes << bttvEmotes << FFZEmotes;
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

void EmotesCache::loadEmotes(const Twitch::Emotes&)
{
}

void EmotesCache::clearCache()
{
    // TODO
}
