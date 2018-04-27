#ifndef GAMEBROWSER_H
#define GAMEBROWSER_H

#include <QObject>
#include <QScrollArea>

namespace Twitch {
class Game;
}

class GameWidget;

class GameBrowser : public QScrollArea {
    Q_OBJECT
public:
    explicit GameBrowser(QWidget* = 0);
    GameWidget* addGame(const Twitch::Game&);
    void clear();

    const QVector<GameWidget*>& visibleWidgets() const;
signals:
    void gameSelected(const Twitch::Game&);
    void updateRequested(GameWidget*);

protected:
    QVector<GameWidget*> m_visibleWidgets;
};

#endif // GAMEBROWSER_H
