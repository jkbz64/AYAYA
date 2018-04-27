#ifndef MPVCONTROLLER_HPP
#define MPVCONTROLLER_HPP

#include "playercontroller.hpp"
#include <mpv/qthelper.hpp>

class MpvController : public PlayerController {
    Q_OBJECT
public:
    explicit MpvController(mpv::qt::Handle);
    ~MpvController();

    mpv::qt::Handle& handle();
    void command(const QVariant&);
    void setProperty(const QString&, const QVariant&);
    void setOption(const QString& name, const QVariant&);
    const QVariant getProperty(const QString&) const;

    virtual bool init() override;
    virtual void load(const QString&) override;
    virtual QString currentPath() override;
    virtual void setVolume(int) override;
    virtual void mute(bool) override;
    virtual int volume() const override;

signals:
    void mpvEvent();
    void startedLoading();
    void loaded();
    void ended();
    void buffering(int);
    void resized();
    void positionChanged(double);
    void volumeChanged(double);

private slots:
    void processEvents();

private:
    mpv::qt::Handle m_mpv;

    void handleMpvEvent(mpv_event*);
};

#endif // MPVOBJECT_HPP
