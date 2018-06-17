#ifndef INITWIDGET_HPP
#define INITWIDGET_HPP

#include <QSettings>
#include <QWidget>

class InitWidget : public QWidget {
    Q_OBJECT
public:
    explicit InitWidget(QWidget* = nullptr);
    virtual ~InitWidget();

    virtual void init() = 0;

    QSettings& initSettings();

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

    static QSettings m_initSettings;
};

#endif // INITWIDGET_HPP
