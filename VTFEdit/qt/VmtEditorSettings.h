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

#pragma once

#include <QFont>
#include <QGuiApplication>
#include <QString>
#include <QStringList>
#include <QStyleHints>

namespace VTFEdit
{
	enum class VmtEditorTheme
	{
		System,
		Light,
		Dark,
	};

	struct VmtEditorSettings
	{
		QString sFontFamily = QStringLiteral("Consolas");
		int iFontSize = 10;
		int iTabSize = 4;
		VmtEditorTheme eTheme = VmtEditorTheme::System;

		bool isDark() const
		{
			switch(eTheme)
			{
			case VmtEditorTheme::Light:
				return false;
			case VmtEditorTheme::Dark:
				return true;
			default:
				return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
			}
		}

		QFont font() const
		{
			QFont Font(sFontFamily, iFontSize);
			Font.setFamilies({ sFontFamily, QStringLiteral("Consolas"), QStringLiteral("Cascadia Mono"),
				QStringLiteral("DejaVu Sans Mono"), QStringLiteral("Courier New") });
			Font.setStyleHint(QFont::Monospace, QFont::StyleStrategy(QFont::PreferDefault | QFont::NoFontMerging));
			Font.setFixedPitch(true);
			return Font;
		}
	};
}
