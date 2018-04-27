#include "gamebrowser.hpp"
#include "flowlayout.hpp"

#include "gamewidget.hpp"

GameBrowser::GameBrowser(QWidget* parent)
    : QScrollArea(parent)
{
    setWidgetResizable(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    auto central = new QWidget(this);
    auto lay = new FlowLayout(this);

    central->setLayout(lay);
    setWidget(central);
}

GameWidget* GameBrowser::addGame(const Twitch::Game& game)
{
    auto gameWidget = new GameWidget(game, this);
    gameWidget->setFixedWidth(150);
    gameWidget->setFixedHeight(150);
    gameWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    widget()->layout()->addWidget(gameWidget);

    connect(gameWidget, &GameWidget::pressed, [this, game]() {
        emit gameSelected(game);
    });

    emit updateRequested(gameWidget);

    m_visibleWidgets.push_back(gameWidget);
    return gameWidget;
}

void GameBrowser::clear()
{
    for (auto& widget : m_visibleWidgets)
        widget->deleteLater();
    m_visibleWidgets.clear();
    widget()->layout()->update();
}

const QVector<GameWidget*>& GameBrowser::visibleWidgets() const
{
    return m_visibleWidgets;
}
