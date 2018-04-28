#ifndef BROWSER_HPP
#define BROWSER_HPP

#include "browseritemwidget.hpp"
#include <QScrollArea>

class Browser : public QScrollArea {
    Q_OBJECT
public:
    explicit Browser(QWidget* = nullptr);
    virtual ~Browser();

    void addItem(BrowserItemWidget*);
    void clear();
    const QVector<BrowserItemWidget*>& browserItems() const;
signals:
    void itemAdded(BrowserItemWidget*);
    void itemHovered(BrowserItemWidget*);
    void itemPressed(BrowserItemWidget*);
    void itemRemoved(BrowserItemWidget*);
    void itemResized(BrowserItemWidget*, QSize, QSize);

protected:
    QSize m_itemSize;
    QVector<BrowserItemWidget*> m_browserItems;
};

#endif // BROWSER_HPP
