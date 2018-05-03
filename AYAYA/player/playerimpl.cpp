#include "playerimpl.hpp"
#include "playerwidget.hpp"

namespace detail {
PlayerImpl::PlayerImpl(PlayerWidget* player)
    : QObject(player)
    , m_playerWidget(player)
{
}

PlayerImpl::~PlayerImpl() = default;

PlayerWidget* PlayerImpl::player()
{
    return m_playerWidget;
}

bool PlayerImpl::initializeGL()
{
    return false;
}

bool PlayerImpl::resizeGL(int, int)
{
    return false;
}

bool PlayerImpl::paintGL()
{
    return false;
}
}
