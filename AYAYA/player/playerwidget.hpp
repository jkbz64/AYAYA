#ifndef PLAYERWIDGET_HPP
#define PLAYERWIDGET_HPP

#include "player/player.hpp"
#include <QDockWidget>

class MpvPlayer;

class PlayerWidget : public QDockWidget {
    Q_OBJECT
public:
    explicit PlayerWidget(QWidget* parent = nullptr);
    ~PlayerWidget();

    void openStream(const QString&);
    void resetStream();
    void mute(bool);
    void setVolume(int);
    int volume() const;

    Player* player();

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    Player* m_player;
private slots:
    void onStartedLoading();
    void onStreamLoaded();
};

#endif // PLAYERWIDGET_HPP
