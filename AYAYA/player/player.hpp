#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "playercontroller.hpp"
#include <QOpenGLWidget>

class Player {
public:
    explicit Player(PlayerController* controller, QWidget* parent);
    virtual ~Player();

    QWidget* playerWidget();
    PlayerController* controller();

protected:
    QWidget* m_playerWidget;
    PlayerController* m_controller;
};

class GLPlayer : public QOpenGLWidget, public Player {
    Q_OBJECT
public:
    explicit GLPlayer(PlayerController* controller, QWidget* parent = nullptr);
    virtual ~GLPlayer() = default;
};

class SimplePlayer : public QWidget, public Player {
    Q_OBJECT
public:
    explicit SimplePlayer(PlayerController* controller, QWidget* parent = nullptr);
    virtual ~SimplePlayer() = default;
};

#endif // PLAYER_HPP
