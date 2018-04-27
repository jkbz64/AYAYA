#ifndef PLAYERCONTROLLER_HPP
#define PLAYERCONTROLLER_HPP

#include <QObject>

class PlayerController : public QObject {
    Q_OBJECT
public:
    PlayerController() = default;
    virtual ~PlayerController() = default;

    virtual bool init() = 0;
    virtual void load(const QString&) = 0;
    virtual QString currentPath() = 0;
    virtual void setVolume(int) = 0;
    virtual void mute(bool) = 0;
    virtual int volume() const = 0;

signals:
    void startedLoading();
    void loaded();
    void ended();
    void buffering(int);
    void resized();
    void positionChanged(double);
    void volumeChanged(double);
};

#endif // PLAYERCONTROLLER_HPP
