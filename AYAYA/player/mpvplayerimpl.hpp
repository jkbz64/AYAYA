#ifndef MPVPLAYERIMPL_HPP
#define MPVPLAYERIMPL_HPP

#include "playerimpl.hpp"
#include <mpv/qthelper.hpp>

class QOpenGLWidget;
class mpv_opengl_cb_context;

namespace detail {
class MpvPlayerImpl : public PlayerImpl {
    Q_OBJECT
public:
    MpvPlayerImpl(PlayerWidget*);
    virtual ~MpvPlayerImpl();

    mpv::qt::Handle& handle();

    virtual bool init() override;
    virtual void load(const QString&) override;
    virtual QString currentPath() override;
    virtual void setVolume(int) override;
    virtual int volume() const override;
    virtual bool paintGL() override;
    virtual bool initializeGL() override;

signals:
    void mpvEvent();

private:
    mutable mpv::qt::Handle m_mpv;
    mpv_opengl_cb_context* m_mpv_gl;
    bool m_initedGL;

private slots:
    static void onUpdate(void*);
    void swapped();
    void maybeUpdate();
    void handleMpvEvent(mpv_event*);
    void processEvents();
};
}

#endif // MPVPLAYERIMPL_HPP
