#ifndef BROWSER_HPP
#define BROWSER_HPP

#include <QGraphicsView>

class BrowserItemWidget;

class Browser : public QScrollArea {
    Q_OBJECT
public:
    Browser(QWidget*);
    virtual ~Browser();

    const QVector<BrowserItemWidget*>& visibleWidgets() const;

    QWidget* centralWidget() const;
    void clear();
signals:
    void itemAdded(BrowserItemWidget*);

protected:
    void addItem(BrowserItemWidget*);
    QVector<BrowserItemWidget*> m_visibleWidgets;
};

#endif // BROWSER_HPP
