#ifndef CHATSERVER_HPP
#define CHATSERVER_HPP

#include <QObject>

class ChatServer : public QObject {
    Q_OBJECT
public:
    explicit ChatServer(QObject* parent = nullptr);

signals:
    void messageReceived(const QString&);
public slots:
};

#endif // CHATSERVER_HPP
