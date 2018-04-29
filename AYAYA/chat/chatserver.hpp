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

    void joinChannel(const QString&);
signals:
    void joined();
    void disconnected();
    void messageReceived(const QString&);

private:
    void onPrivateMessageReceived(IrcPrivateMessage*);
    void onMessageReceived(IrcMessage*);

    IrcConnection* m_connection;
};

#endif // CHATSERVER_HPP
