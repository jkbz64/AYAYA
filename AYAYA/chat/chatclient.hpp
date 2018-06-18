#ifndef CHATCLIENT_HPP
#define CHATCLIENT_HPP

#include "TwitchQt/twitchemote.hpp"
#include <QColor>
#include <QObject>

class QTimer;

class IrcConnection;
class IrcMessage;

struct TwitchMessage {
    QString m_author;
    QString m_content;
    QColor m_color;
    bool m_isMod;
    bool m_isSubscriber;
    QVector<Twitch::Emote> m_emotes;
};

class ChatClient : public QObject {
    Q_OBJECT
public:
    explicit ChatClient(QObject* parent = nullptr);
    virtual ~ChatClient();

    const QString& currentChannel() const;
    void joinChannel(const QString&);
    void depart();
signals:
    void connected();
    void disconnected();
    void joinedChannel(const QString&);
    void departed();
    void messageReceived(const TwitchMessage&);

protected:
    virtual void setupConnection();
    virtual void onMessageReceived(IrcMessage*);
    virtual void onConnected();
    virtual void onDisconnected();

    IrcConnection* m_connection;
    QString m_currentChannel;
};

#endif // CHATSERVER_HPP
