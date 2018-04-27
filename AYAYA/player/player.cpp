#include "player.hpp"

Player::Player(PlayerController* controller, QWidget* parent)
    : m_playerWidget(parent)
    , m_controller(controller)
{
}

Player::~Player()
{
    delete m_controller;
}

QWidget* Player::playerWidget()
{
    return m_playerWidget;
}

PlayerController* Player::controller()
{
    return m_controller;
}

SimplePlayer::SimplePlayer(PlayerController* controller, QWidget* parent)
    : QWidget(parent)
    , Player(controller, this)
{
}

GLPlayer::GLPlayer(PlayerController* controller, QWidget* parent)
    : QOpenGLWidget(parent)
    , Player(controller, this)
{
}
