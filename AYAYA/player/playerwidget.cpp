#include "playerwidget.hpp"
#include "controlswidget.hpp"

#include <QGridLayout>
#include <QSpacerItem>

#include "mpvplayerimpl.hpp"

#include <QDebug>

PlayerWidget::PlayerWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_impl(nullptr)
    , m_controlsWidget(nullptr)
{
    setBaseSize(1, 1);
    // Backend defaults to MPV
    setBackend(Backend::MPV);
}

PlayerWidget::~PlayerWidget()
{
}

void PlayerWidget::setBackend(PlayerWidget::Backend backend)
{
    m_backend = backend;

    if (m_impl)
        delete m_impl;
    m_impl = new detail::MpvPlayerImpl(this);
    m_impl->init();

    if (!layout())
        setupOverlay();
}

void PlayerWidget::openStream(const QString& streamName)
{
    if (m_impl)
        m_impl->load(QString("https://www.twitch.tv/" + streamName));
}

void PlayerWidget::resetStream()
{
    if (m_impl)
        m_impl->load(m_impl->currentPath());
}

void PlayerWidget::setVolume(int value)
{
    if (m_impl)
        m_impl->setVolume(value);
}

int PlayerWidget::volume() const
{
    if (m_impl)
        return m_impl->volume();
    return 0;
}

ControlsWidget* PlayerWidget::controlsWidget()
{
    return m_controlsWidget;
}

void PlayerWidget::initializeGL()
{
    if (m_impl && !m_impl->initializeGL())
        QOpenGLWidget::initializeGL();
}

void PlayerWidget::paintGL()
{
    if (m_impl && !m_impl->paintGL())
        QOpenGLWidget::paintGL();
}

void PlayerWidget::resizeGL(int w, int h)
{
    if (m_impl && !m_impl->resizeGL(w, h))
        QOpenGLWidget::resizeGL(w, h);
}

void PlayerWidget::setupOverlay()
{
    auto overlayLayout = new QGridLayout();
    m_controlsWidget = new ControlsWidget(this);

    overlayLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 0, 12, 1);
    overlayLayout->addWidget(m_controlsWidget, 12, 0, 2, 1);

    setLayout(overlayLayout);
}
