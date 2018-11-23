#ifndef BROWSERITEMWIDGET_HPP
#define BROWSERITEMWIDGET_HPP

#include <QFrame>

class QPropertyAnimation;

class BrowserItemWidget : public QFrame {
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)
public:
    explicit BrowserItemWidget(QWidget* = nullptr);
    virtual ~BrowserItemWidget();

    void setColor(QColor color);
    const QColor& color();
signals:
    void pressed();
    void hovered();

protected:
    virtual void enterEvent(QEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent*) override;

private:
    QPropertyAnimation* m_highlightAnimation;
    QColor m_currentColor;
};

#endif // BROWSERITEMWIDGET_HPP
