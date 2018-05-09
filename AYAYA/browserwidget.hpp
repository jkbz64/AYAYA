#ifndef BROWSERWIDGET_H
#define BROWSERWIDGET_H

#include "mainwidget.hpp"
#include <QSettings>
#include <TwitchQt/Twitch>

class Channel;
class GameItemWidget;
class StreamItemWidget;

namespace Ui {
class BrowserWidget;
}

class BrowserWidget : public MainWidget {
    Q_OBJECT
public:
    explicit BrowserWidget(QWidget* parent = nullptr);
    ~BrowserWidget();

    virtual void init() override;

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

    void performUpdate(QVector<GameItemWidget*>);
    void performUpdate(QVector<StreamItemWidget*>);
};

#endif // BROWSERWIDGET_H
