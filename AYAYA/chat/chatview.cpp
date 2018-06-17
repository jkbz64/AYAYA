#include "chatview.hpp"
#include "chatclient.hpp"
#include "emotescache.hpp"

ChatView::ChatView(QWidget* parent)
    : QTextBrowser(parent)
    , m_emotesCache(new EmotesCache(this))
{
    connect(emotesCache(), &EmotesCache::loadedEmote, this, &ChatView::onEmoteLoaded);
}

ChatView::~ChatView()
{
}

EmotesCache* ChatView::emotesCache() const
{
    return m_emotesCache;
}

void ChatView::addMessage(const QString& message)
{
    append(message + "\n");
}

void ChatView::onJoinedChannel(const QString& channel)
{
    reset();
    emotesCache()->loadChannelEmotes(channel);
    append("You've joined #" + channel + " channel.");
}

void ChatView::onMessageReceived(const TwitchMessage& message)
{
    auto editedMessage = message.m_content;
    emotesCache()->loadEmotes(message.m_emotes);

    QSet<QString> preCodes;
    for (const auto& emote : message.m_emotes)
        preCodes.insert(emote.code());

    const auto words = QSet<QString>::fromList(message.m_content.split(QRegExp("[\r\n\t ]+")));
    for (const auto& word : words) {
        if (emotesCache()->isEmoteLoaded(word.simplified()) || preCodes.find(word.simplified()) != preCodes.end()) {
            editedMessage.replace(word, "<img src=\"" + word + "\" align=\"middle\"/>");
        }
    }
    addMessage("<b>" + message.m_author + "</b>: " + editedMessage);
}

void ChatView::onEmoteLoaded(const QPair<Twitch::Emote, QImage>& emote)
{
    document()->addResource(QTextDocument::ImageResource, QUrl(emote.first.code()), QVariant(emote.second));
}

void ChatView::reset()
{
    document()->setHtml("");
}
