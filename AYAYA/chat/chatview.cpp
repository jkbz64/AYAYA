#include "chatview.hpp"
#include "chatcache.hpp"
#include "chatclient.hpp"
#include <QTextBlock>

ChatView::ChatView(QWidget* parent)
    : QTextBrowser(parent)
    , m_cache(new ChatCache(this))
{
    connect(cache(), &ChatCache::loadedBadge, this, &ChatView::onBadgeLoaded);
    connect(cache(), &ChatCache::loadedEmote, this, &ChatView::onEmoteLoaded);
}

ChatView::~ChatView()
{
}

ChatCache* ChatView::cache() const
{
    return m_cache;
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

    append("You've joined #" + channel + " channel.");
}

#include <QDebug>

void ChatView::onMessageReceived(const TwitchMessage& message)
{
    auto editedMessage = message.m_content;

    QString badges = QString("");
    for (const auto& badge : message.m_badges)
        badges += "<img src=\"" + badge + "\" align=\"middle\"></img> ";
    cache()->loadEmotes(message.m_emotes);

    QSet<QString> preCodes;
    for (const auto& emote : message.m_emotes)
        preCodes.insert(emote.code());

    const auto words = QSet<QString>::fromList(message.m_content.split(QRegExp("[\r\n\t ]+")));
    for (const auto& word : words) {
        if (cache()->isEmoteLoaded(word.simplified()) || preCodes.find(word.simplified()) != preCodes.end()) {
            editedMessage.replace(word, "<img src=\"" + word + "\" align=\"middle\"></img>");
        }
    }

    addMessage("<p>" + badges + "<b><font color=\"" + message.m_color.name() + "\">" + message.m_author + "</font></b>: " + editedMessage + "</p>");

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

void ChatView::onBadgeLoaded(const QPair<Twitch::Badge, QPair<QString, QImage>>& badge)
{
    document()->addResource(QTextDocument::ImageResource, QUrl(badge.first.m_name + "/" + badge.second.first), QVariant(badge.second.second));
}

void ChatView::reset()
{
    document()->setHtml("");
}
