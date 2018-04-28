#ifndef BROWSERITEMWIDGET_HPP
#define BROWSERITEMWIDGET_HPP

#include <QFrame>
#include <QVariant>

class BrowserItemWidget : public QFrame {
    Q_OBJECT
public:
    explicit BrowserItemWidget(QWidget* parent = nullptr);
    virtual ~BrowserItemWidget() = default;

    void updateData(const QVariant&);
    const QVariant& data() const;

signals:
    void dataUpdated();
    void pressed();
    void hovered();
    void resized(QSize, QSize);

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void resizeEvent(QResizeEvent*) override;

    QVariant m_data;
};

#endif // BROWSERITEMWIDGET_HPP
