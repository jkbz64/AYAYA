#ifndef PLAYERIMPL_HPP
#define PLAYERIMPL_HPP

#include <QObject>

class QString;
class PlayerWidget;

namespace detail {
class PlayerImpl : public QObject {
    Q_OBJECT
public:
    PlayerImpl(PlayerWidget* = nullptr);
    virtual ~PlayerImpl();

    PlayerWidget* player();

    virtual bool init() = 0;
    virtual void load(const QString&) = 0;
    virtual QString currentPath() = 0;
    virtual void setVolume(int) = 0;
    virtual int volume() const = 0;

    virtual bool paintGL();
    virtual bool initializeGL();
    virtual bool resizeGL(int, int);

private:
    PlayerWidget* m_playerWidget;
};
}

#endif // PLAYERIMPL_HPP
