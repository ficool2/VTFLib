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

#include "ImageView.h"

#include <QBrush>
#include <QColor>
#include <QMouseEvent>
#include <QPainter>
#include <QTransform>

namespace VTFEdit
{
	namespace
	{
		const QImage &checkerboardPattern()
		{
			static const QImage Pattern = []
			{
				QImage Image(16, 16, QImage::Format_RGB32);
				Image.fill(QColor(255, 255, 255));

				{
					QPainter Painter(&Image);
					Painter.fillRect(0, 0, 8, 8, QColor(191, 191, 191));
					Painter.fillRect(8, 8, 8, 8, QColor(191, 191, 191));
				}

				return Image;
			}();

			return Pattern;
		}
	}

	ImageView::ImageView(QWidget *pParent)
		: QWidget(pParent)
		, m_bTiled(false)
		, m_fScale(1.0f)
		, m_bCheckerboard(false)
	{
		setMouseTracking(true);
		setContextMenuPolicy(Qt::CustomContextMenu);
	}

	void ImageView::setImage(const QImage &Image)
	{
		m_Image = Image;
		updateGeometryForImage();
		update();
	}

	void ImageView::setScale(float fScale)
	{
		if(m_fScale == fScale)
		{
			return;
		}

		m_fScale = fScale;
		updateGeometryForImage();
		update();
	}

	void ImageView::setCheckerboard(bool bCheckerboard)
	{
		if(m_bCheckerboard == bCheckerboard)
		{
			return;
		}

		m_bCheckerboard = bCheckerboard;
		update();
	}

	QSize ImageView::displaySize() const
	{
		if(m_Image.isNull())
		{
			return QSize(0, 0);
		}

		return QSize(qMax(1, static_cast<int>(static_cast<float>(m_Image.width()) * m_fScale)),
			qMax(1, static_cast<int>(static_cast<float>(m_Image.height()) * m_fScale)));
	}

	void ImageView::setTiled(bool bTiled)
	{
		if(m_bTiled == bTiled)
		{
			return;
		}

		m_bTiled = bTiled;
		updateGeometryForImage();
		update();
	}

	void ImageView::updateGeometryForImage()
	{
		const int iTiles = m_bTiled ? 2 : 1;
		const QSize Display = displaySize();
		const QSize Size = m_Image.isNull()
			? QSize(0, 0)
			: QSize(Display.width() * iTiles + margin() * 2,
				Display.height() * iTiles + margin() * 2);

		setFixedSize(Size);
	}

	void ImageView::paintEvent(QPaintEvent *pEvent)
	{
		if(m_Image.isNull())
		{
			return;
		}

		QPainter Painter(this);

		QBrush Checkerboard;
		if(m_bCheckerboard)
		{
			Checkerboard = QBrush(checkerboardPattern());
		}

		const QSize Display = displaySize();
		const int iTiles = m_bTiled ? 2 : 1;

		for(int j = 0; j < iTiles; j++)
		{
			for(int i = 0; i < iTiles; i++)
			{
				const QRect Target(margin() + i * Display.width(), margin() + j * Display.height(),
					Display.width(), Display.height());

				const QRect Clip = Target.intersected(pEvent->rect());
				if(Clip.isEmpty())
				{
					continue;
				}

				if(m_bCheckerboard)
				{
					// checker board is in screen space
					Checkerboard.setTransform(QTransform::fromTranslate(Target.x(), Target.y()));
					Painter.fillRect(Clip, Checkerboard);
				}

				// only rasterize the clipped region
				Painter.save();
				Painter.setClipRect(Clip);
				Painter.translate(Target.topLeft());
				Painter.scale(static_cast<qreal>(Display.width()) / m_Image.width(),
					static_cast<qreal>(Display.height()) / m_Image.height());
				Painter.drawImage(0, 0, m_Image);
				Painter.restore();
			}
		}
	}

	void ImageView::mouseMoveEvent(QMouseEvent *pEvent)
	{
		if(!m_Image.isNull())
		{
			const QPoint Position = pEvent->position().toPoint() - QPoint(margin(), margin());
			const QSize Display = displaySize();
			const int iTiles = m_bTiled ? 2 : 1;

			if(Position.x() >= 0 && Position.y() >= 0
				&& Position.x() < Display.width() * iTiles
				&& Position.y() < Display.height() * iTiles)
			{
				emit mouseMovedOverImage(Position.x() % Display.width(), Position.y() % Display.height());
			}
		}

		QWidget::mouseMoveEvent(pEvent);
	}
}
