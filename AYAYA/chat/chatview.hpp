#ifndef CHATVIEW_HPP
#define CHATVIEW_HPP

#include <QPair>
#include <QTextBrowser>
#include <TwitchQt/twitchemote.hpp>

class TwitchMessage;
class EmotesCache;

class ChatView : public QTextBrowser {
    Q_OBJECT
public:
    ChatView(QWidget* = nullptr);
    virtual ~ChatView();

    void reset();
    void addMessage(const QString&);

    void setSpacing(int);
    int spacing() const;

    EmotesCache* emotesCache() const;

private slots:
    friend class ChatWidget;
    void onJoinedChannel(const QString&);
    void onMessageReceived(const TwitchMessage&);
    void onEmoteLoaded(const QPair<Twitch::Emote, QImage>&);

private:
    EmotesCache* m_emotesCache;
    int m_spacing;
};

#endif // CHATVIEW_HPP
