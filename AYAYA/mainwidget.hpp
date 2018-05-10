#ifndef MAINWIDGET_HPP
#define MAINWIDGET_HPP

#include <QWidget>

class MainWidget : public QWidget {
    Q_OBJECT
public:
    explicit MainWidget(QWidget* = nullptr);
    virtual ~MainWidget();

    virtual void init() = 0;
signals:
    void startedIniting();
    void initProgress(const QString&);
    void endedIniting();

private:
    void onStartedIniting();
    void onEndedIniting();
};

#endif // MAINWIDGET_HPP
