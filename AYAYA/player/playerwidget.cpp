#include "playerwidget.hpp"
#include "controlswidget.hpp"
#include "mpvplayer.hpp"
#include "player.hpp"

#include <QGridLayout>
#include <QSpacerItem>

PlayerWidget::PlayerWidget(QWidget* parent)
    : QDockWidget(parent)
    , m_player(new MpvPlayer(nullptr))
{
    setFeatures(NoDockWidgetFeatures);
    setAllowedAreas(Qt::NoDockWidgetArea);
    setTitleBarWidget(new QWidget());

    auto playerWidget = m_player->playerWidget();
    setWidget(playerWidget);
    auto controller = m_player->controller();
    auto controlsWidget = new ControlsWidget(playerWidget);

    connect(controller, &PlayerController::startedLoading, this, &PlayerWidget::onStartedLoading);
    connect(controller, &PlayerController::loaded, this, &PlayerWidget::onStreamLoaded);
    connect(controller, &PlayerController::volumeChanged, this, [controlsWidget]() {

    });

    playerWidget->setLayout(new QGridLayout());

    auto lay = qobject_cast<QGridLayout*>(playerWidget->layout());
    lay->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 0, 12, 1);
    lay->addWidget(controlsWidget, 12, 0, 2, 1);
}

PlayerWidget::~PlayerWidget()
{
    delete m_player;
}

void PlayerWidget::openStream(const QString& streamName)
{
    m_player->controller()->load("https://www.twitch.tv/" + streamName);
}

void PlayerWidget::resetStream()
{
    auto controller = m_player->controller();
    controller->load(controller->currentPath());
}

void PlayerWidget::mute(bool mute)
{
    m_player->controller()->mute(mute);
}

void PlayerWidget::setVolume(int value)
{
    m_player->controller()->setVolume(value);
}

int PlayerWidget::volume() const
{
    return m_player->controller()->volume();
}

Player* PlayerWidget::player()
{
    return m_player;
}

void PlayerWidget::mousePressEvent(QMouseEvent* event)
{
    setVolume(50);
}

void PlayerWidget::onStartedLoading()
{
    // TODO
}

void PlayerWidget::onStreamLoaded()
{
    // TODO
}
