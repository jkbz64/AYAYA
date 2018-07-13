#include "nullplayerimpl.hpp"
#include "../playerwidget.hpp"
#include <QLabel>

detail::NullPlayerImpl::NullPlayerImpl(PlayerWidget* player)
    : PlayerImpl(player)
{
    auto label = new QLabel("Null Backend. Set different backend in the settings or compile AYAYA with one of supported backends.", player);
    label->setWordWrap(true);
    m_renderWidget = label;
}

detail::NullPlayerImpl::~NullPlayerImpl()
{
}

bool detail::NullPlayerImpl::init()
{
    return true;
}

void detail::NullPlayerImpl::load(const QString&)
{
}

QString detail::NullPlayerImpl::currentPath()
{
    return QString();
}

void detail::NullPlayerImpl::setVolume(int)
{
}

int detail::NullPlayerImpl::volume() const
{
    return 0;
}
