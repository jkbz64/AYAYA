#include "browser.hpp"
#include "browseritemwidget.hpp"

Browser::Browser(QWidget* parent)
    : QScrollArea(parent)
{
    setWidgetResizable(true);
    setWidget(new QWidget(this));
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

Browser::~Browser() = default;

const QVector<BrowserItemWidget*>& Browser::visibleWidgets() const
{
    return m_visibleWidgets;
}

QWidget* Browser::centralWidget() const
{
    return widget();
}

void Browser::addItem(BrowserItemWidget* item)
{
    m_visibleWidgets.push_back(item);
    emit itemAdded(item);
}

void Browser::clear()
{
    for (auto& widget : m_visibleWidgets)
        widget->deleteLater();
    m_visibleWidgets.clear();
}
