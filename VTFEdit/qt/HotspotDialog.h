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

#include "RectFile.h"

#include <QDialog>
#include <QImage>
#include <QRect>
#include <QVector>
#include <QWidget>

class QCheckBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QTreeWidget;

namespace VTFEdit
{
	class HotspotPreview : public QWidget
	{
		Q_OBJECT

	public:
		explicit HotspotPreview(QWidget *pParent = nullptr);

		void setImage(const QImage &Image, int iWidth, int iHeight);

		// owned by dialog
		void setRectangles(const QVector<RectEntry> *pRectangles);
		void setSelected(int iIndex);

		void setSnap(int iSnap);

		void resetView();

	signals:
		void selectionRequested(int iIndex);
		void rectangleEdited(int iIndex, const QRect &Rectangle);
		void rectangleCreated(const QRect &Rectangle);

	protected:
		void paintEvent(QPaintEvent *pEvent) override;
		void mousePressEvent(QMouseEvent *pEvent) override;
		void mouseMoveEvent(QMouseEvent *pEvent) override;
		void mouseReleaseEvent(QMouseEvent *pEvent) override;
		void wheelEvent(QWheelEvent *pEvent) override;
		void leaveEvent(QEvent *pEvent) override;

	private:
		enum Drag
		{
			DragNone,
			DragCreate,
			DragMove,
			DragResize,
			DragPan,
		};

		enum Edge
		{
			EdgeLeft = 1,
			EdgeTop = 2,
			EdgeRight = 4,
			EdgeBottom = 8,
		};

		// where the texture is drawn
		QRectF target() const;
		qreal scale() const;

		QPointF clampPan(const QPointF &Pan) const;

		QRectF toWidget(const QRect &Rectangle) const;
		QPoint toImage(const QPoint &Position) const;
		QPointF toImageF(const QPointF &Position) const;

		int snap(int iValue, bool bSnapping) const;
		QRect clamp( const QRect& Rectangle ) const;

		int hitTest(const QPoint &Position, int &iEdges) const;

		void updateCursor(const QPoint &Position);

		QImage m_Image;
		int m_iImageWidth;
		int m_iImageHeight;

		const QVector<RectEntry> *m_pRectangles;
		int m_iSelected;

		int m_iSnap;

		qreal m_fZoom;
		QPointF m_Pan;

		Drag m_Drag;
		int m_iDragEdges;
		QPoint m_DragOrigin;
		QRect m_DragStart;
		QRect m_DragRectangle;

		QPoint m_PanStart;
		QPointF m_PanOrigin;
	};

	class HotspotDialog : public QDialog
	{
		Q_OBJECT

	public:
		HotspotDialog(const RectFile &Rectangles, const QImage &Image,
			int iImageWidth, int iImageHeight, QWidget *pParent = nullptr);

		const RectFile &rectangles() const { return m_Rectangles; }

	private slots:
		void onSelectionChanged();
		void onAdd();
		void onDuplicate();
		void onRemove();
		void onMoveUp();
		void onMoveDown();
		void onGenerateGrid();
		void onRectangleEdited();
		void onPreviewSelection(int iIndex);
		void onPreviewEdited(int iIndex, const QRect &Rectangle);
		void onPreviewCreated(const QRect &Rectangle);
		void onSnapChanged(int iSnap);

	private:
		QWidget *createEditor();

		void rebuildList(int iSelect);
		void updateItem(int iIndex);
		void updateEditor();
		void updateTileWarning();
		void updateButtons();

		int selectedRectangle() const;
		void move(int iDirection);

		RectFile m_Rectangles;
		QImage m_Image;
		int m_iImageWidth;
		int m_iImageHeight;
		bool m_bUpdating;

		QTreeWidget *m_pList;
		HotspotPreview *m_pPreview;

		QPushButton *m_pDuplicateButton;
		QPushButton *m_pRemoveButton;
		QPushButton *m_pMoveUpButton;
		QPushButton *m_pMoveDownButton;

		QWidget *m_pEditor;
		QSpinBox *m_pX;
		QSpinBox *m_pY;
		QSpinBox *m_pWidth;
		QSpinBox *m_pHeight;
		QCheckBox *m_pRotate;
		QCheckBox *m_pReflect;
		QCheckBox *m_pAlt;
		QCheckBox *m_pTile;
		QLabel *m_pTileWarning;
		QSpinBox *m_pSnap;
		QLabel *m_pHint;
	};
}
