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

#include "VtfOptions.h"

#include <QList>
#include <QString>
#include <QStringList>

namespace VTFEdit
{
	struct VtfPreset
	{
		QString sName;
		QString sDescription;
		bool bBuiltin = false;
		VtfOptions Options;
	};

	class VtfPresets
	{
	public:
		VtfPresets();

		const QList<VtfPreset> &presets() const { return m_Presets; }
		int count() const { return m_Presets.size(); }
		const VtfPreset *at(int iIndex) const;

		int indexOf(const QString &sName) const;
		int match(const VtfOptions &Options) const;

		int save(const QString &sName, const VtfOptions &Options);

		bool remove(int iIndex);

		bool isModified(int iIndex) const;
		bool hasModifiedBuiltins() const;

		void restoreDefaults();

		static void apply(const VtfPreset &Preset, VtfOptions &Options);
		static bool equals(const VtfOptions &A, const VtfOptions &B);

		static QString filePath();

		bool read(const QString &sFileName);
		bool write(const QString &sFileName) const;

		bool read() { return read(filePath()); }
		bool write() const { return write(filePath()); }

	private:
		static const QList<VtfPreset> &stockPresets();

		QList<VtfPreset> m_Presets;
		QStringList m_RemovedBuiltins;
	};
}
