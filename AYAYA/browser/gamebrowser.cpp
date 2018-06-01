#include "gamebrowser.hpp"

#include "gameitemwidget.hpp"
#include <TwitchQt/twitchgame.hpp>

#include "flowlayout.hpp"
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>

GameBrowser::GameBrowser(QWidget* parent)
    : Browser(parent)
    , m_minItemSize(150)
{
    centralWidget()->setLayout(new FlowLayout);
}

GameItemWidget* GameBrowser::addGame(const Twitch::Game& game)
{
    auto gameWidget = new GameItemWidget(game, centralWidget());
    gameWidget->setFixedSize(m_minItemSize, m_minItemSize);
    widget()->layout()->addWidget(gameWidget);
    fixLayout();
    addItem(gameWidget);

    return gameWidget;
}

void GameBrowser::setMinItemSize(int size)
{
    m_minItemSize = size;
    fixLayout();
}

int GameBrowser::minItemSize() const
{
    return m_minItemSize;
}

int GameBrowser::currentItemSize() const
{
    return m_currentItemSize;
}

int GameBrowser::rowCount() const
{
    return m_rowCount;
}

#include <QResizeEvent>

void GameBrowser::resizeEvent(QResizeEvent* event)
{
    Browser::resizeEvent(event);
    fixLayout();
}

void GameBrowser::fixLayout()
{
    const int row = width() / minItemSize();
    const int diff = width() % (minItemSize() + 1);
    const int wPerOne = diff / (row - 1);
    // Update current item size
    m_currentItemSize = minItemSize() + wPerOne;
    for (QWidget* w : visibleWidgets()) {
        w->setFixedSize(currentItemSize(), currentItemSize());
    }
    // Update row count
    m_rowCount = row;
}
