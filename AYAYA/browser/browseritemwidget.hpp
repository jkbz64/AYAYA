#ifndef BROWSERITEMWIDGET_HPP
#define BROWSERITEMWIDGET_HPP

#include <QFrame>

class BrowserItemWidget : public QFrame {
    Q_OBJECT
public:
    explicit BrowserItemWidget(QWidget* = nullptr);
    virtual ~BrowserItemWidget();
signals:
    void pressed();
    void hovered();

protected:
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent*) override;
};

#endif // BROWSERITEMWIDGET_HPP
