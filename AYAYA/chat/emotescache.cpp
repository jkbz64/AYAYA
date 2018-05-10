#include "emotescache.hpp"
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTimer>

namespace {
QString emotePrefix(const Twitch::Emote& emote)
{
    switch (emote.m_type) {
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
Twitch::Emote::EmoteType prefixToEmoteType(const QString& prefix)
{
    if (prefix == "TwitchEmotes/")
        return Twitch::Emote::EmoteType::TwitchEmotes;
    else if (prefix == "FFZ/")
        return Twitch::Emote::EmoteType::FFZ;
    else if (prefix == "BTTV/")
        return Twitch::Emote::EmoteType::BTTV;
    throw std::runtime_error("Invalid prefix");
}
QString emoteTypeToUrl(Twitch::Emote::EmoteType type)
{
    switch (type) {
    case Twitch::Emote::EmoteType::TwitchEmotes:
        return QString(Twitch::TwitchEmotes::Emote::urlTemplate());
        break;
    case Twitch::Emote::EmoteType::BTTV:
        return QString(Twitch::BTTV::Emote::urlTemplate());
        break;
    case Twitch::Emote::EmoteType::FFZ:
        return QString(Twitch::FFZ::Emote::urlTemplate());
        break;
    }
}
QString emoteExtension(const Twitch::Emote& emote)
{
    switch (emote.m_imageType) {
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
    switch (emote.m_imageType) {
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
    , m_processInterval(500)
    , m_processTimer(new QTimer(this))
{
    connect(m_processTimer, &QTimer::timeout, this, &EmotesCache::processQueuedEmotes);
    ensureCache();
}

EmotesCache::~EmotesCache()
{
}

void EmotesCache::initCache()
{
    emit startedInitingCache();
    auto cacheDirectory = ensureCache();

    // We're using new QFile because m_cacheFile's buffor might be at the end of the file
    QFile cachedEmotesFile(cacheDirectory.absoluteFilePath("cachedEmotes.csv"));
    if (cachedEmotesFile.open(QIODevice::ReadOnly)) {
        int emoteCount = 1;
        while (!cachedEmotesFile.atEnd()) {
            auto emotePair = cachedEmotesFile.readLine().split(',');
            QString emotePath = emotePair.first();
            QString prefix = emotePath.split('/').at(0) + "/";
            QString id = emotePath.split('/').at(1);
            QString code = emotePair.last().simplified();
            const auto emoteType = prefixToEmoteType(prefix);
            m_emotesQueue << Twitch::Emote{
                emoteType,
                id,
                code,
                Twitch::Emote::ImageType::PNG,
                emoteTypeToUrl(emoteType).replace("{{id}}", id)
            };
            emit initProgress(emoteCount++);
        }
    }
    scheduleProcessing();
    emit endedInitingCache();
}

void EmotesCache::clearCache()
{
    m_cachedEmotes.clear();
    if (m_cacheFile.isOpen())
        m_cacheFile.close();
    m_cacheFile.open(QFile::WriteOnly, QFile::Truncate);
    m_cacheFile.close();
}

bool EmotesCache::hasEmote(const QString& emoteCode)
{
    return m_cachedEmotes.find(emoteCode) != m_cachedEmotes.end();
}

void EmotesCache::fetchGlobalEmotes()
{
    auto globalEmotesReply = m_api->getGlobalEmotes();
    auto bttvEmotesReply = m_api->getBTTVGlobalEmotes();
    auto ffzEmotesReply = m_api->getFFZGlobalEmotes();

    connect(globalEmotesReply, &Twitch::Reply::finished, [this, globalEmotesReply]() {
        auto emotes = globalEmotesReply->toEmotes();
        m_emotesQueue << emotes;

        /*{ // Subscribers emotes fetching
            auto subscribersEmotesReply = m_api->getTwitchSubscriberEmotes();
            connect(subscribersEmotesReply, &Twitch::Reply::finished, [this, subscribersEmotesReply]() {
                auto emotes = subscribersEmotesReply->toEmotes();
                m_emotesQueue << emotes;
                scheduleProcessing();
                // I know, in real scenario we won't know which file is the biggest but in this case come on...
                // This file has over 100MB...
                emit fetchedGlobalEmotes();
            });

            connect(subscribersEmotesReply, &Twitch::Reply::downloadProgress, [this](qint64 current, qint64 total) {
                const QString totalString = total != -1 ? QString::number(bytesToMB(total)) + QString("MB") : QString("?");
                emit globalEmotesFetchProgress(EmotesBackend::TwitchEmotes, QString::number(bytesToMB(current)) + "MB", totalString);
            });
        }*/
        emit fetchedGlobalEmotes();
    });

    connect(globalEmotesReply, &Twitch::Reply::downloadProgress, [this](qint64 current, qint64 total) {
        const QString totalString = total != -1 ? QString::number(bytesToMB(total)) + QString("MB") : QString("?");
        emit globalEmotesFetchProgress(EmotesBackend::TwitchEmotes, QString::number(bytesToMB(current)) + "MB", totalString);
    });

    connect(bttvEmotesReply, &Twitch::Reply::finished, [this, bttvEmotesReply]() {
        auto emotes = bttvEmotesReply->toEmotes();
        m_emotesQueue << emotes;
        scheduleProcessing();
    });

    connect(bttvEmotesReply, &Twitch::Reply::downloadProgress, [this](qint64 current, qint64 total) {
        const QString totalString = total != -1 ? QString::number(bytesToMB(total)) + QString("MB") : QString("?");
        emit globalEmotesFetchProgress(EmotesBackend::BTTV, QString::number(bytesToMB(current)) + "MB", totalString);
    });

    connect(ffzEmotesReply, &Twitch::Reply::finished, [this, ffzEmotesReply]() {
        auto emotes = ffzEmotesReply->toEmotes();
        m_emotesQueue << emotes;
        scheduleProcessing();
    });

    connect(ffzEmotesReply, &Twitch::Reply::downloadProgress, [this](qint64 current, qint64 total) {
        const QString totalString = total != -1 ? QString::number(bytesToMB(total)) + QString("MB") : QString("?");
        emit globalEmotesFetchProgress(EmotesBackend::FFZ, QString::number(bytesToMB(current)) + "MB", totalString);
    });

    emit startedFetchingGlobalEmotes();
}

void EmotesCache::fetchChannelEmotes(const QString& channel)
{
    auto bttvEmotesReply = m_api->getBTTVSubscriberEmotesByChannel(channel);
    auto ffzEmotesReply = m_api->getFFZSubscriberEmotesByChannel(channel);

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

void EmotesCache::forceProcessing()
{
    processQueuedEmotes();
}

void EmotesCache::scheduleProcessing()
{
    m_processTimer->setSingleShot(true);
    m_processTimer->setInterval(m_processInterval);
    if (!m_processTimer->isActive())
        m_processTimer->start();
}

void EmotesCache::processQueuedEmotes()
{
    emit startedProcessing();
    // TODO I need to do some research on concurrent use of this (fetching emotes adds it to the same queue, might do some double-buffering)
    QDir cacheDirectory = ensureCache();
    m_processTimer->stop();

    int currentEmote = 1;
    for (const auto& emote : m_emotesQueue) {
        if (!isCached(emote)) {
            const QString url = QString(emote.m_url).replace("{{size}}", "1");
            auto imageReply = m_api->getImage(url);
            connect(imageReply, &Twitch::Reply::finished, [this, imageReply, emote]() {
                if (imageReply->currentState() == Twitch::ReplyState::Success)
                    cacheEmote(emote, imageReply->data().value<QImage>());
            });
        } else {
            if (!hasEmote(emote.m_code)) {
                // Load from cache
                QImage image(cacheDirectory.absoluteFilePath(emotePrefix(emote) + emote.m_id + ".png"));
                cacheEmote(emote, image);
            }
        }
        emit processProgress(currentEmote++, m_emotesQueue.size() - 1);
    }
    m_emotesQueue.clear();
    // Set interval just in case timer was started before processing all elements
    m_processTimer->setInterval(m_processInterval);
    emit endedProcessing();
}

QDir EmotesCache::ensureCache()
{
    QDir cacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if (!cacheDirectory.exists()) {
        cacheDirectory.mkpath(".");
        cacheDirectory.mkdir("TwitchEmotes");
        cacheDirectory.mkdir("BTTV");
        cacheDirectory.mkdir("FFZ");
    }

    if (!m_cacheFile.isOpen()) {
        m_cacheFile.setFileName(cacheDirectory.absoluteFilePath("cachedEmotes.csv"));
        m_cacheFile.open(QIODevice::ReadWrite | QIODevice::Append);
    }

    return cacheDirectory;
}

bool EmotesCache::isCached(const Twitch::Emote& emote)
{
    const auto cacheDirectory = ensureCache();
    return cacheDirectory.exists(cacheDirectory.absoluteFilePath(emotePrefix(emote) + emote.m_id + emoteExtension(emote)));
}

void EmotesCache::cacheEmote(const Twitch::Emote& emote, const QImage& image)
{
    const bool isAlreadyCached = m_cachedEmotes.insert(emote.m_code, image) == m_cachedEmotes.end();
    if (!isAlreadyCached)
        emit emoteCached(qMakePair(emote, image));
    // Cache image if it's not gif
    if (emote.m_imageType != Twitch::Emote::ImageType::GIF && !isCached(emote)) {
        appendEmoteToCacheFile(emote);
        const auto cacheDirectory = ensureCache();
        QFile imageFile(cacheDirectory.absoluteFilePath(emotePrefix(emote) + emote.m_id + emoteExtension(emote)));
        if (imageFile.open(QIODevice::WriteOnly)) {
            switch (emote.m_imageType) {
            case Twitch::Emote::ImageType::PNG:
                image.save(&imageFile, "PNG");
                break;
            case Twitch::Emote::ImageType::GIF:
                // image.save(&imageFile, "GIF"); It seems Qt does not support saving gif so we will be loading them
                break;
            }
        }
    }
}

void EmotesCache::appendEmoteToCacheFile(const Twitch::Emote& emote)
{
    if (!m_cacheFile.isOpen())
        ensureCache();
    m_cacheFile.write(QString(emotePrefix(emote) + emote.m_id + "," + emote.m_code + "\n").toUtf8());
}
