#include "chatview.hpp"
#include <QGraphicsTextItem>
#include <QRectF>
#include <QScrollBar>

#include <QDebug>
#include <QTimer>

ChatView::ChatView(QWidget* parent)
    : QGraphicsView(parent)
    , m_currentHeight(0)
    , m_spacing(5)
    , m_shouldFollow(false)
    , m_flushTimer(new QTimer(this))
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setScene(new QGraphicsScene());
    scene()->setSceneRect(0, 0, size().width(), size().height());

    connect(m_flushTimer, &QTimer::timeout, this, &ChatView::flush, Qt::DirectConnection);
    m_flushTimer->start(200);
}

ChatView::~ChatView() = default;

#include <QDebug>

void ChatView::reset()
{
    m_currentHeight = 0;
    m_messagesQueue.clear();
    scene()->clear();
    scene()->setSceneRect(0, 0, size().width(), size().height());
}

void ChatView::addMessage(const QString& message)
{
    m_messagesQueue.enqueue(message);
}

void ChatView::flush()
{
    const bool isMaxBefore = verticalScrollBar()->value() == verticalScrollBar()->maximum();
    const int heightBefore = verticalScrollBar()->value();

    bool shouldUpdate = false;
    while (!m_messagesQueue.isEmpty()) {
        auto&& message = m_messagesQueue.dequeue();

        QGraphicsTextItem* item = scene()->addText(message);
        item->setPos(QPointF(0, m_currentHeight));

        const auto itemSize = item->boundingRect().height() + m_spacing;
        m_currentHeight += itemSize;

        if (m_currentHeight >= sceneRect().height()) {
            scene()->setSceneRect(0, 0, size().width(), sceneRect().height() + itemSize);
        }

        shouldUpdate = true;
    }

    if (shouldUpdate)
        updateView();

    if (isMaxBefore || m_shouldFollow) {
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
        m_shouldFollow = false;
    } else
        verticalScrollBar()->setValue(heightBefore);
}

void ChatView::setSpacing(int spacing)
{
    m_spacing = spacing;
}

int ChatView::spacing() const
{
    return m_spacing;
}

#include <QResizeEvent>

void ChatView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);

    updateView();

    if (event->size().height() < sceneRect().height() && event->size().height() > m_currentHeight) {
        scene()->setSceneRect(0, 0, size().width(), event->size().height());
    }
    m_shouldFollow = true;
}

void ChatView::updateView()
{
    if (scene()->items().empty()) {
        scene()->setSceneRect(0, 0, size().width(), size().height());
        fitInView(0, 0, size().width(), size().height());
    } else {
        if (verticalScrollBar()->maximum() == 0)
            scene()->setSceneRect(0, 0, size().width(), size().height());
        else
            scene()->setSceneRect(0, 0, size().width(), sceneRect().height());
        fitInView(0, verticalScrollBar()->value(), size().width(), size().height());
    }
    update();
}
