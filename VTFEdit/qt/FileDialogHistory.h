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

#include <QDir>
#include <QFileInfo>
#include <QString>

namespace VTFEdit
{
	namespace FileDialogHistory
	{
		inline QString s_sFileDirectory; // vtf and vmt
		inline QString s_sImageDirectory; // imported / exported images

		// the path handed to file dialog
		inline QString path(const QString &sDirectory, const QString &sSuggestedName)
		{
			if(sSuggestedName.isEmpty())
			{
				return sDirectory;
			}

			if(sDirectory.isEmpty() || QFileInfo(sSuggestedName).isAbsolute())
			{
				return sSuggestedName;
			}

			return QDir(sDirectory).filePath(sSuggestedName);
		}

		inline void remember(QString &sDirectory, const QString &sFileName)
		{
			const QString sPath = QFileInfo(sFileName).absolutePath();

			if(!sPath.isEmpty())
			{
				sDirectory = sPath;
			}
		}
	}
}
