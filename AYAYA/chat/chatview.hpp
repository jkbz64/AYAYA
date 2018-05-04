#ifndef CHATVIEW_HPP
#define CHATVIEW_HPP

#include <QGraphicsView>
#include <QQueue>

class ChatView : public QGraphicsView {
    Q_OBJECT
public:
    ChatView(QWidget* = nullptr);
    virtual ~ChatView();
    void reset();
    void addMessage(const QString&);
    int spacing() const;
    void setSpacing(int);
    void updateView();

protected:
    virtual void resizeEvent(QResizeEvent*);

private:
    QGraphicsScene m_scene;
    int m_currentHeight;
    int m_spacing;
    bool m_shouldFollow;

    QTimer* m_flushTimer;
    QQueue<QString> m_messagesQueue;
    QVector<QGraphicsTextItem*> m_lastMessages;
    void flush();
};

#endif // CHATVIEW_HPP
