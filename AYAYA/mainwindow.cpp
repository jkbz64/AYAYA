#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QDebug>

#include <QMovie>
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    // Browse
    connect(m_ui->m_browseButton, &QPushButton::released, this, [this]() {
        m_ui->m_mainStack->setCurrentWidget(m_ui->m_browserWidget);
    });
    connect(m_ui->m_browseButton, &QPushButton::released, m_ui->m_browserWidget, &BrowserWidget::showTopGames);

    //Settings
    connect(m_ui->m_settingsButton, &QPushButton::released, this, [this]() {
        // m_ui->m_centralStack->setCurrentWidget(m_ui->m_settingsWidget);
    });

    connect(m_ui->m_browserWidget, &BrowserWidget::streamEntered, this, &MainWindow::onStreamEntered);

    connect(m_ui->m_streamWidget, &StreamWidget::enteredTheaterMode, this, &MainWindow::onEnteredTheaterMode);
    connect(m_ui->m_streamWidget, &StreamWidget::enteredFullscreenMode, this, &MainWindow::onEnteredFullscreenMode);
    connect(m_ui->m_streamWidget, &StreamWidget::leftTheaterMode, this, &MainWindow::onLeftTheaterMode);
    connect(m_ui->m_streamWidget, &StreamWidget::leftFullscreenMode, this, &MainWindow::onLeftFullscreenMode);

    m_ui->m_browserWidget->showTopGames();
}

MainWindow::~MainWindow()
{
    delete m_ui;
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
