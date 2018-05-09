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
    auto cacheDirectory = ensureCache();

    QFile cachedEmotesFile(cacheDirectory.absoluteFilePath("cachedEmotes.csv"));
    if (cachedEmotesFile.open(QIODevice::ReadOnly)) {
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
            scheduleProcessing();
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

void EmotesCache::clearCache()
{
}

bool EmotesCache::hasEmote(const QString& emoteCode)
{
    return m_cachedEmotes.find(emoteCode) != m_cachedEmotes.end();
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

void EmotesCache::scheduleProcessing()
{
    m_processTimer->setSingleShot(true);
    m_processTimer->setInterval(m_processInterval);
    if (!m_processTimer->isActive())
        m_processTimer->start();
}

void EmotesCache::processQueuedEmotes()
{
    // TODO I need to do some research on concurrent use of this (fetching emotes adds it to the same queue, might do some double-buffering)
    m_processTimer->stop();
    QDir cacheDirectory = ensureCache();
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
    }
    m_emotesQueue.clear();
    // Set interval just in case timer was started before processing all elements
    m_processTimer->setInterval(m_processInterval);
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
