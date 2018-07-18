#include "quickstartdialog.hpp"

#include "ui_quickstartdialog.h"

QuickstartDialog::QuickstartDialog(QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::QuickstartDialog())
{
    m_ui->setupUi(this);

    connect(m_ui->m_okButton, &QPushButton::pressed, this, &QuickstartDialog::accept);
    connect(m_ui->m_skipButton, &QPushButton::pressed, this, &QuickstartDialog::reject);

    connect(m_ui->m_defaultThemeRadioButton, &QRadioButton::pressed, this, &QuickstartDialog::checkValidity);
    connect(m_ui->m_darkThemeRadioButton, &QRadioButton::pressed, this, &QuickstartDialog::checkValidity);

    connect(m_ui->m_mpvRadioButton, &QRadioButton::pressed, this, &QuickstartDialog::checkValidity);
    connect(m_ui->m_vlcRadioButton, &QRadioButton::pressed, this, &QuickstartDialog::checkValidity);
    connect(m_ui->m_nullRadioButton, &QRadioButton::pressed, this, &QuickstartDialog::checkValidity);

    connect(m_ui->m_ytdlButton, &QPushButton::pressed, this, &QuickstartDialog::onYtdlPressed);
    connect(m_ui->m_streamlinkButton, &QPushButton::pressed, this, &QuickstartDialog::onStreamlinkPressed);
}

void QuickstartDialog::onYtdlPressed()
{
}

void QuickstartDialog::onStreamlinkPressed()
{
}

void QuickstartDialog::checkValidity()
{
    const auto isThemeSelected = m_ui->m_defaultThemeRadioButton->isChecked() || m_ui->m_darkThemeRadioButton->isChecked();
    const auto isExtractorSelected = true;
    const auto isPlayerBackendSelected = m_ui->m_mpvRadioButton->isChecked() || m_ui->m_vlcRadioButton->isChecked() || m_ui->m_nullRadioButton->isChecked();
    if (isThemeSelected && isExtractorSelected && isPlayerBackendSelected)
        m_ui->m_okButton->setEnabled(true);
}
