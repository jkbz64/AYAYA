#ifndef STREAMWIDGET_HPP
#define STREAMWIDGET_HPP

#include <QFrame>
#include <TwitchQt/twitchstream.hpp>

class StreamWidget : public QFrame {
    Q_OBJECT
public:
    explicit StreamWidget(const Twitch::Stream&, QWidget* = nullptr);

    void setPreview(const QPixmap&);

    const Twitch::Stream& data() const;
signals:
    void hovered();
    void pressed();

protected:
    virtual void paintEvent(QPaintEvent*) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

private:
    Twitch::Stream m_data;
    friend class BrowserWidget;
    QPixmap m_preview;
};

#endif // STREAMWIDGET_HPP
