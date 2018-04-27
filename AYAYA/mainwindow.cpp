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

    // Following
    connect(m_ui->m_followedButton, &QPushButton::released, this, [this]() {
        m_ui->m_mainStack->setCurrentWidget(m_ui->m_browserWidget);
    });
    // TODO

    //Settings
    connect(m_ui->m_settingsButton, QPushButton::released, this, [this]() {
        // m_ui->m_centralStack->setCurrentWidget(m_ui->m_settingsWidget);
    });

    connect(m_ui->m_browserWidget, &BrowserWidget::streamEntered, this, &MainWindow::onStreamEntered);

    m_ui->m_browserWidget->showTopGames();
}

MainWindow::~MainWindow()
{
}

void MainWindow::onStreamEntered(const Twitch::User& user, const Twitch::Stream& stream)
{
    m_ui->m_mainStack->setCurrentWidget(m_ui->m_livestreamWidget);
    m_ui->m_livestreamWidget->initialize(user, stream);
}
