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

#include <QByteArray>
#include <QPair>
#include <QRect>
#include <QString>
#include <QVector>

namespace VTFEdit
{
	// Represents a .rect file
	// See https://developer.valvesoftware.com/wiki/Hotspot_texturing

	enum TileAxis
	{
		TileAxisNone,
		TileAxisX,
		TileAxisY,
		TileAxisBoth,
	};

	struct RectEntry
	{
		// top left origin
		QRect Rectangle;

		bool bRotate;
		bool bReflect;
		bool bAlt;
		bool bTile;

		RectEntry()
			: bRotate(false)
			, bReflect(false)
			, bAlt(false)
			, bTile(false)
		{
		}
	};

	class RectFile
	{
	public:
		bool load(const QByteArray &Data, QString *psError = nullptr);
		QByteArray save() const;

		bool isEmpty() const { return m_Rectangles.isEmpty(); }

		QVector<RectEntry> &rectangles() { return m_Rectangles; }
		const QVector<RectEntry> &rectangles() const { return m_Rectangles; }

		static QString pathForTexture(const QString &sFileName);

		static TileAxis tileAxis(const QRect &Rectangle, int iImageWidth, int iImageHeight);

	private:
		QVector<RectEntry> m_Rectangles;
	};
}
