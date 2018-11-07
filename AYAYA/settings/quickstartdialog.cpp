#include "quickstartdialog.hpp"
#include "../player/extractors/ytdlextractor.hpp"
#include "ui_quickstartdialog.h"

namespace {
using PlayerBackend = PlayerWidget::Backend;
using ExtractorBackend = PlayerWidget::ExtractorBackend;
}

QuickstartDialog::QuickstartDialog(QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::QuickstartDialog())
{
    m_ui->setupUi(this);

    connect(m_ui->m_okButton, &QPushButton::pressed, this, &QuickstartDialog::accept);
    connect(m_ui->m_skipButton, &QPushButton::pressed, this, &QuickstartDialog::reject);

#ifdef MPV
    m_ui->m_mpvRadioButton->setEnabled(true);
#endif
#ifdef VLC
    m_ui->m_vlcRadioButton->setEnabled(true);
#endif
}

QuickstartDialog::~QuickstartDialog()
{
    delete m_ui;
}

Theme QuickstartDialog::selectedTheme() const
{
    return m_ui->m_defaultThemeRadioButton->isChecked() ? Theme::Default : Theme::Dark;
}

PlayerBackend QuickstartDialog::selectedPlayerBackend() const
{
    if (m_ui->m_mpvRadioButton->isChecked())
        return PlayerBackend::Mpv;
    else if (m_ui->m_vlcRadioButton->isChecked())
        return PlayerBackend::Vlc;
    return PlayerBackend::Null;
}

ExtractorBackend QuickstartDialog::selectedExtractorBackend() const
{
    if (m_ui->m_ytdlRadioButton->isChecked())
        return ExtractorBackend::Ytdl;
    else if (m_ui->m_streamlinkRadioButton->isChecked())
        return ExtractorBackend::Streamlink;
    return ExtractorBackend::Null;
}
