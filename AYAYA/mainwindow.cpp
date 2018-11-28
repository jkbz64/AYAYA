#include "mainwindow.hpp"
#include "browser/gamebrowser.hpp"
#include "settings/extractorsettings.hpp"
#include "settings/globalsettings.hpp"
#include "settings/playersettings.hpp"
#include "ui_mainwindow.h"
#include <QMovie>
#include <QPropertyAnimation>
#include <QTextStream>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_theme(Theme::Default)
    , m_previousWidget(nullptr)
{
    m_ui->setupUi(this);
    setStyleSheet("MainWindow { padding: 1px; }");
    // Splash screen clap face
    m_ui->m_clapFace->setMovie(new QMovie(":/gifs/sunwithfaceclap.gif", QByteArray(), this));

    // Navigation bar
    connect(navigationBar(), &MainNavigationBar::watchButtonPressed, this, &MainWindow::onWatchPressed);
    connect(navigationBar(), &MainNavigationBar::browseButtonPressed, this, &MainWindow::onBrowsePressed);
    connect(navigationBar(), &MainNavigationBar::settingsButtonPressed, this, &MainWindow::onSettingsPressed);
    connect(navigationBar(), &MainNavigationBar::quitButtonPressed, this, &MainWindow::close);

    // Browser
    connect(browserWidget(), &BrowserWidget::streamEntered, this, &MainWindow::onStreamEntered);

    // Stream
    connect(streamWidget(), &StreamWidget::enteredTheaterMode, this, &MainWindow::onEnteredFullscreenMode);
    connect(streamWidget(), &StreamWidget::enteredFullscreenMode, this, &MainWindow::onEnteredFullscreenMode);
    connect(streamWidget(), &StreamWidget::leftFullscreenMode, this, &MainWindow::onLeftFullscreenMode);

    // Save default margins / spacing for going back from fulllscreen mode
    m_defaultMargin = m_ui->m_mainWidget->layout()->margin();
    m_defaultSpacing = m_ui->m_mainWidget->layout()->spacing();

    // Settings
    settingsWidget()->addTab<GlobalSettings>(this);
    settingsWidget()->addTab<ExtractorSettings>(streamWidget()->player());
    settingsWidget()->addTab<PlayerSettings>(streamWidget()->player());

    m_ui->m_settingsWidget->hide();

    // In case of Pepega
    m_ui->m_centralStack->setCurrentWidget(m_ui->m_splashWidget);
    setCurrentMainWidget(browserWidget());
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::setTheme(Theme theme)
{
    if (m_theme != theme) {
        switch (theme) {
        case Theme::Default:
            qApp->setStyleSheet(styleSheet()); // Reset to default
            break;
        case Theme::Dark: {
            QFile f(":qdarkstyle/style.qss");
            if (!f.exists()) {
                printf("Unable to set stylesheet, file not found\n");
            } else {
                f.open(QFile::ReadOnly | QFile::Text);
                QTextStream ts(&f);
                qApp->setStyleSheet(ts.readAll());
            }
            break;
        }
        }
    }

    m_theme = theme;
}

const MainWindow::Theme& MainWindow::theme() const
{
    return m_theme;
}

MainNavigationBar* MainWindow::navigationBar() const
{
    return m_ui->m_navigationBar;
}

BrowserWidget* MainWindow::browserWidget() const
{
    return m_ui->m_browserWidget;
}

StreamWidget* MainWindow::streamWidget() const
{
    return m_ui->m_streamWidget;
}

SettingsWidget* MainWindow::settingsWidget() const
{
    return m_ui->m_settingsWidget;
}

void MainWindow::init()
{
    m_initQueue << m_ui->m_settingsWidget << m_ui->m_browserWidget << m_ui->m_streamWidget;
    initNextWidget();
}

void MainWindow::setupInitWidget(InitWidget* widget)
{
    connect(widget, &InitWidget::startedIniting, this, &MainWindow::onInitStarted);
    connect(widget, &InitWidget::initProgress, this, &MainWindow::onInitProgress, Qt::QueuedConnection);
    connect(widget, &InitWidget::endedIniting, this, &MainWindow::onEndedIniting);
    emit widget->startedIniting();
}

void MainWindow::initNextWidget()
{
    if (!m_initQueue.empty()) {
        const auto currentInitWidget = m_initQueue.front();
        m_initQueue.pop_front();
        setupInitWidget(currentInitWidget);
    }
}

void MainWindow::removeMargins()
{
    setStyleSheet("padding: 0px");
    m_ui->m_mainWidget->layout()->setContentsMargins(0, 0, 0, 0);
    m_ui->m_mainWidget->layout()->setSpacing(0);
}

void MainWindow::restoreDefaultMargins()
{
    setStyleSheet("padding: 1px");
    m_ui->m_mainWidget->layout()->setMargin(m_defaultMargin);
    m_ui->m_mainWidget->layout()->setSpacing(m_defaultSpacing);
}

QWidget* MainWindow::currentMainWidget() const
{
    return m_ui->m_mainStack->currentWidget();
}

void MainWindow::setCurrentMainWidget(QWidget* widget) const
{
    m_ui->m_mainStack->setCurrentWidget(widget);
}

// Slots
void MainWindow::onInitStarted()
{
    m_ui->m_centralStack->setCurrentWidget(m_ui->m_splashWidget);
    m_ui->m_clapFace->movie()->start();
    m_ui->m_statusLabel->setText("Initing...");
}

void MainWindow::onInitProgress(const QString& progressText)
{
    m_ui->m_statusLabel->setText(progressText);
}

void MainWindow::onEndedIniting()
{
    const auto initedWidget = qobject_cast<InitWidget*>(sender());
    disconnect(initedWidget, &InitWidget::startedIniting, 0, 0);
    disconnect(initedWidget, &InitWidget::initProgress, 0, 0);
    disconnect(initedWidget, &InitWidget::endedIniting, 0, 0);

    if (!m_initQueue.empty())
        initNextWidget();
    else {
        QTimer::singleShot(50, this, [this]() {
            m_ui->m_statusLabel->setText("AYAYA is now starting");
        });
        // Let the user see the pretty sun... even if it's for a second :)
        QTimer::singleShot(1000, this, [this]() {
            m_ui->m_clapFace->movie()->stop();
            m_ui->m_clapFace->movie()->deleteLater();
            m_ui->m_centralStack->setCurrentWidget(m_ui->m_mainWidget);
        });
    }
}

void MainWindow::onWatchPressed()
{
    setCurrentMainWidget(streamWidget());
}

void MainWindow::onBrowsePressed()
{
    setCurrentMainWidget(browserWidget());
    browserWidget()->setCurrentBrowser(browserWidget()->gameBrowser());
    browserWidget()->showTopGames();
}

#include <QPropertyAnimation>

void MainWindow::onSettingsPressed()
{
    if (m_ui->m_settingsWidget->isHidden()) {
        m_ui->m_settingsWidget->show();
        QTimer::singleShot(0, [sw = m_ui->m_settingsWidget]() {
            QPropertyAnimation* anim = new QPropertyAnimation(sw, "pos");
            anim->setStartValue(QPoint(sw->pos().x() + sw->width(), sw->pos().y()));
            anim->setEndValue(QPoint(sw->pos().x(), sw->pos().y()));
            anim->setDuration(600);
            anim->setEasingCurve(QEasingCurve::OutBack);
            anim->start(QAbstractAnimation::DeletionPolicy::DeleteWhenStopped);
        });
    } else {
        m_ui->m_settingsWidget->hide();
    }
}

#include <QPushButton>

void MainWindow::onStreamEntered(const Twitch::Stream& stream)
{
    navigationBar()->watchButton()->setEnabled(true);
    setCurrentMainWidget(streamWidget());
    streamWidget()->openStream(stream);
}

void MainWindow::onEnteredFullscreenMode()
{
    showFullScreen();
    navigationBar()->hide();
    statusBar()->hide();

    removeMargins();
}

void MainWindow::onLeftFullscreenMode()
{
    showNormal();
    navigationBar()->show();
    statusBar()->show();

    restoreDefaultMargins();
}
