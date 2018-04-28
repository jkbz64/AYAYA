#include "browser.hpp"
#include "flowlayout.hpp"

Browser::Browser(QWidget* parent)
    : QScrollArea(parent)
    , m_itemSize(150, 150)
{
    setWidgetResizable(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    auto centralWidget = new QWidget(this);
    centralWidget->setLayout(new FlowLayout(centralWidget));
    setWidget(centralWidget);
}

Browser::~Browser() = default;

void Browser::addItem(BrowserItemWidget* item)
{
    item->setParent(this);

    item->setFixedSize(m_itemSize);
    item->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    widget()->layout()->addWidget(item);

    connect(item, &BrowserItemWidget::hovered, [this, item]() { emit itemHovered(item); });
    connect(item, &BrowserItemWidget::pressed, [this, item]() { emit itemPressed(item); });
    connect(item, &BrowserItemWidget::resized, [this, item](QSize oldSize, QSize newSize) { emit itemResized(item, oldSize, newSize); });

    m_browserItems.push_back(item);
    emit itemAdded(item);
}

void Browser::clear()
{
    for (auto item : m_browserItems) {
        emit itemRemoved(item);
        item->deleteLater();
    }
    m_browserItems.clear();
}

const QVector<BrowserItemWidget*>& Browser::browserItems() const
{
    return m_browserItems;
}
