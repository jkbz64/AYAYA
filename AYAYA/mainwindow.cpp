#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QMovie>
#include <QPropertyAnimation>
#include <QStandardPaths>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    // Save default margins/ spacing for going back from fulllscreen mode
    m_defaultMargin = m_ui->m_mainWidget->layout()->margin();
    m_defaultSpacing = m_ui->m_mainWidget->layout()->spacing();
    // Splash screen clap face
    m_ui->m_clapFace->setMovie(new QMovie(":/gifs/sunwithfaceclap.gif", QByteArray(), this));
    // Navigation bar
    connect(navigationBar(), &MainNavigationBar::browseButtonPressed, this, &MainWindow::onBrowsePressed);
    connect(navigationBar(), &MainNavigationBar::quitButtonPressed, this, &MainWindow::close);
    // Browser
    connect(browserWidget(), &BrowserWidget::streamEntered, this, &MainWindow::onStreamEntered);
    // Stream
    connect(streamWidget(), &StreamWidget::enteredTheaterMode, this, &MainWindow::onEnteredFullscreenMode);
    connect(streamWidget(), &StreamWidget::enteredFullscreenMode, this, &MainWindow::onEnteredFullscreenMode);
    connect(streamWidget(), &StreamWidget::leftFullscreenMode, this, &MainWindow::onLeftFullscreenMode);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::init()
{
    m_initQueue << m_ui->m_browserWidget << m_ui->m_streamWidget;
    initNextWidget();
}

MainNavigationBar* MainWindow::navigationBar() const
{
    return m_ui->m_navigationBar;
}

void MainWindow::setCurrentMainWidget(QWidget* widget) const
{
    m_ui->m_mainStack->setCurrentWidget(widget);
}

QWidget* MainWindow::currentMainWidget() const
{
    return m_ui->m_mainStack->currentWidget();
}

BrowserWidget* MainWindow::browserWidget() const
{
    return m_ui->m_browserWidget;
}

StreamWidget* MainWindow::streamWidget() const
{
    return m_ui->m_streamWidget;
}

void MainWindow::setupInitWidget(InitWidget* widget)
{
    connect(widget, &InitWidget::startedIniting, this, &MainWindow::onInitStarted);
    // Queued Connection lets us see some init progress logs ;)
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
    m_ui->m_mainWidget->layout()->setMargin(0);
    m_ui->m_mainWidget->layout()->setSpacing(0);
}

void MainWindow::restoreDefaultMargins()
{
    m_ui->m_mainWidget->layout()->setMargin(m_defaultMargin);
    m_ui->m_mainWidget->layout()->setSpacing(m_defaultSpacing);
}

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

    if (!m_initQueue.empty()) {
        initNextWidget();
    } else {
        QTimer::singleShot(250, this, [this]() {
            m_ui->m_statusLabel->setText("AYAYA is now starting");
        });
        // Let the user see the pretty sun... even if it's for a second :)
        QTimer::singleShot(1000, this, [this]() {
            m_ui->m_clapFace->movie()->stop();
            m_ui->m_centralStack->setCurrentWidget(m_ui->m_mainWidget);
        });
    }
}

void MainWindow::onBrowsePressed()
{
    setCurrentMainWidget(browserWidget());
}

void MainWindow::onStreamEntered(const Twitch::User& user, const Twitch::Stream& stream)
{
    setCurrentMainWidget(streamWidget());
    streamWidget()->initialize(user, stream);
}

void MainWindow::onEnteredFullscreenMode()
{
    showFullScreen();
    navigationBar()->hide();
    m_ui->m_horizontalLine->hide();
    statusBar()->hide();

    removeMargins();
}

void MainWindow::onLeftFullscreenMode()
{
    showNormal();
    navigationBar()->show();
    m_ui->m_horizontalLine->show();
    statusBar()->show();

    restoreDefaultMargins();
}
