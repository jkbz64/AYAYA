#include "chatview.hpp"
#include "chatclient.hpp"
#include "emotescache.hpp"
#include <QTextBlock>

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
    append(message);
}

void ChatView::onJoinedChannel(const QString& channel)
{
    reset();

    auto frameFormat = document()->rootFrame()->frameFormat();
    frameFormat.setBottomMargin(20);
    document()->rootFrame()->setFrameFormat(frameFormat);

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
            editedMessage.replace(word, "<img src=\"" + word + "\" align=\"middle\"></img>");
        }
    }
    addMessage("<p><b><font color=\"" + message.m_color.name() + "\">" + message.m_author + "</font></b>: " + editedMessage + "</p>");

    auto b = document()->lastBlock().blockFormat();
    b.setLineHeight(30, QTextBlockFormat::FixedHeight);

    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::KeepAnchor);
    cursor.setBlockFormat(b);

    if (document()->blockCount() > 50) { // Keep only the 50 newest comments
        QTextCursor eraseCursor(document());
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::NextBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
}

void ChatView::onEmoteLoaded(const QPair<Twitch::Emote, QImage>& emote)
{
    document()->addResource(QTextDocument::ImageResource, QUrl(emote.first.code()), QVariant(emote.second));
}

void ChatView::reset()
{
    document()->setHtml("");
}
