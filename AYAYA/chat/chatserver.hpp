#ifndef CHATSERVER_HPP
#define CHATSERVER_HPP

#include <QObject>

class IrcConnection;
class IrcPrivateMessage;
class IrcMessage;

class ChatServer : public QObject {
    Q_OBJECT
public:
    explicit ChatServer(QObject* parent = nullptr);

    const QString& currentChannel() const;
    void joinChannel(const QString&);
signals:
    void joined();
    void disconnected();
    void messageReceived(const QString&, const QString&);

private:
    void onPrivateMessageReceived(IrcPrivateMessage*);
    void onMessageReceived(IrcMessage*);

    IrcConnection* m_connection;
    QString m_currentChannel;
};

#endif // CHATSERVER_HPP
