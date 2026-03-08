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

    // Custom Build (Korean)
    const QString customBuildHtml = QString::fromUtf8(
        "<html><body style=\"font-family: sans-serif; margin: 12px;\">"
        "<h2>\ucee4\uc2a4\ud140 \ube4c\ub4dc \ud328\uce58 \ub178\ud2b8</h2>"
        "<p style=\"color: #666;\">qBittorrent " QBT_VERSION " \uae30\ubc18 &mdash; \ucee4\uc2a4\ud140 \uc218\uc815 \uc0ac\ud56d \uc801\uc6a9\ub428.</p>"
        "<hr/>"

        "<h3>&#x2705; \ub2e4\uc6b4\ub85c\ub4dc \uc644\ub8cc \ud6c4 \uc0c1\ud0dc \uc790\ub3d9 \ubcc0\uacbd</h3>"
        "<p>\ud1a0\ub82c\ud2b8 \ub2e4\uc6b4\ub85c\ub4dc\uac00 \uc644\ub8cc\ub418\uba74 \uc790\ub3d9\uc73c\ub85c \uc815\uc9c0\ub418\uc5b4 "
        "\uc0c1\ud0dc\uac00 <i>\uc2dc\ub4dc \uc911</i> \ub300\uc2e0 <b>\uc644\ub8cc</b>\ub85c \ud45c\uc2dc\ub429\ub2c8\ub2e4.</p>"
        "<ul>"
        "<li>\uc644\ub8cc \ud3f4\ub354\ub85c \uc774\ub3d9 \uc5ec\ubd80\uc640 \ubb34\uad00\ud558\uac8c \ud56d\uc0c1 \uc801\uc6a9</li>"
        "<li>\uc774\ub3d9 \uc644\ub8cc \uc989\uc2dc UI \uc0c1\ud0dc \uac31\uc2e0</li>"
        "</ul>"

        "<hr/>"

        "<h3>&#x2705; \ud1a0\ub82c\ud2b8 \ucd94\uac00 \ub300\ud654\uc0c1\uc790 \ud30c\uc77c \ud06c\uae30 \ud544\ud130</h3>"
        "<p><i>\ud1a0\ub82c\ud2b8 \ucd94\uac00</i> \ub300\ud654\uc0c1\uc790\uc758 <b>\uc804\uccb4 \uc120\ud0dd</b> / <b>\uc120\ud0dd \ud574\uc81c</b> "
        "\ubc84\ud2bc \uc606\uc5d0 \ud30c\uc77c \ud06c\uae30 \ud544\ud130 \ucf58\ud2b8\ub864\uc774 \ucd94\uac00\ub418\uc5c8\uc2b5\ub2c8\ub2e4.</p>"
        "<ul>"
        "<li>\uccb4\ud06c\ubc15\uc2a4: \ud544\ud130 \ud65c\uc131\ud654 / \ube44\ud65c\uc131\ud654</li>"
        "<li>\ud14d\uc2a4\ud2b8 \uc785\ub825: \uae30\uc900 \ud06c\uae30 \uac12 \uc785\ub825</li>"
        "<li>\ub2e8\uc704 \uc120\ud0dd: <b>KB / MB / GB / TB</b></li>"
        "<li>\ud65c\uc131\ud654\ub41c \uc0c1\ud0dc\uc5d0\uc11c \uae30\uc900 \ud06c\uae30 \uc774\uc0c1\uc778 \ud30c\uc77c\uc740 \uc790\ub3d9 \uccb4\ud06c, \ubbf8\ub9cc\uc740 \uccb4\ud06c \ud574\uc81c</li>"
        "<li>\ud3f4\ub354 \ud2b8\ub9ac \uc804\uccb4\uc5d0 \uc7ac\uadc0\uc801\uc73c\ub85c \uc801\uc6a9</li>"
        "<li>\uc124\uc815\uac12(\ud65c\uc131\ud654 \uc5ec\ubd80, \ud06c\uae30, \ub2e8\uc704) \uc571 \uc7ac\uc2dc\uc791 \ud6c4\uc5d0\ub3c4 \uc720\uc9c0</li>"
        "<li>\ub300\ud654\uc0c1\uc790\uac00 \uc5f4\ub9b4 \ub54c\ub9c8\ub2e4 \uc800\uc7a5\ub41c \uae30\uc900\uc73c\ub85c \uc790\ub3d9 \ud30c\uc77c \uc120\ud0dd</li>"
        "<li>\ub9c8\uadf8\ub137 \ub9c1\ud06c \uba54\ud0c0\ub370\uc774\ud130 \uc218\uc2e0 \ud6c4\uc5d0\ub3c4 \uc801\uc6a9</li>"
        "</ul>"

        "<hr/>"

"<h3>&#x2705; \ud1a0\ub82c\ud2b8 \ucd94\uac00 \ub300\ud654\uc0c1\uc790 \uc790\ub3d9 \ub2eb\ud798</h3>"
"<p>\ud06c\uae30 \ud544\ud130\uac00 \ud65c\uc131\ud654\ub41c \uc0c1\ud0dc\uc5d0\uc11c \ud1a0\ub82c\ud2b8 \ucd94\uac00 \ub300\ud654\uc0c1\uc790\uac00 \uc5f4\ub9ac\uba74, "
"\uc124\uc815\ud55c \uc2dc\uac04(\ucd08) \ud6c4 \uc790\ub3d9\uc73c\ub85c \ud655\uc778\ub429\ub2c8\ub2e4.</p>"
"<ul>"
"<li>OK \ubc84\ud2bc\uc5d0 \uce74\uc6b4\ud2b8\ub2e4\uc6b4 \ud45c\uc2dc</li>"
"<li>\ud658\uacbd\uc124\uc815 &gt; \ub2e4\uc6b4\ub85c\ub4dc &gt; '\ud1a0\ub82c\ud2b8 \ucd94\uac00 \uc2dc' \ud56d\ubaa9\uc5d0\uc11c \ud65c\uc131\ud654 \ubc0f \ub300\uae30 \uc2dc\uac04 \uc124\uc815 \uac00\ub2a5</li>"
"<li>\ub300\ud654\uc0c1\uc790 \ub0b4 \uc5b4\ub5a4 \uc124\uc815\uc744 \ubcc0\uacbd\ud558\uba74 \ud0c0\uc774\uba38\uac00 \uc989\uc2dc \uc911\ub2e8\ub428</li>"
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
