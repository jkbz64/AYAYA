#include "mpvplayerimpl.hpp"
#include "../playerwidget.hpp"
#include <QOpenGLContext>
#include <QOpenGLWidget>
#include <mpv/opengl_cb.h>
#include <mpv/qthelper.hpp>

namespace detail {
namespace {
    static void* get_proc_address(void* ctx, const char* name)
    {
        Q_UNUSED(ctx);
        QOpenGLContext* glctx = QOpenGLContext::currentContext();
        if (!glctx)
            return NULL;
        return (void*)glctx->getProcAddress(QByteArray(name));
    }

    static void wakeup(void* ctx)
    {
        // This callback is invoked from any mpv thread (but possibly also
        // recursively from a thread that is calling the mpv API). Just notify
        // the Qt GUI thread to wake up (so that it can process events with
        // mpv_wait_event()), and return as quickly as possible.
        MpvPlayerImpl* mpvObject = (MpvPlayerImpl*)ctx;
        emit mpvObject->mpvEvent();
    }

    // Helper functions
    void command(mpv::qt::Handle& m_mpv, const QVariant& params)
    {
        mpv::qt::command_variant(m_mpv, params);
    }

    void setProperty(mpv::qt::Handle& m_mpv, const QString& name, const QVariant& value)
    {
        mpv::qt::set_property_variant(m_mpv, name, value);
    }

    const QVariant getProperty(mpv::qt::Handle& m_mpv, const QString& name)
    {
        return mpv::qt::get_property_variant(m_mpv, name);
    }

    void setOption(mpv::qt::Handle& m_mpv, const QString& name, const QVariant& value)
    {
        mpv::qt::set_option_variant(m_mpv, name, value);
    }
}

MpvPlayerImpl::MpvPlayerImpl(PlayerWidget* player)
    : PlayerImpl(player)
    , m_mpv_gl(nullptr)
{
    m_renderWidget = new MpvWidget(this);
}

MpvPlayerImpl::~MpvPlayerImpl()
{
    const auto render = qobject_cast<MpvWidget*>(renderWidget());
    render->makeCurrent();
    mpv_opengl_cb_set_update_callback(m_mpv_gl, NULL, NULL);
    if (m_mpv_gl && render->initedGL())
        mpv_opengl_cb_uninit_gl(m_mpv_gl);
}

mpv_opengl_cb_context* MpvPlayerImpl::context() const
{
    return m_mpv_gl;
}

mpv::qt::Handle& MpvPlayerImpl::handle() const
{
    return m_mpv;
}

bool MpvPlayerImpl::init()
{
    std::setlocale(LC_NUMERIC, "C");
    connect(this, &MpvPlayerImpl::mpvEvent, this, &MpvPlayerImpl::processEvents, Qt::QueuedConnection);

    m_mpv = mpv::qt::Handle::FromRawHandle(mpv_create());
    if (!m_mpv)
        throw std::runtime_error("could not create mpv context");

    mpv_set_option_string(m_mpv, "vo", "opengl-cb");
    mpv_set_option_string(m_mpv, "demuxer-seekable-cache", "no");
    mpv_set_option_string(m_mpv, "cache-secs", "8");

    mpv_observe_property(m_mpv, 0, "playback-time", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "volume", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "cache-buffering-state", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "paused-for-cache", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "core-idle", MPV_FORMAT_FLAG);

    mpv_set_wakeup_callback(m_mpv, wakeup, this);

    if (mpv_initialize(m_mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    m_mpv_gl = (mpv_opengl_cb_context*)mpv_get_sub_api(m_mpv, MPV_SUB_API_OPENGL_CB);

    if (!m_mpv_gl)
        throw std::runtime_error("OpenGL not compiled in");

    mpv_opengl_cb_set_update_callback(m_mpv_gl, MpvWidget::onUpdate, (void*)m_renderWidget);

    return true;
}

void MpvPlayerImpl::load(const QString& path)
{
    if (path.isEmpty())
        setOption("pause", true);
    else {
        setOption("pause", false);
        command(m_mpv, (QStringList() << "loadfile" << path));
    }
}

QString MpvPlayerImpl::currentPath()
{
    return getProperty(m_mpv, "path").toString();
}

void MpvPlayerImpl::setVolume(int value)
{
    setOption("volume", value);
}

int MpvPlayerImpl::volume() const
{
    return getProperty(m_mpv, "volume").toInt();
}

void MpvPlayerImpl::setOption(const QString& option, const QVariant& value)
{
    detail::setOption(m_mpv, option, value);
}

void MpvPlayerImpl::processEvents()
{
    while (m_mpv) {
        mpv_event* event = mpv_wait_event(m_mpv, 0);
        if (event->event_id == MPV_EVENT_NONE)
            break;
        handleMpvEvent(event);
    }
}

void MpvPlayerImpl::handleMpvEvent(mpv_event* event)
{
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE: {
        mpv_event_property* prop = (mpv_event_property*)event->data;
        if (strcmp(prop->name, "playback-time") == 0) {
            if (prop->format == MPV_FORMAT_DOUBLE)
                emit player()->positionChanged(*(double*)prop->data);
        }
        if (strcmp(prop->name, "volume") == 0) {
            if (prop->format == MPV_FORMAT_DOUBLE)
                emit player()->volumeChanged((int)(*(double*)prop->data));
        }
        if (strcmp(prop->name, "cache-buffering-state") == 0) {
            if (prop->format == MPV_FORMAT_INT64) {
                if ((int)*(int*)prop->data < 100)
                    emit player()->buffering(*(int*)prop->data);
            }
        }
        break;
    }
    case MPV_EVENT_VIDEO_RECONFIG:
        // TODO
        break;
    case MPV_EVENT_END_FILE:
        emit player()->ended();
        break;
    case MPV_EVENT_START_FILE:
        emit player()->startedLoading();
        break;
    case MPV_EVENT_FILE_LOADED:
        emit player()->loaded();
        break;
    case MPV_EVENT_LOG_MESSAGE: {
        // struct mpv_event_log_message* msg = (struct mpv_event_log_message*)event->data;

        break;
    }
    case MPV_EVENT_SHUTDOWN: {
        break;
    }
    default:;
        // Ignore uninteresting or unknown events.
    }
}

// MpvWidget
MpvWidget::MpvWidget(MpvPlayerImpl* impl)
    : QOpenGLWidget(nullptr)
    , m_impl(impl)
    , m_initedGL(false)
{
}

MpvWidget::~MpvWidget()
{
}

bool MpvWidget::initedGL() const
{
    return m_initedGL;
}

void MpvWidget::initializeGL()
{
    QOpenGLWidget::initializeGL();
    if (m_initedGL) {
        makeCurrent();
        mpv_opengl_cb_uninit_gl(m_impl->context());
    }

    int r = mpv_opengl_cb_init_gl(m_impl->context(), NULL, get_proc_address, NULL);
    if (r < 0)
        throw std::runtime_error("could not initialize OpenGL");

    m_initedGL = true;
}

void MpvWidget::paintGL()
{
    QOpenGLWidget::paintGL();
    mpv_opengl_cb_draw(m_impl->context(), defaultFramebufferObject(), m_impl->player()->width(), -(m_impl->player()->height()));
}

void MpvWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);
}

void MpvWidget::onUpdate(void* ctx)
{
    QMetaObject::invokeMethod((MpvWidget*)ctx, "maybeUpdate");
}

void MpvWidget::maybeUpdate()
{
    const auto player = m_impl->player();
    if (player->window()->isMinimized()) {
        makeCurrent();
        paintGL();
        context()->swapBuffers(context()->surface());
        swapped();
        doneCurrent();
    } else
        update();
}

void MpvWidget::swapped()
{
    mpv_opengl_cb_report_flip(m_impl->context(), 0);
}
}
