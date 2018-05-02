#ifndef CHATCLIENT_HPP
#define CHATCLIENT_HPP

#include <QObject>

class QTimer;

class IrcConnection;
class IrcMessage;

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
    void joined();
    void departed();
    void messageReceived(const QString&, const QString&);

protected:
    virtual void setupConnection();
    virtual void onMessageReceived(IrcMessage*);
    virtual void onConnected();
    virtual void onDisconnected();

    IrcConnection* m_connection;
    QTimer* m_retryTimer;
    QString m_currentChannel;
};

#endif // CHATSERVER_HPP
