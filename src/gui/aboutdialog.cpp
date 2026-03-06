/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2006  Christophe Dumez <chris@qbittorrent.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If you
 * modify file(s), you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version.
 */

#include "aboutdialog.h"

#include <QClipboard>

#include "base/global.h"
#include "base/path.h"
#include "base/unicodestrings.h"
#include "base/utils/io.h"
#include "base/utils/misc.h"
#include "base/version.h"
#include "ui_aboutdialog.h"
#include "uithememanager.h"
#include "utils.h"

#define SETTINGS_KEY(name) u"AboutDialog/" name

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::AboutDialog)
    , m_storeDialogSize(SETTINGS_KEY(u"Size"_s))
{
    m_ui->setupUi(this);

    // Title
    m_ui->labelName->setText(QStringLiteral("<b><h2>qBittorrent " QBT_VERSION " (%1-bit)</h2></b>").arg(QT_POINTER_SIZE * 8));

    m_ui->logo->setPixmap(UIThemeManager::instance()->getScaledPixmap(u"qbittorrent-tray"_s, 32));

    // About
    const QString aboutText =
        u"<p style=\"white-space: pre-wrap;\">"
        u"%1\n\n"
        u"%2\n\n"
        u"<table>"
        u"<tr><td>%3</td><td><a href=\"https://www.qbittorrent.org\">https://www.qbittorrent.org</a></td></tr>"
        u"<tr><td>%4</td><td><a href=\"https://forum.qbittorrent.org\">https://forum.qbittorrent.org</a></td></tr>"
        u"<tr><td>%5</td><td><a href=\"https://bugs.qbittorrent.org\">https://bugs.qbittorrent.org</a></td></tr>"
        u"</table>"
        u"</p>"_s
        .arg(tr("An advanced BitTorrent client programmed in C++, based on Qt toolkit and libtorrent-rasterbar.")
                .replace(u"C++"_s, u"C\u2060+\u2060+"_s) // make C++ non-breaking
            , tr("Copyright %1 2006-2025 The qBittorrent project").arg(C_COPYRIGHT)
            , tr("Home Page:")
            , tr("Forum:")
            , tr("Bug Tracker:"));
    m_ui->labelAbout->setText(aboutText);

    m_ui->labelMascot->setPixmap(Utils::Gui::scaledPixmap(Path(u":/icons/mascot.png"_s)));

    // Thanks
    if (const auto readResult = Utils::IO::readFile(Path(u":/thanks.html"_s), -1, QIODevice::Text))
    {
        m_ui->textBrowserThanks->setHtml(QString::fromUtf8(readResult.value()));
    }

    // Translation
    if (const auto readResult = Utils::IO::readFile(Path(u":/translators.html"_s), -1, QIODevice::Text))
    {
        m_ui->textBrowserTranslation->setHtml(QString::fromUtf8(readResult.value()));
    }

    // License
    if (const auto readResult = Utils::IO::readFile(Path(u":/gpl.html"_s), -1, QIODevice::Text))
    {
        m_ui->textBrowserLicense->setHtml(QString::fromUtf8(readResult.value()));
    }

    // Custom Build
    const QString customBuildHtml = QString::fromLatin1(
        "<html><body style=\"font-family: sans-serif; margin: 12px;\">"
        "<h2>Custom Build Patch Notes</h2>"
        "<p style=\"color: #666;\">Based on qBittorrent " QBT_VERSION " &mdash; custom modifications applied.</p>"
        "<hr/>"
        "<h3>&#x2705; Auto-complete status after move to finished folder</h3>"
        "<p>When a torrent finishes downloading and is moved to the completed folder, "
        "the torrent is automatically stopped so the status changes to "
        "<b>Completed</b> instead of remaining as <i>Seeding</i>.</p>"
        "<ul>"
        "<li>Handles both immediate completion and cases where the move was already in progress</li>"
        "<li>State is refreshed in the UI immediately after the move finishes</li>"
        "</ul>"
        "<hr/>"
        "<h3>&#x2705; Minimum file size filter in Add Torrent dialog</h3>"
        "<p>The <i>Add New Torrent</i> dialog now has a size filter control next to the "
        "<b>Select All</b> / <b>Select None</b> buttons.</p>"
        "<ul>"
        "<li>Checkbox to enable / disable the filter</li>"
        "<li>Text box to enter a size threshold value</li>"
        "<li>Drop-down to choose unit: <b>KB / MB / GB / TB</b></li>"
        "<li>When enabled, files &ge; the threshold are checked automatically; "
        "smaller files are unchecked</li>"
        "<li>Works recursively through all folder levels in the file tree</li>"
        "<li>Settings (enabled state, value, unit) are <b>saved and restored</b> across restarts</li>"
        "<li>Filter is <b>re-applied automatically every time</b> the Add Torrent dialog opens</li>"
        "<li>Also works for magnet links once metadata is received</li>"
        "</ul>"
        "</body></html>"
    );
    m_ui->textBrowserCustomBuild->setHtml(customBuildHtml);

    // Software Used
    m_ui->labelQtVer->setText(QStringLiteral(QT_VERSION_STR));
    m_ui->labelLibtVer->setText(Utils::Misc::libtorrentVersionString());
    m_ui->labelBoostVer->setText(Utils::Misc::boostVersionString());
    m_ui->labelOpensslVer->setText(Utils::Misc::opensslVersionString());
    m_ui->labelZlibVer->setText(Utils::Misc::zlibVersionString());

    connect(m_ui->btnCopyToClipboard, &QAbstractButton::clicked, this, &AboutDialog::copyVersionsToClipboard);

    const QString DBIPText = u"<html><head/><body><p>"
                             u"%1 (<a href=\"https://db-ip.com/\">https://db-ip.com/</a>)"
                             u"</p></body></html>"_s
                             .arg(tr("The free IP to Country Lite database by DB-IP is used for resolving the countries of peers. "
                                     "The database is licensed under the Creative Commons Attribution 4.0 International License"));
    m_ui->labelDBIP->setText(DBIPText);

    if (const QSize dialogSize = m_storeDialogSize; dialogSize.isValid())
        resize(dialogSize);
}

AboutDialog::~AboutDialog()
{
    m_storeDialogSize = size();
    delete m_ui;
}

void AboutDialog::copyVersionsToClipboard() const
{
    const QString versions = u"%1 %2\n%3 %4\n%5 %6\n%7 %8\n%9 %10\n"_s
        .arg(m_ui->labelQt->text(), m_ui->labelQtVer->text()
            , m_ui->labelLibt->text(), m_ui->labelLibtVer->text()
            , m_ui->labelBoost->text(), m_ui->labelBoostVer->text()
            , m_ui->labelOpenssl->text(), m_ui->labelOpensslVer->text()
            , m_ui->labelZlib->text(), m_ui->labelZlibVer->text());
    qApp->clipboard()->setText(versions);
}
