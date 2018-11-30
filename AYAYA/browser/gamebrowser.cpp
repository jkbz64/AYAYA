#include "gamebrowser.hpp"
#include "flowlayout.hpp"
#include "gameitemwidget.hpp"

#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QTimer>

GameBrowser::GameBrowser(QWidget* parent)
    : Browser(parent)
    , m_minItemSize(200)
    , m_resizeTimer(new QTimer(this))
    , m_resizeAnimations(new QParallelAnimationGroup(this))
{
    centralWidget()->setLayout(new FlowLayout);
    m_resizeTimer->setInterval(150);
    connect(m_resizeTimer, &QTimer::timeout, this, &GameBrowser::fixLayout, Qt::UniqueConnection);

    setMinimumWidth(300);
}

GameItemWidget* GameBrowser::addGame(const Twitch::Game& game)
{
    auto gameWidget = new GameItemWidget(game, centralWidget());
    gameWidget->setFixedSize(m_minItemSize, m_minItemSize);
    widget()->layout()->addWidget(gameWidget);
    fixLayout();
    addItem(gameWidget);

    connect(gameWidget, &BrowserItemWidget::pressed, this, [this, gameWidget, game]() {
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        m_resizeAnimations->stop();
        m_resizeAnimations->clear();

        auto fadeAnimations = new QParallelAnimationGroup(this);
        for (const auto w : visibleWidgets()) {
            disconnect(w, &BrowserItemWidget::pressed, 0, 0);

            QPropertyAnimation* opacityAnimation = new QPropertyAnimation(w->graphicsEffect(), "opacity");
            opacityAnimation->setStartValue(0.99);
            opacityAnimation->setEndValue(0.0);
            opacityAnimation->setDuration(150);
            fadeAnimations->addAnimation(opacityAnimation);
        }
        fadeAnimations->start(QAbstractAnimation::DeletionPolicy::DeleteWhenStopped);

        connect(fadeAnimations, &QParallelAnimationGroup::stateChanged, [this, game](QAnimationGroup::State ns, QAnimationGroup::State) {
            if (ns == QAnimationGroup::State::Stopped)
                emit gameSelected(game);
        });
    },
        Qt::UniqueConnection);

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
    m_resizeTimer->setInterval(150);
    m_resizeTimer->start();
}

void GameBrowser::fixLayout()
{
    int row = width() / minItemSize();
    if (row <= 0)
        row = 1;
    const int diff = width() % (minItemSize() - 20);
    const int wPerOne = diff / row;
    m_currentItemSize = minItemSize() + wPerOne;

    m_resizeAnimations->stop();
    m_resizeAnimations->clear();

    for (const auto w : visibleWidgets()) {
        auto minSizeAnimation = new QPropertyAnimation(w, "minimumSize");
        auto maxSizeAnimation = new QPropertyAnimation(w, "maximumSize");

        minSizeAnimation->setDuration(70);
        minSizeAnimation->setStartValue(w->size());
        minSizeAnimation->setEndValue(QSize(currentItemSize(), currentItemSize()));
        minSizeAnimation->setEasingCurve(QEasingCurve::InCirc);

        maxSizeAnimation->setDuration(70);
        maxSizeAnimation->setStartValue(w->size());
        maxSizeAnimation->setEndValue(QSize(currentItemSize(), currentItemSize()));
        maxSizeAnimation->setEasingCurve(QEasingCurve::InCirc);

        m_resizeAnimations->addAnimation(minSizeAnimation);
        m_resizeAnimations->addAnimation(maxSizeAnimation);
    }

    m_resizeAnimations->start();

    m_rowCount = row;
}
