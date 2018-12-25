#ifndef MPVPLAYERIMPL_HPP
#define MPVPLAYERIMPL_HPP

#include "playerimpl.hpp"
#include <QOpenGLWidget>
#include <mpv/qthelper.hpp>

class QOpenGLWidget;
class mpv_opengl_cb_context;

namespace detail {
class MpvPlayerImpl : public PlayerImpl {
    Q_OBJECT
public:
    MpvPlayerImpl(PlayerWidget*);
    virtual ~MpvPlayerImpl();

    mpv::qt::Handle& handle() const;
    mpv_opengl_cb_context* context() const;

    virtual bool init() override;
    virtual void load(const QString&) override;
    virtual QString currentPath() override;
    virtual void setVolume(int) override;
    virtual int volume() const override;

    void setOption(const QString&, const QVariant&);

signals:
    void mpvEvent();

private:
    mutable mpv::qt::Handle m_mpv;
    mpv_opengl_cb_context* m_mpv_gl;

private slots:
    void handleMpvEvent(mpv_event*);
    void processEvents();
};

class MpvWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    MpvWidget(MpvPlayerImpl*);
    virtual ~MpvWidget();

    bool initedGL() const;

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int, int) override;

private:
    MpvPlayerImpl* m_impl;
    bool m_initedGL;

    friend class detail::MpvPlayerImpl;
    static void onUpdate(void*);

private slots:
    void maybeUpdate();
    void swapped();
};
}

#endif // MPVPLAYERIMPL_HPP
