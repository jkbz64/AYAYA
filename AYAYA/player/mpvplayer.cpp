#include "mpvplayer.hpp"
#include "mpvcontroller.hpp"
#include <QOpenGLContext>
#include <mpv/opengl_cb.h>
#include <mpv/qthelper.hpp>

namespace {
static void* get_proc_address(void* ctx, const char* name)
{
    Q_UNUSED(ctx);
    QOpenGLContext* glctx = QOpenGLContext::currentContext();
    if (!glctx)
        return NULL;
    return (void*)glctx->getProcAddress(QByteArray(name));
}
}

MpvPlayer::MpvPlayer(QWidget* parent)
    : GLPlayer(new MpvController(mpv::qt::Handle::FromRawHandle(mpv_create())), parent)
    , m_mpv(qobject_cast<MpvController*>(controller()))
{
    if (m_mpv->init()) {
        setBaseSize(1, 1);
        m_mpv->setOption("vo", "opengl-cb");
        m_mpv_gl = (mpv_opengl_cb_context*)mpv_get_sub_api(m_mpv->handle(), MPV_SUB_API_OPENGL_CB);
        if (!m_mpv_gl)
            throw std::runtime_error("OpenGL not compiled in");
        mpv_opengl_cb_set_update_callback(m_mpv_gl, MpvPlayer::onUpdate, (void*)this);
    }
}

MpvPlayer::~MpvPlayer()
{
    makeCurrent();
    mpv_opengl_cb_set_update_callback(m_mpv_gl, NULL, NULL);
    mpv_opengl_cb_uninit_gl(m_mpv_gl);
}

void MpvPlayer::initializeGL()
{
    int r = mpv_opengl_cb_init_gl(m_mpv_gl, NULL, get_proc_address, NULL);
    if (r < 0)
        throw std::runtime_error("could not initialize OpenGL");
}

void MpvPlayer::paintGL()
{
    mpv_opengl_cb_draw(m_mpv_gl, defaultFramebufferObject(), width(), -height());
}

void MpvPlayer::maybeUpdate()
{
    if (window()->isMinimized()) {
        makeCurrent();
        paintGL();
        context()->swapBuffers(context()->surface());
        swapped();
        doneCurrent();
    } else {
        update();
    }
}

void MpvPlayer::onUpdate(void* ctx)
{
    QMetaObject::invokeMethod((MpvPlayer*)ctx, "maybeUpdate");
}

void MpvPlayer::swapped()
{
    mpv_opengl_cb_report_flip(m_mpv_gl, 0);
}
