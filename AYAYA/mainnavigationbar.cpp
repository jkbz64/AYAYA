#include "mainnavigationbar.hpp"
#include "ui_mainnavigationbar.h"

MainNavigationBar::MainNavigationBar(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::MainNavigationBar)
{
    m_ui->setupUi(this);
    connect(m_ui->m_watchButton, &QPushButton::pressed, this, &MainNavigationBar::watchButtonPressed);
    connect(m_ui->m_browseButton, &QPushButton::pressed, this, &MainNavigationBar::browseButtonPressed);
    connect(m_ui->m_settingsButton, &QPushButton::pressed, this, &MainNavigationBar::settingsButtonPressed);
}

MainNavigationBar::~MainNavigationBar()
{
    delete m_ui;
}

QPushButton* MainNavigationBar::watchButton() const
{
    return m_ui->m_watchButton;
}

QPushButton* MainNavigationBar::browseButton() const
{
    return m_ui->m_browseButton;
}

QPushButton* MainNavigationBar::settingsButton() const
{
    return m_ui->m_settingsButton;
}
