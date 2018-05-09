#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QMovie>
#include <QPropertyAnimation>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    auto clapFace = new QMovie(":/gifs/sunwithfaceclap.gif", QByteArray(), this);
    clapFace->start();
    m_ui->m_clapFace->setMovie(clapFace);

    // Browse
    connect(m_ui->m_browseButton, &QPushButton::released, this, [this]() {
        m_ui->m_mainStack->setCurrentWidget(m_ui->m_browserWidget);
    });
    connect(m_ui->m_browseButton, &QPushButton::released, m_ui->m_browserWidget, &BrowserWidget::showTopGames);

    // TODO Settings
    /*connect(m_ui->m_settingsButton, &QPushButton::released, this, [this]() {
        // m_ui->m_centralStack->setCurrentWidget(m_ui->m_settingsWidget);
    });*/

    // Browser
    connect(m_ui->m_browserWidget, &BrowserWidget::streamEntered, this, &MainWindow::onStreamEntered);

    // Stream
    connect(m_ui->m_streamWidget, &StreamWidget::enteredTheaterMode, this, &MainWindow::onEnteredTheaterMode);
    connect(m_ui->m_streamWidget, &StreamWidget::enteredFullscreenMode, this, &MainWindow::onEnteredFullscreenMode);
    connect(m_ui->m_streamWidget, &StreamWidget::leftTheaterMode, this, &MainWindow::onLeftTheaterMode);
    connect(m_ui->m_streamWidget, &StreamWidget::leftFullscreenMode, this, &MainWindow::onLeftFullscreenMode);
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

void MainWindow::setupInitWidget(MainWidget* widget)
{
    connect(widget, &MainWidget::startedIniting, [this]() {
        m_ui->m_statusLabel->setText("Initing...");
    });

    connect(widget, &MainWidget::initProgress, this, &MainWindow::onInitProgress, Qt::QueuedConnection);

    connect(widget, &MainWidget::endedIniting, [this]() {
        if (!m_initQueue.empty()) {
            initNextWidget();
        } else {
            QTimer::singleShot(1000, this, [this]() {
                m_ui->m_centralStack->setCurrentWidget(m_ui->m_mainWidget);
            });
        }
    });
    emit widget->startedIniting();
}

void MainWindow::initNextWidget()
{
    if (!m_initQueue.empty()) {
        auto currentInitWidget = m_initQueue.front();
        m_initQueue.pop_front();
        setupInitWidget(currentInitWidget);
    }
}

void MainWindow::onInitProgress(const QString& progressText)
{
    m_ui->m_statusLabel->setText(m_ui->m_statusLabel->text() + progressText);
}

void MainWindow::onStreamEntered(const Twitch::User& user, const Twitch::Stream& stream)
{
    m_ui->m_mainStack->setCurrentWidget(m_ui->m_streamWidget);
    m_ui->m_streamWidget->initialize(user, stream);
}

void MainWindow::onEnteredTheaterMode()
{
    hide();
}

void MainWindow::onEnteredFullscreenMode()
{
    hide();
}

void MainWindow::onLeftTheaterMode()
{
    show();
    m_ui->m_streamWidget->setParent(this);
    m_ui->m_centralStack->addWidget(m_ui->m_streamWidget);
    m_ui->m_centralStack->setCurrentWidget(m_ui->m_streamWidget);
}

void MainWindow::onLeftFullscreenMode()
{
    show();
    m_ui->m_streamWidget->setParent(this);
    m_ui->m_centralStack->addWidget(m_ui->m_streamWidget);
    m_ui->m_centralStack->setCurrentWidget(m_ui->m_streamWidget);
}
