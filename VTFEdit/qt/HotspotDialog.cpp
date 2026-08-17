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

#include "HotspotDialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QSpinBox>
#include <QStringList>
#include <QTreeWidget>
#include <QWheelEvent>
#include <QVBoxLayout>
#include <QtMath>

namespace VTFEdit
{
	namespace
	{
		// how close to an edge counts as grabbing it
		const int HandleSize = 6;

		enum Column
		{
			ColumnIndex,
			ColumnX,
			ColumnY,
			ColumnWidth,
			ColumnHeight,
			ColumnFlags,
			ColumnCount,
		};

		const QColor WarningColor(232, 144, 0);

		bool isTileValid(const RectEntry &Entry, int iImageWidth, int iImageHeight)
		{
			return !Entry.bTile
				|| RectFile::tileAxis(Entry.Rectangle, iImageWidth, iImageHeight) != TileAxisNone;
		}

		QString tileFlagLabel(const RectEntry &Entry, int iImageWidth, int iImageHeight)
		{
			switch(RectFile::tileAxis(Entry.Rectangle, iImageWidth, iImageHeight))
			{
			case TileAxisX:		return HotspotDialog::tr("tile x");
			case TileAxisY:		return HotspotDialog::tr("tile y");
			case TileAxisBoth:	return HotspotDialog::tr("tile x, y");
			default:			return HotspotDialog::tr("tile (!)");
			}
		}

		QString tileWarning()
		{
			return HotspotDialog::tr("A tiling rectangle has to span the full width or the full height of the texture");
		}

		QStringList rectangleColumns(int iIndex, const RectEntry &Entry,
			int iImageWidth, int iImageHeight)
		{
			const QRect Rectangle = Entry.Rectangle.normalized();

			QStringList Flags;
			if(Entry.bRotate)
			{
				Flags.append(HotspotDialog::tr("rotate"));
			}
			if(Entry.bReflect)
			{
				Flags.append(HotspotDialog::tr("reflect"));
			}
			if(Entry.bAlt)
			{
				Flags.append(HotspotDialog::tr("alt"));
			}
			if(Entry.bTile)
			{
				Flags.append(tileFlagLabel(Entry, iImageWidth, iImageHeight));
			}

			QStringList Columns;
			Columns.append(QString::number(iIndex + 1));
			Columns.append(QString::number(Rectangle.left()));
			Columns.append(QString::number(Rectangle.top()));
			Columns.append(QString::number(Rectangle.width()));
			Columns.append(QString::number(Rectangle.height()));
			Columns.append(Flags.join(QLatin1String(", ")));

			return Columns;
		}

		void applyTileWarning(QTreeWidgetItem *pItem, const RectEntry &Entry,
			int iImageWidth, int iImageHeight)
		{
			const bool bValid = isTileValid(Entry, iImageWidth, iImageHeight);

			pItem->setForeground(ColumnFlags, bValid ? QBrush() : QBrush(WarningColor));

			for(int i = 0; i < ColumnCount; i++)
			{
				pItem->setToolTip(i, bValid ? QString() : tileWarning());
			}
		}

		QTreeWidgetItem *createRectangleItem(int iIndex, const RectEntry &Entry,
			int iImageWidth, int iImageHeight)
		{
			QTreeWidgetItem *pItem = new QTreeWidgetItem(
				rectangleColumns(iIndex, Entry, iImageWidth, iImageHeight));

			// the numbers read better lined up on their right edge
			for(int i = ColumnIndex; i < ColumnFlags; i++)
			{
				pItem->setTextAlignment(i, Qt::AlignRight | Qt::AlignVCenter);
			}

			applyTileWarning(pItem, Entry, iImageWidth, iImageHeight);

			return pItem;
		}
	}

	//
	// HotspotPreview.
	//

	HotspotPreview::HotspotPreview(QWidget *pParent)
		: QWidget(pParent)
		, m_iImageWidth(1)
		, m_iImageHeight(1)
		, m_pRectangles(nullptr)
		, m_iSelected(-1)
		, m_iSnap(0)
		, m_fZoom(1.0)
		, m_Drag(DragNone)
		, m_iDragEdges(0)
	{
		setMinimumSize(256, 256);
		setMouseTracking(true);
	}

	void HotspotPreview::setImage(const QImage &Image, int iWidth, int iHeight)
	{
		m_Image = Image;
		m_iImageWidth = iWidth > 0 ? iWidth : 1;
		m_iImageHeight = iHeight > 0 ? iHeight : 1;

		resetView();
	}

	void HotspotPreview::setSnap(int iSnap)
	{
		m_iSnap = qMax(0, iSnap);
	}

	void HotspotPreview::resetView()
	{
		m_fZoom = 1.0;
		m_Pan = QPointF();
		update();
	}

	void HotspotPreview::setRectangles(const QVector<RectEntry> *pRectangles)
	{
		m_pRectangles = pRectangles;
		update();
	}

	void HotspotPreview::setSelected(int iIndex)
	{
		m_iSelected = iIndex;
		update();
	}

	qreal HotspotPreview::scale() const
	{
		const qreal fFit = qMin(qreal(width()) / m_iImageWidth, qreal(height()) / m_iImageHeight);
		return fFit * m_fZoom;
	}

	QPointF HotspotPreview::clampPan(const QPointF &Pan) const
	{
		const qreal fScale = scale();


		const qreal fSlackX = qMax(qreal(0.0), (m_iImageWidth * fScale - width()) * 0.5);
		const qreal fSlackY = qMax(qreal(0.0), (m_iImageHeight * fScale - height()) * 0.5);

		return QPointF(qBound(-fSlackX, Pan.x(), fSlackX), qBound(-fSlackY, Pan.y(), fSlackY));
	}

	QRectF HotspotPreview::target() const
	{
		const qreal fScale = scale();

		const qreal fWidth = m_iImageWidth * fScale;
		const qreal fHeight = m_iImageHeight * fScale;

		const QPointF Pan = clampPan(m_Pan);

		return QRectF((width() - fWidth) * 0.5 + Pan.x(), (height() - fHeight) * 0.5 + Pan.y(),
			fWidth, fHeight);
	}

	QRectF HotspotPreview::toWidget(const QRect &Rectangle) const
	{
		const QRectF Target = target();
		const qreal fScaleX = Target.width() / m_iImageWidth;
		const qreal fScaleY = Target.height() / m_iImageHeight;

		const QRect Normalized = Rectangle.normalized();

		return QRectF(
			Target.left() + Normalized.left() * fScaleX,
			Target.top() + Normalized.top() * fScaleY,
			Normalized.width() * fScaleX,
			Normalized.height() * fScaleY);
	}

	QPoint HotspotPreview::toImage(const QPoint &Position) const
	{
		const QRectF Target = target();
		if(Target.width() <= 0.0 || Target.height() <= 0.0)
		{
			return QPoint();
		}

		return QPoint(
			qFloor((Position.x() - Target.left()) * m_iImageWidth / Target.width()),
			qFloor((Position.y() - Target.top()) * m_iImageHeight / Target.height()));
	}

	QPointF HotspotPreview::toImageF(const QPointF &Position) const
	{
		const QRectF Target = target();
		if(Target.width() <= 0.0 || Target.height() <= 0.0)
		{
			return QPointF();
		}

		return QPointF(
			(Position.x() - Target.left()) * m_iImageWidth / Target.width(),
			(Position.y() - Target.top()) * m_iImageHeight / Target.height());
	}

	int HotspotPreview::snap(int iValue, bool bSnapping) const
	{
		if(!bSnapping || m_iSnap <= 1)
		{
			return iValue;
		}

		return qRound(qreal(iValue) / m_iSnap) * m_iSnap;
	}

	QRect HotspotPreview::clamp(const QRect &Rectangle) const
	{
		QRect Result = Rectangle.normalized();

		Result.setLeft(qBound(0, Result.left(), m_iImageWidth - 1));
		Result.setTop(qBound(0, Result.top(), m_iImageHeight - 1));
		Result.setRight(qBound(Result.left(), Result.right(), m_iImageWidth - 1));
		Result.setBottom(qBound(Result.top(), Result.bottom(), m_iImageHeight - 1));

		return Result;
	}

	int HotspotPreview::hitTest(const QPoint &Position, int &iEdges) const
	{
		iEdges = 0;

		if(m_pRectangles == nullptr)
		{
			return -1;
		}

		// prioritize selected rectangle
		if(m_iSelected >= 0 && m_iSelected < m_pRectangles->count())
		{
			const QRectF Rectangle = toWidget(m_pRectangles->at(m_iSelected).Rectangle);
			const QRectF Grab = Rectangle.adjusted(-HandleSize, -HandleSize, HandleSize, HandleSize);

			if(Grab.contains(Position))
			{
				if(qAbs(Position.x() - Rectangle.left()) <= HandleSize)
				{
					iEdges |= EdgeLeft;
				}
				else if(qAbs(Position.x() - Rectangle.right()) <= HandleSize)
				{
					iEdges |= EdgeRight;
				}

				if(qAbs(Position.y() - Rectangle.top()) <= HandleSize)
				{
					iEdges |= EdgeTop;
				}
				else if(qAbs(Position.y() - Rectangle.bottom()) <= HandleSize)
				{
					iEdges |= EdgeBottom;
				}

				if(iEdges != 0)
				{
					return m_iSelected;
				}
			}
		}

		// topmost first
		for(int i = m_pRectangles->count() - 1; i >= 0; i--)
		{
			if(toWidget(m_pRectangles->at(i).Rectangle).contains(Position))
			{
				return i;
			}
		}

		return -1;
	}

	void HotspotPreview::updateCursor(const QPoint &Position)
	{
		int iEdges = 0;
		const int iIndex = hitTest(Position, iEdges);

		if(iEdges == (EdgeLeft | EdgeTop) || iEdges == (EdgeRight | EdgeBottom))
		{
			setCursor(Qt::SizeFDiagCursor);
		}
		else if(iEdges == (EdgeRight | EdgeTop) || iEdges == (EdgeLeft | EdgeBottom))
		{
			setCursor(Qt::SizeBDiagCursor);
		}
		else if(iEdges == EdgeLeft || iEdges == EdgeRight)
		{
			setCursor(Qt::SizeHorCursor);
		}
		else if(iEdges == EdgeTop || iEdges == EdgeBottom)
		{
			setCursor(Qt::SizeVerCursor);
		}
		else if(iIndex >= 0)
		{
			setCursor(Qt::SizeAllCursor);
		}
		else
		{
			setCursor(Qt::CrossCursor);
		}
	}

	void HotspotPreview::mousePressEvent(QMouseEvent *pEvent)
	{
		if(pEvent->button() == Qt::MiddleButton || pEvent->button() == Qt::RightButton)
		{
			m_Drag = DragPan;
			m_PanStart = pEvent->pos();
			m_PanOrigin = clampPan(m_Pan);
			setCursor(Qt::ClosedHandCursor);
			return;
		}

		if(pEvent->button() != Qt::LeftButton || m_pRectangles == nullptr)
		{
			QWidget::mousePressEvent(pEvent);
			return;
		}

		const QPoint Position = pEvent->pos();

		int iEdges = 0;
		const int iIndex = hitTest(Position, iEdges);

		m_DragOrigin = toImage(Position);

		if(iIndex < 0)
		{
			if(!target().contains(Position))
			{
				return;
			}

			// create new one
			m_Drag = DragCreate;
			m_iDragEdges = 0;
			m_DragRectangle = clamp(QRect(m_DragOrigin, m_DragOrigin));
			update();
			return;
		}

		if(iIndex != m_iSelected)
		{
			emit selectionRequested(iIndex);
		}

		m_DragStart = m_pRectangles->at(iIndex).Rectangle.normalized();
		m_DragRectangle = m_DragStart;
		m_iDragEdges = iEdges;
		m_Drag = iEdges != 0 ? DragResize : DragMove;
	}

	void HotspotPreview::mouseMoveEvent(QMouseEvent *pEvent)
	{
		if(m_Drag == DragPan)
		{
			m_Pan = m_PanOrigin + QPointF(pEvent->pos() - m_PanStart);
			update();
			return;
		}

		if(m_Drag == DragNone)
		{
			updateCursor(pEvent->pos());
			QWidget::mouseMoveEvent(pEvent);
			return;
		}

		const bool bSnapping = !(pEvent->modifiers() & Qt::AltModifier);

		const QPoint Position = toImage(pEvent->pos());

		if(m_Drag == DragCreate)
		{
			QRect Rectangle = QRect(m_DragOrigin, Position).normalized();

			Rectangle.setLeft(snap(Rectangle.left(), bSnapping));
			Rectangle.setTop(snap(Rectangle.top(), bSnapping));
			Rectangle.setRight(snap(Rectangle.right() + 1, bSnapping) - 1);
			Rectangle.setBottom(snap(Rectangle.bottom() + 1, bSnapping) - 1);

			m_DragRectangle = clamp(Rectangle);
			update();
			return;
		}

		QRect Rectangle = m_DragStart;

		if(m_Drag == DragMove)
		{
			Rectangle.translate(Position - m_DragOrigin);
			Rectangle.moveTo(snap(Rectangle.left(), bSnapping), snap(Rectangle.top(), bSnapping));
			Rectangle.moveLeft(qBound(0, Rectangle.left(), m_iImageWidth - Rectangle.width()));
			Rectangle.moveTop(qBound(0, Rectangle.top(), m_iImageHeight - Rectangle.height()));
		}
		else
		{
			if(m_iDragEdges & EdgeLeft)
			{
				Rectangle.setLeft(qMin(snap(Position.x(), bSnapping), m_DragStart.right()));
			}
			if(m_iDragEdges & EdgeRight)
			{
				Rectangle.setRight(qMax(snap(Position.x() + 1, bSnapping) - 1, m_DragStart.left()));
			}
			if(m_iDragEdges & EdgeTop)
			{
				Rectangle.setTop(qMin(snap(Position.y(), bSnapping), m_DragStart.bottom()));
			}
			if(m_iDragEdges & EdgeBottom)
			{
				Rectangle.setBottom(qMax(snap(Position.y() + 1, bSnapping) - 1, m_DragStart.top()));
			}

			Rectangle = clamp(Rectangle);
		}

		if(Rectangle != m_DragRectangle)
		{
			m_DragRectangle = Rectangle;
			emit rectangleEdited(m_iSelected, Rectangle);
		}
	}

	void HotspotPreview::mouseReleaseEvent(QMouseEvent *pEvent)
	{
		if(m_Drag == DragPan)
		{
			if(pEvent->button() == Qt::MiddleButton || pEvent->button() == Qt::RightButton)
			{
				m_Pan = clampPan(m_Pan);
				m_Drag = DragNone;
				updateCursor(pEvent->pos());
			}
			return;
		}

		if(pEvent->button() != Qt::LeftButton)
		{
			QWidget::mouseReleaseEvent(pEvent);
			return;
		}

		const Drag Mode = m_Drag;
		m_Drag = DragNone;
		m_iDragEdges = 0;

		if(Mode == DragCreate)
		{
			const QRect Rectangle = m_DragRectangle;
			m_DragRectangle = QRect();
			update();

			// ignore accidental click
			if(Rectangle.width() > 1 && Rectangle.height() > 1)
			{
				emit rectangleCreated(Rectangle);
			}
		}

		updateCursor(pEvent->pos());
	}

	void HotspotPreview::wheelEvent(QWheelEvent *pEvent)
	{
		const qreal fSteps = pEvent->angleDelta().y() / 120.0;
		if(fSteps == 0.0 || m_Drag != DragNone)
		{
			QWidget::wheelEvent(pEvent);
			return;
		}

		const QPointF Position = pEvent->position();
		const QPointF Image = toImageF(Position);

		const qreal fZoom = qBound(qreal(1.0), m_fZoom * qPow(1.25, fSteps), qreal(64.0));
		if(fZoom == m_fZoom)
		{
			pEvent->accept();
			return;
		}

		m_fZoom = fZoom;

		const qreal fScale = scale();
		m_Pan = QPointF(
			Position.x() - Image.x() * fScale - (width() - m_iImageWidth * fScale) * 0.5,
			Position.y() - Image.y() * fScale - (height() - m_iImageHeight * fScale) * 0.5);
		m_Pan = clampPan(m_Pan);

		update();
		updateCursor(Position.toPoint());

		pEvent->accept();
	}

	void HotspotPreview::leaveEvent(QEvent *pEvent)
	{
		unsetCursor();
		QWidget::leaveEvent(pEvent);
	}

	void HotspotPreview::paintEvent(QPaintEvent *)
	{
		QPainter Painter(this);
		Painter.fillRect(rect(), QColor(60, 60, 60));

		const QRectF Target = target();

		if(!m_Image.isNull())
		{
			Painter.drawImage(Target, m_Image);
		}

		Painter.setPen(QPen(QColor(96, 96, 96), 1.0));
		Painter.drawRect(Target.adjusted(-0.5, -0.5, 0.5, 0.5));

		if(m_pRectangles != nullptr)
		{
			for(int i = 0; i < m_pRectangles->count(); i++)
			{
				if(i == m_iSelected)
				{
					continue;
				}

				const RectEntry &Entry = m_pRectangles->at(i);

				const bool bValid = isTileValid(Entry, m_iImageWidth, m_iImageHeight);

				Painter.setPen(QPen(bValid ? QColor(64, 160, 255) : WarningColor,
					1.0, bValid ? Qt::SolidLine : Qt::DashLine));
				Painter.drawRect(toWidget(Entry.Rectangle));
			}

			if(m_iSelected >= 0 && m_iSelected < m_pRectangles->count())
			{
				const RectEntry &Entry = m_pRectangles->at(m_iSelected);
				const QRectF Rectangle = toWidget(Entry.Rectangle);

				const bool bValid = isTileValid(Entry, m_iImageWidth, m_iImageHeight);

				Painter.fillRect(Rectangle, QColor(255, 64, 64, 48));
				Painter.setPen(QPen(bValid ? QColor(255, 64, 64) : WarningColor,
					1.0, bValid ? Qt::SolidLine : Qt::DashLine));
				Painter.drawRect(Rectangle);

				// resize handles on the corners and edge midpoints
				const qreal fMiddleX = Rectangle.center().x();
				const qreal fMiddleY = Rectangle.center().y();

				const QPointF Handles[8] =
				{
					QPointF(Rectangle.left(), Rectangle.top()),
					QPointF(fMiddleX, Rectangle.top()),
					QPointF(Rectangle.right(), Rectangle.top()),
					QPointF(Rectangle.right(), fMiddleY),
					QPointF(Rectangle.right(), Rectangle.bottom()),
					QPointF(fMiddleX, Rectangle.bottom()),
					QPointF(Rectangle.left(), Rectangle.bottom()),
					QPointF(Rectangle.left(), fMiddleY),
				};

				Painter.setBrush(QColor(255, 64, 64));
				for(const QPointF &Handle : Handles)
				{
					Painter.drawRect(QRectF(Handle.x() - 2.0, Handle.y() - 2.0, 4.0, 4.0));
				}
				Painter.setBrush(Qt::NoBrush);
			}
		}

		if(m_Drag == DragCreate && !m_DragRectangle.isEmpty())
		{
			Painter.setPen(QPen(QColor(255, 255, 255), 1.0, Qt::DashLine));
			Painter.drawRect(toWidget(m_DragRectangle));
		}
	}

	//
	// HotspotDialog.
	//

	HotspotDialog::HotspotDialog(const RectFile &Rectangles, const QImage &Image,
		int iImageWidth, int iImageHeight, QWidget *pParent)
		: QDialog(pParent)
		, m_Rectangles(Rectangles)
		, m_Image(Image)
		, m_iImageWidth(iImageWidth > 0 ? iImageWidth : 1)
		, m_iImageHeight(iImageHeight > 0 ? iImageHeight : 1)
		, m_bUpdating(false)
	{
		setWindowTitle(tr("Hotspot Rectangles"));

		m_pList = new QTreeWidget(this);
		m_pList->setMinimumWidth(280);
		m_pList->setColumnCount(ColumnCount);
		m_pList->setRootIsDecorated(false);
		m_pList->setUniformRowHeights(true);
		m_pList->setAllColumnsShowFocus(true);
		m_pList->setHeaderLabels(QStringList()
			<< tr("#") << tr("X") << tr("Y") << tr("Width") << tr("Height") << tr("Flags"));

		QHeaderView *pHeader = m_pList->header();
		pHeader->setStretchLastSection(true);
		for(int i = ColumnIndex; i < ColumnFlags; i++)
		{
			pHeader->setSectionResizeMode(i, QHeaderView::ResizeToContents);
			m_pList->headerItem()->setTextAlignment(i, Qt::AlignRight | Qt::AlignVCenter);
		}

		connect(m_pList, &QTreeWidget::itemSelectionChanged, this, &HotspotDialog::onSelectionChanged);

		QPushButton *pAddButton = new QPushButton(tr("&Add"), this);
		connect(pAddButton, &QPushButton::clicked, this, &HotspotDialog::onAdd);

		m_pDuplicateButton = new QPushButton(tr("D&uplicate"), this);
		connect(m_pDuplicateButton, &QPushButton::clicked, this, &HotspotDialog::onDuplicate);

		m_pRemoveButton = new QPushButton(tr("&Remove"), this);
		connect(m_pRemoveButton, &QPushButton::clicked, this, &HotspotDialog::onRemove);

		m_pMoveUpButton = new QPushButton(tr("Move U&p"), this);
		connect(m_pMoveUpButton, &QPushButton::clicked, this, &HotspotDialog::onMoveUp);

		m_pMoveDownButton = new QPushButton(tr("Move &Down"), this);
		connect(m_pMoveDownButton, &QPushButton::clicked, this, &HotspotDialog::onMoveDown);

		QPushButton *pGenerateButton = new QPushButton(tr("Generate &Grid..."), this);
		pGenerateButton->setToolTip(tr("Replace the rectangles with an evenly divided grid."));
		connect(pGenerateButton, &QPushButton::clicked, this, &HotspotDialog::onGenerateGrid);

		QGridLayout *pButtonLayout = new QGridLayout();
		pButtonLayout->addWidget(pAddButton, 0, 0);
		pButtonLayout->addWidget(m_pDuplicateButton, 0, 1);
		pButtonLayout->addWidget(m_pRemoveButton, 1, 0);
		pButtonLayout->addWidget(pGenerateButton, 1, 1);
		pButtonLayout->addWidget(m_pMoveUpButton, 2, 0);
		pButtonLayout->addWidget(m_pMoveDownButton, 2, 1);

		QGroupBox *pList = new QGroupBox(tr("Rectangles:"), this);
		QVBoxLayout *pListLayout = new QVBoxLayout(pList);
		pListLayout->addWidget(m_pList, 1);
		pListLayout->addLayout(pButtonLayout);

		m_pEditor = createEditor();

		m_pPreview = new HotspotPreview(this);
		m_pPreview->setImage(m_Image, m_iImageWidth, m_iImageHeight);
		m_pPreview->setRectangles(&m_Rectangles.rectangles());
		connect(m_pPreview, &HotspotPreview::selectionRequested, this, &HotspotDialog::onPreviewSelection);
		connect(m_pPreview, &HotspotPreview::rectangleEdited, this, &HotspotDialog::onPreviewEdited);
		connect(m_pPreview, &HotspotPreview::rectangleCreated, this, &HotspotDialog::onPreviewCreated);

		m_pSnap = new QSpinBox(this);
		m_pSnap->setRange(0, qMin(m_iImageWidth, m_iImageHeight));
		m_pSnap->setSuffix(tr(" px"));
		m_pSnap->setToolTip(tr("Grid the mouse snaps to, or zero for none. "
			"Hold Alt while dragging to ignore it."));
		m_pSnap->setValue(qMin(32, m_pSnap->maximum()));
		connect(m_pSnap, &QSpinBox::valueChanged, this, &HotspotDialog::onSnapChanged);

		m_pPreview->setSnap(m_pSnap->value());

		QHBoxLayout *pViewLayout = new QHBoxLayout();
		pViewLayout->addWidget(new QLabel(tr("Snap:"), this));
		pViewLayout->addWidget(m_pSnap);
		pViewLayout->addStretch(1);

		m_pHint = new QLabel(tr("Drag on the texture to draw a rectangle, or drag a rectangle and its handles to adjust it.\n"
			"Use mouse wheel to zoom and middle mouse button to pan."), this);
		m_pHint->setWordWrap(true);

		QGroupBox *pPreview = new QGroupBox(tr("Preview:"), this);
		QVBoxLayout *pPreviewLayout = new QVBoxLayout(pPreview);
		pPreviewLayout->addWidget(m_pPreview, 1);
		pPreviewLayout->addLayout(pViewLayout);
		pPreviewLayout->addWidget(m_pHint);

		QVBoxLayout *pLeftLayout = new QVBoxLayout();
		pLeftLayout->addWidget(pList, 1);
		pLeftLayout->addWidget(m_pEditor);

		QDialogButtonBox *pButtons = new QDialogButtonBox(
			QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
		connect(pButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
		connect(pButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);

		QHBoxLayout *pContentLayout = new QHBoxLayout();
		pContentLayout->addLayout(pLeftLayout, 1);
		pContentLayout->addWidget(pPreview, 2);

		QVBoxLayout *pLayout = new QVBoxLayout(this);
		pLayout->addLayout(pContentLayout, 1);
		pLayout->addWidget(pButtons);

		resize(880, 600);

		rebuildList(m_Rectangles.isEmpty() ? -1 : 0);
	}

	QWidget *HotspotDialog::createEditor()
	{
		QGroupBox *pEditor = new QGroupBox(tr("Rectangle:"), this);
		QFormLayout *pForm = new QFormLayout(pEditor);

		m_pX = new QSpinBox(pEditor);
		m_pX->setRange(0, m_iImageWidth - 1);
		connect(m_pX, &QSpinBox::valueChanged, this, &HotspotDialog::onRectangleEdited);
		pForm->addRow(tr("X:"), m_pX);

		m_pY = new QSpinBox(pEditor);
		m_pY->setRange(0, m_iImageHeight - 1);
		connect(m_pY, &QSpinBox::valueChanged, this, &HotspotDialog::onRectangleEdited);
		pForm->addRow(tr("Y:"), m_pY);

		m_pWidth = new QSpinBox(pEditor);
		m_pWidth->setRange(1, m_iImageWidth);
		connect(m_pWidth, &QSpinBox::valueChanged, this, &HotspotDialog::onRectangleEdited);
		pForm->addRow(tr("Width:"), m_pWidth);

		m_pHeight = new QSpinBox(pEditor);
		m_pHeight->setRange(1, m_iImageHeight);
		connect(m_pHeight, &QSpinBox::valueChanged, this, &HotspotDialog::onRectangleEdited);
		pForm->addRow(tr("Height:"), m_pHeight);

		m_pRotate = new QCheckBox(tr("Rotate"), pEditor);
		m_pRotate->setToolTip(tr("Allow the region to be rotated to better match the target surface."));
		connect(m_pRotate, &QCheckBox::toggled, this, &HotspotDialog::onRectangleEdited);
		pForm->addRow(tr("Flags:"), m_pRotate);

		m_pReflect = new QCheckBox(tr("Reflect"), pEditor);
		m_pReflect->setToolTip(tr("Allow the region to be randomly flipped horizontally."));
		connect(m_pReflect, &QCheckBox::toggled, this, &HotspotDialog::onRectangleEdited);
		pForm->addRow(QString(), m_pReflect);

		m_pAlt = new QCheckBox(tr("Alternate"), pEditor);
		m_pAlt->setToolTip(tr("Only pick this region when the Alt key is held in the editor."));
		connect(m_pAlt, &QCheckBox::toggled, this, &HotspotDialog::onRectangleEdited);
		pForm->addRow(QString(), m_pAlt);

		m_pTile = new QCheckBox(tr("Tile"), pEditor);
		m_pTile->setToolTip(tr("Allow the region to repeat along whichever axis it spans the whole texture on."));
		connect(m_pTile, &QCheckBox::toggled, this, &HotspotDialog::onRectangleEdited);
		pForm->addRow(QString(), m_pTile);

		return pEditor;
	}

	//
	// List and editor.
	//

	void HotspotDialog::rebuildList(int iSelect)
	{
		const bool bUpdating = m_bUpdating;
		m_bUpdating = true;

		m_pList->clear();

		for(int i = 0; i < m_Rectangles.rectangles().count(); i++)
		{
			m_pList->addTopLevelItem(createRectangleItem(i, m_Rectangles.rectangles().at(i),
				m_iImageWidth, m_iImageHeight));
		}

		m_bUpdating = bUpdating;

		if(iSelect >= 0 && iSelect < m_pList->topLevelItemCount())
		{
			m_pList->setCurrentItem(m_pList->topLevelItem(iSelect));
		}
		else
		{
			m_pList->setCurrentItem(nullptr);
			onSelectionChanged();
		}
	}

	void HotspotDialog::updateItem(int iIndex)
	{
		QTreeWidgetItem *pItem = m_pList->topLevelItem(iIndex);
		if(pItem == nullptr)
		{
			return;
		}

		const RectEntry &Entry = m_Rectangles.rectangles().at(iIndex);

		const QStringList Columns = rectangleColumns(iIndex, Entry, m_iImageWidth, m_iImageHeight);
		for(int i = 0; i < Columns.count(); i++)
		{
			pItem->setText(i, Columns.at(i));
		}

		applyTileWarning(pItem, Entry, m_iImageWidth, m_iImageHeight);
	}

	int HotspotDialog::selectedRectangle() const
	{
		const int iIndex = m_pList->indexOfTopLevelItem(m_pList->currentItem());
		return iIndex >= 0 && iIndex < m_Rectangles.rectangles().count() ? iIndex : -1;
	}

	void HotspotDialog::onSelectionChanged()
	{
		updateEditor();
		updateButtons();

		m_pPreview->setSelected(selectedRectangle());
	}

	void HotspotDialog::updateEditor()
	{
		const int iIndex = selectedRectangle();

		m_pEditor->setEnabled(iIndex >= 0);

		if(iIndex < 0)
		{
			return;
		}

		const RectEntry &Entry = m_Rectangles.rectangles().at(iIndex);
		const QRect Rectangle = Entry.Rectangle.normalized();

		m_bUpdating = true;

		m_pX->setValue(Rectangle.left());
		m_pY->setValue(Rectangle.top());
		m_pWidth->setValue(Rectangle.width());
		m_pHeight->setValue(Rectangle.height());

		m_pRotate->setChecked(Entry.bRotate);
		m_pReflect->setChecked(Entry.bReflect);
		m_pAlt->setChecked(Entry.bAlt);
		m_pTile->setChecked(Entry.bTile);

		m_bUpdating = false;
	}

	void HotspotDialog::updateButtons()
	{
		const int iIndex = selectedRectangle();

		m_pDuplicateButton->setEnabled(iIndex >= 0);
		m_pRemoveButton->setEnabled(iIndex >= 0);
		m_pMoveUpButton->setEnabled(iIndex > 0);
		m_pMoveDownButton->setEnabled(iIndex >= 0
			&& iIndex + 1 < m_Rectangles.rectangles().count());
	}

	//
	// Editing.
	//

	void HotspotDialog::onAdd()
	{
		RectEntry Entry;

		// crappy but convenient heuristic
		Entry.Rectangle = QRect(0, 0, qMax(1, m_iImageWidth / 4), qMax(1, m_iImageHeight / 4));

		m_Rectangles.rectangles().append(Entry);

		rebuildList(m_Rectangles.rectangles().count() - 1);
	}

	void HotspotDialog::onDuplicate()
	{
		const int iIndex = selectedRectangle();
		if(iIndex < 0)
		{
			return;
		}

		m_Rectangles.rectangles().insert(iIndex + 1, m_Rectangles.rectangles().at(iIndex));

		rebuildList(iIndex + 1);
	}

	void HotspotDialog::onRemove()
	{
		const int iIndex = selectedRectangle();
		if(iIndex < 0)
		{
			return;
		}

		m_Rectangles.rectangles().removeAt(iIndex);

		rebuildList(qMin(iIndex, m_Rectangles.rectangles().count() - 1));
	}

	void HotspotDialog::onMoveUp()
	{
		move(-1);
	}

	void HotspotDialog::onMoveDown()
	{
		move(1);
	}

	void HotspotDialog::move(int iDirection)
	{
		const int iIndex = selectedRectangle();
		if(iIndex < 0)
		{
			return;
		}

		const int iTarget = iIndex + iDirection;
		if(iTarget < 0 || iTarget >= m_Rectangles.rectangles().count())
		{
			return;
		}

		m_Rectangles.rectangles().swapItemsAt(iIndex, iTarget);

		rebuildList(iTarget);
	}

	void HotspotDialog::onGenerateGrid()
	{
		QDialog Dialog(this);
		Dialog.setWindowTitle(tr("Generate Grid"));

		QSpinBox *pColumns = new QSpinBox(&Dialog);
		pColumns->setRange(1, m_iImageWidth);
		pColumns->setValue(qMin(4, m_iImageWidth));

		QSpinBox *pRows = new QSpinBox(&Dialog);
		pRows->setRange(1, m_iImageHeight);
		pRows->setValue(qMin(4, m_iImageHeight));

		QCheckBox *pReplace = new QCheckBox(tr("Replace the existing rectangles"), &Dialog);
		pReplace->setChecked(true);

		QLabel *pCellSize = new QLabel(&Dialog);

		auto UpdateGrid = [&]()
		{
			pCellSize->setText(tr("%1 x %2 pixels")
				.arg(m_iImageWidth / pColumns->value())
				.arg(m_iImageHeight / pRows->value()));
		};

		connect(pColumns, &QSpinBox::valueChanged, &Dialog, UpdateGrid);
		connect(pRows, &QSpinBox::valueChanged, &Dialog, UpdateGrid);
		UpdateGrid();

		QDialogButtonBox *pButtons = new QDialogButtonBox(
			QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &Dialog);
		connect(pButtons, &QDialogButtonBox::accepted, &Dialog, &QDialog::accept);
		connect(pButtons, &QDialogButtonBox::rejected, &Dialog, &QDialog::reject);

		QFormLayout *pForm = new QFormLayout();
		pForm->addRow(tr("Columns:"), pColumns);
		pForm->addRow(tr("Rows:"), pRows);
		pForm->addRow(tr("Cell size:"), pCellSize);
		pForm->addRow(QString(), pReplace);

		QVBoxLayout *pLayout = new QVBoxLayout(&Dialog);
		pLayout->addLayout(pForm);
		pLayout->addWidget(pButtons);

		if(Dialog.exec() != QDialog::Accepted)
		{
			return;
		}

		const int iColumns = pColumns->value();
		const int iRows = pRows->value();

		if(pReplace->isChecked())
		{
			m_Rectangles.rectangles().clear();
		}

		// not mulyipling here to handle textures that divide unevenly
		for(int iRow = 0; iRow < iRows; iRow++)
		{
			const int iTop = iRow * m_iImageHeight / iRows;
			const int iBottom = (iRow + 1) * m_iImageHeight / iRows;

			for(int iColumn = 0; iColumn < iColumns; iColumn++)
			{
				const int iLeft = iColumn * m_iImageWidth / iColumns;
				const int iRight = (iColumn + 1) * m_iImageWidth / iColumns;

				RectEntry Entry;
				Entry.Rectangle = QRect(QPoint(iLeft, iTop), QPoint(iRight - 1, iBottom - 1));

				m_Rectangles.rectangles().append(Entry);
			}
		}

		rebuildList(m_Rectangles.isEmpty() ? -1 : 0);
	}

	void HotspotDialog::onRectangleEdited()
	{
		if(m_bUpdating)
		{
			return;
		}

		const int iIndex = selectedRectangle();
		if(iIndex < 0)
		{
			return;
		}

		m_bUpdating = true;

		// keep it in bounds
		const int iX = qBound(0, m_pX->value(), m_iImageWidth - 1);
		const int iY = qBound(0, m_pY->value(), m_iImageHeight - 1);
		const int iWidth = qBound(1, m_pWidth->value(), m_iImageWidth - iX);
		const int iHeight = qBound(1, m_pHeight->value(), m_iImageHeight - iY);

		m_pX->setValue(iX);
		m_pY->setValue(iY);
		m_pWidth->setValue(iWidth);
		m_pHeight->setValue(iHeight);

		m_bUpdating = false;

		RectEntry &Entry = m_Rectangles.rectangles()[iIndex];
		Entry.Rectangle = QRect(iX, iY, iWidth, iHeight);
		Entry.bRotate = m_pRotate->isChecked();
		Entry.bReflect = m_pReflect->isChecked();
		Entry.bAlt = m_pAlt->isChecked();
		Entry.bTile = m_pTile->isChecked();

		updateItem(iIndex);
		m_pPreview->update();
	}

	void HotspotDialog::onPreviewSelection(int iIndex)
	{
		if(iIndex >= 0 && iIndex < m_pList->topLevelItemCount())
		{
			m_pList->setCurrentItem(m_pList->topLevelItem(iIndex));
		}
	}

	void HotspotDialog::onPreviewEdited(int iIndex, const QRect &Rectangle)
	{
		if(iIndex < 0 || iIndex >= m_Rectangles.rectangles().count())
		{
			return;
		}

		m_Rectangles.rectangles()[iIndex].Rectangle = Rectangle;

		updateItem(iIndex);
		updateEditor();
		m_pPreview->update();
	}

	void HotspotDialog::onPreviewCreated(const QRect &Rectangle)
	{
		RectEntry Entry;
		Entry.Rectangle = Rectangle;

		m_Rectangles.rectangles().append(Entry);

		rebuildList(m_Rectangles.rectangles().count() - 1);
	}

	void HotspotDialog::onSnapChanged(int iSnap)
	{
		m_pPreview->setSnap(iSnap);
	}
}
