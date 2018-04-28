#ifndef BROWSERWIDGET_H
#define BROWSERWIDGET_H

#include <QSettings>
#include <QWidget>

#include <TwitchQt/Twitch>

class Channel;
class GameWidget;
class StreamWidget;

namespace Ui {
class BrowserWidget;
}

class BrowserWidget : public QWidget {
    Q_OBJECT
public:
    explicit BrowserWidget(QWidget* parent = nullptr);
    void showTopGames();
    void searchStreamsByName(const QString&);
signals:
    void streamEntered(const Twitch::User&, const Twitch::Stream&);

public slots:
    void searchStreamsByGame(const Twitch::Game&);

private slots:
    //void updateWidget(GameWidget*);
    //void updateWidget(StreamWidget*);

    void onStreamSelected(const Twitch::Stream&);

private:
    Ui::BrowserWidget* m_ui;
    QSettings m_settings;
    Twitch::Api* m_api;

    void performUpdate(QVector<GameWidget*>);
    void performUpdate(QVector<StreamWidget*>);
};

#endif // BROWSERWIDGET_H
