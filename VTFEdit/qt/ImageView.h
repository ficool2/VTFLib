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

#include <QImage>
#include <QWidget>

namespace VTFEdit
{
	class ImageView : public QWidget
	{
		Q_OBJECT

	public:
		explicit ImageView(QWidget *pParent = nullptr);

		void setImage(const QImage &Image);
		const QImage &image() const { return m_Image; }

		void setTiled(bool bTiled);
		bool isTiled() const { return m_bTiled; }

		void setScale(float fScale);
		float scale() const { return m_fScale; }

		void setCheckerboard(bool bCheckerboard);
		bool hasCheckerboard() const { return m_bCheckerboard; }

		QSize displaySize() const;

		// padding around the image so it can be panned slightly past its borders
		static int margin() { return 64; }

		static int maximumDisplayDimension() { return (QWIDGETSIZE_MAX - margin() * 2) / 2; }

	signals:
		void mouseMovedOverImage(int iX, int iY);

	protected:
		void paintEvent(QPaintEvent *pEvent) override;
		void mouseMoveEvent(QMouseEvent *pEvent) override;

	private:
		void updateGeometryForImage();

		QImage m_Image;
		bool m_bTiled;
		float m_fScale;
		bool m_bCheckerboard;
	};
}
