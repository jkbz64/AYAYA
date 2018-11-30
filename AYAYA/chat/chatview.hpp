#ifndef CHATVIEW_HPP
#define CHATVIEW_HPP

#include <QPair>
#include <QTextBrowser>

namespace Twitch {
class Emote;
class Badge;
class Stream;
}

class TwitchMessage;
class ChatCache;

class ChatView : public QTextBrowser {
    Q_OBJECT
public:
    ChatView(QWidget* = nullptr);
    virtual ~ChatView();

    void reset();
    void addMessage(const QString&);

    ChatCache* cache() const;

private slots:
    friend class ChatWidget;
    void onJoinedChannel(const QString&);
    void onMessageReceived(const TwitchMessage&);
    void onEmoteLoaded(const QPair<Twitch::Emote, QImage>&);
    void onBadgeLoaded(const QPair<Twitch::Badge, QPair<QString, QImage>>&);

private:
    ChatCache* m_cache;
};

#endif // CHATVIEW_HPP
