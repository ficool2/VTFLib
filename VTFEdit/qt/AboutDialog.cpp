/*
 * VTFEdit
 * Copyright (C) 2005-2026 ficool2, Neil Jedrzejewski & Ryan Gregg
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "AboutDialog.h"

#include "VTFLibQt.h"

#include <QtGlobal>

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>

namespace VTFEdit
{
	AboutDialog::AboutDialog(QWidget *pParent)
		: QDialog(pParent)
	{
		setWindowTitle(tr("About VTFEdit++"));

		const QIcon AppIcon(QStringLiteral(":/app.ico"));
		setWindowIcon(AppIcon);

		QGroupBox *pAbout = new QGroupBox(this);
		QHBoxLayout *pAboutRow = new QHBoxLayout(pAbout);

		QLabel *pIcon = new QLabel(pAbout);
		pIcon->setPixmap(AppIcon.pixmap(64, 64));
		pIcon->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

		QVBoxLayout *pAboutLayout = new QVBoxLayout();

		QLabel *pVersion = new QLabel(tr("VTFEdit++ Version: 4.3.0"), pAbout);
		////QLabel *pLibVersion = new QLabel(
		//	tr("VTFLib Version: %1").arg(QString::fromLatin1(vlGetVersionString())), pAbout);
		QLabel *pAuthor = new QLabel(
			tr("Authors: ficool2, misyltoad, Neil Jedrzejewski & Ryan Gregg"), pAbout);
		QLabel *pLanguage = new QLabel(tr("Written In: C++"), pAbout);
		QLabel *pBuildDate = new QLabel(
			tr("Date: %1").arg(QString::fromLatin1(__DATE__)),
			pAbout);

		QLabel *pLinks = new QLabel(pAbout);
		pLinks->setTextFormat(Qt::RichText);
		pLinks->setOpenExternalLinks(true);
		pLinks->setText(QStringLiteral(
			"<a href=\"https://ficool2.github.io/HammerPlusPlus-Website\">https://ficool2.github.io/HammerPlusPlus-Website</a><br>"));

		pAboutLayout->addWidget(pVersion);
		//pAboutLayout->addWidget(pLibVersion);
		pAboutLayout->addWidget(pAuthor);
		pAboutLayout->addWidget(pLanguage);
		pAboutLayout->addWidget(pBuildDate);
		pAboutLayout->addWidget(pLinks);

		QLabel *pQt = new QLabel(pAbout);
		pQt->setTextFormat(Qt::RichText);
		pQt->setOpenExternalLinks(true);
		pQt->setWordWrap(true);
		pQt->setMinimumWidth(360);
		pQt->setText(tr(
			"This program uses Qt %1, which is licensed under the GNU Lesser General "
			"Public License version 3. See <a href=\"https://www.qt.io/licensing/\">"
			"qt.io/licensing</a> for details.").arg(QStringLiteral(QT_VERSION_STR)));

		pAboutLayout->addWidget(pQt);

		pAboutRow->addWidget(pIcon);
		pAboutRow->addLayout(pAboutLayout);

		QDialogButtonBox *pButtons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
		connect(pButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);

		QVBoxLayout *pLayout = new QVBoxLayout(this);
		pLayout->addWidget(pAbout);
		pLayout->addWidget(pButtons);
	}
}
