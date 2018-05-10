#ifndef INITWIDGET_HPP
#define INITWIDGET_HPP

#include <QSettings>
#include <QWidget>

class InitWidget : public QWidget {
    Q_OBJECT
public:
    explicit InitWidget(QWidget* = nullptr);
    virtual ~InitWidget();

    QSettings& initSettings();

    virtual void init() = 0;
signals:
    void startedIniting();
    void initProgress(const QString&);
    void endedIniting();

protected:
    void tryToEndInit();
    bool isFulfilled(const QString&);
    void setRequirementFulfilled(const QString&);
    virtual bool checkInitStatus();

private:
    QHash<QString, bool> m_initStatus;

    void onStartedIniting();
    void onEndedIniting();
};

#endif // INITWIDGET_HPP
