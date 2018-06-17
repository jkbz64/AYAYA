#ifndef CHATVIEW_HPP
#define CHATVIEW_HPP

#include <QPair>
#include <QTextBrowser>

namespace Twitch {
class Emote;
}

class TwitchMessage;
class EmotesCache;

class ChatView : public QTextBrowser {
    Q_OBJECT
public:
    ChatView(QWidget* = nullptr);
    virtual ~ChatView();

    void reset();
    void addMessage(const QString&);

    EmotesCache* emotesCache() const;

private slots:
    friend class ChatWidget;
    void onJoinedChannel(const QString&);
    void onMessageReceived(const TwitchMessage&);
    void onEmoteLoaded(const QPair<Twitch::Emote, QImage>&);

private:
    EmotesCache* m_emotesCache;
};

#endif // CHATVIEW_HPP
