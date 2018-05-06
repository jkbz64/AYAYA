#include "chatview.hpp"
#include <QGraphicsTextItem>
#include <QRectF>
#include <QScrollBar>

#include <QDebug>
#include <QTimer>

ChatView::ChatView(QWidget* parent)
    : QTextBrowser(parent)
    , m_spacing(5)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

ChatView::~ChatView()
{
}

void ChatView::addMessage(const QString& message)
{
    append(message + "\n");
}

void ChatView::reset()
{
    document()->clear();
}
