#include "playerimpl.hpp"
#include "../playerwidget.hpp"

namespace detail {
PlayerImpl::PlayerImpl(PlayerWidget* player)
    : QObject(player)
    , m_playerWidget(player)
    , m_renderWidget(nullptr)
{
}

PlayerImpl::~PlayerImpl()
{
    delete m_renderWidget;
}

PlayerWidget* PlayerImpl::player()
{
    return m_playerWidget;
}

QWidget* PlayerImpl::renderWidget()
{
    return m_renderWidget;
}
}
