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

#include "SheetDialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

namespace VTFEdit
{
	namespace
	{
		enum
		{
			SequenceRole = Qt::UserRole,
			FrameRole = Qt::UserRole + 1,
		};

		QString sequenceLabel(const SheetSequence &Sequence)
		{
			return SheetDialog::tr("Sequence %1 (%2 frames, %3)")
				.arg(Sequence.iNumber)
				.arg(Sequence.Frames.count())
				.arg(Sequence.bClamp ? SheetDialog::tr("clamp") : SheetDialog::tr("loop"));
		}

		QString frameLabel(int iIndex, const SheetFrame &Frame)
		{
			return SheetDialog::tr("Frame %1 (%2 s)")
				.arg(iIndex)
				.arg(QString::number(Frame.fDuration, 'g', 4));
		}
	}

	//
	// SheetPreview.
	//

	SheetPreview::SheetPreview(QWidget *pParent)
		: QWidget(pParent)
		, m_bHasFrame(false)
		, m_bAnimating(false)
	{
		setMinimumSize(160, 160);
	}

	void SheetPreview::setAnimating(bool bAnimating)
	{
		if(m_bAnimating == bAnimating)
		{
			return;
		}

		m_bAnimating = bAnimating;
		update();
	}

	void SheetPreview::setImage(const QImage &Image)
	{
		m_Image = Image;
		update();
	}

	void SheetPreview::setFrame(const SheetFrame &Frame, bool bValid)
	{
		m_Frame = Frame;
		m_bHasFrame = bValid;
		update();
	}

	void SheetPreview::clearFrame()
	{
		m_bHasFrame = false;
		update();
	}

	void SheetPreview::paintEvent(QPaintEvent *)
	{
		QPainter Painter(this);
		Painter.fillRect(rect(), QColor(60, 60, 60));

		if(m_Image.isNull())
		{
			return;
		}

		if(m_bAnimating && m_bHasFrame)
		{
			// blow the frame itself up to fill the widget
			const SheetCoords &Coords = m_Frame.Images[0];

			QRect Source(
				qRound(Coords.fLeft * m_Image.width()),
				qRound(Coords.fTop * m_Image.height()),
				qMax(1, qRound((Coords.fRight - Coords.fLeft) * m_Image.width())),
				qMax(1, qRound((Coords.fBottom - Coords.fTop) * m_Image.height())));
			Source = Source.intersected(m_Image.rect());

			if(Source.isEmpty())
			{
				return;
			}

			QSize Frame = Source.size().scaled(size(), Qt::KeepAspectRatio);
			QRect Target(QPoint((width() - Frame.width()) / 2, (height() - Frame.height()) / 2), Frame);

			Painter.drawImage(Target, m_Image, Source);
			return;
		}

		// fit the sheet into the widget
		QSize Size = m_Image.size().scaled(size(), Qt::KeepAspectRatio);
		QRect Target(QPoint((width() - Size.width()) / 2, (height() - Size.height()) / 2), Size);

		Painter.drawImage(Target, m_Image);

		if(!m_bHasFrame)
		{
			return;
		}

		for(int i = SheetImagesPerFrame - 1; i >= 0; i--)
		{
			const SheetCoords &Coords = m_Frame.Images[i];

			QRectF Rectangle(
				Target.left() + Coords.fLeft * Target.width(),
				Target.top() + Coords.fTop * Target.height(),
				(Coords.fRight - Coords.fLeft) * Target.width(),
				(Coords.fBottom - Coords.fTop) * Target.height());

			Painter.setPen(QPen(i == 0 ? QColor(255, 64, 64) : QColor(64, 160, 255), 1.0));
			Painter.drawRect(Rectangle);
		}
	}

	//
	// SheetDialog.
	//

	SheetDialog::SheetDialog(const SheetFile &Sheet, const QImage &Image,
		int iImageWidth, int iImageHeight, QWidget *pParent)
		: QDialog(pParent)
		, m_Sheet(Sheet)
		, m_Image(Image)
		, m_iImageWidth(iImageWidth > 0 ? iImageWidth : 1)
		, m_iImageHeight(iImageHeight > 0 ? iImageHeight : 1)
		, m_bUpdating(false)
		, m_iPlaySequence(-1)
		, m_iPlayFrame(0)
	{
		setWindowTitle(tr("Sprite Sheet"));

		m_pTree = new QTreeWidget(this);
		m_pTree->setHeaderHidden(true);
		m_pTree->setColumnCount(1);
		m_pTree->setMinimumWidth(240);
		connect(m_pTree, &QTreeWidget::itemSelectionChanged, this, &SheetDialog::onSelectionChanged);

		QPushButton *pAddSequenceButton = new QPushButton(tr("Add &Sequence"), this);
		connect(pAddSequenceButton, &QPushButton::clicked, this, &SheetDialog::onAddSequence);

		m_pAddFrameButton = new QPushButton(tr("Add &Frame"), this);
		connect(m_pAddFrameButton, &QPushButton::clicked, this, &SheetDialog::onAddFrame);

		m_pDuplicateButton = new QPushButton(tr("D&uplicate"), this);
		connect(m_pDuplicateButton, &QPushButton::clicked, this, &SheetDialog::onDuplicate);

		m_pRemoveButton = new QPushButton(tr("&Remove"), this);
		connect(m_pRemoveButton, &QPushButton::clicked, this, &SheetDialog::onRemove);

		m_pMoveUpButton = new QPushButton(tr("Move U&p"), this);
		connect(m_pMoveUpButton, &QPushButton::clicked, this, &SheetDialog::onMoveUp);

		m_pMoveDownButton = new QPushButton(tr("Move &Down"), this);
		connect(m_pMoveDownButton, &QPushButton::clicked, this, &SheetDialog::onMoveDown);

		QPushButton *pGenerateButton = new QPushButton(tr("Generate &Grid..."), this);
		pGenerateButton->setToolTip(tr("Create a sequence from an evenly divided grid of frames."));
		connect(pGenerateButton, &QPushButton::clicked, this, &SheetDialog::onGenerateGrid);

		QGridLayout *pButtonLayout = new QGridLayout();
		pButtonLayout->addWidget(pAddSequenceButton, 0, 0);
		pButtonLayout->addWidget(m_pAddFrameButton, 0, 1);
		pButtonLayout->addWidget(m_pDuplicateButton, 1, 0);
		pButtonLayout->addWidget(m_pRemoveButton, 1, 1);
		pButtonLayout->addWidget(m_pMoveUpButton, 2, 0);
		pButtonLayout->addWidget(m_pMoveDownButton, 2, 1);
		pButtonLayout->addWidget(pGenerateButton, 3, 0, 1, 2);

		QGroupBox *pSequences = new QGroupBox(tr("Sequences:"), this);
		QVBoxLayout *pSequencesLayout = new QVBoxLayout(pSequences);
		pSequencesLayout->addWidget(m_pTree, 1);
		pSequencesLayout->addLayout(pButtonLayout);

		m_pEmptyEditor = new QWidget(this);
		QVBoxLayout *pEmptyLayout = new QVBoxLayout(m_pEmptyEditor);
		QLabel *pEmptyLabel = new QLabel(tr("Select a sequence or frame to edit it."), m_pEmptyEditor);
		pEmptyLabel->setAlignment(Qt::AlignCenter);
		pEmptyLabel->setWordWrap(true);
		pEmptyLayout->addWidget(pEmptyLabel);

		m_pSequenceEditor = createSequenceEditor();
		m_pFrameEditor = createFrameEditor();

		m_pEditorStack = new QStackedWidget(this);
		m_pEditorStack->addWidget(m_pEmptyEditor);
		m_pEditorStack->addWidget(m_pSequenceEditor);
		m_pEditorStack->addWidget(m_pFrameEditor);

		m_pPreview = new SheetPreview(this);
		m_pPreview->setImage(m_Image);

		m_pPlayTimer = new QTimer(this);
		m_pPlayTimer->setSingleShot(true);
		connect(m_pPlayTimer, &QTimer::timeout, this, &SheetDialog::onPlaybackTimeout);

		m_pPlayButton = new QPushButton(tr("&Play"), this);
		m_pPlayButton->setToolTip(tr("Play the selected sequence back at its frame durations."));
		connect(m_pPlayButton, &QPushButton::clicked, this, &SheetDialog::onPlayClicked);

		m_pPlaySpeed = new QDoubleSpinBox(this);
		m_pPlaySpeed->setDecimals(2);
		m_pPlaySpeed->setSingleStep(0.25);
		m_pPlaySpeed->setRange(0.05, 20.0);
		m_pPlaySpeed->setValue(1.0);
		m_pPlaySpeed->setSuffix(tr("x"));
		m_pPlaySpeed->setToolTip(tr("Playback speed multiplier."));

		m_pPlayStatus = new QLabel(this);

		QHBoxLayout *pPlayLayout = new QHBoxLayout();
		pPlayLayout->addWidget(m_pPlayButton);
		pPlayLayout->addWidget(new QLabel(tr("Speed:"), this));
		pPlayLayout->addWidget(m_pPlaySpeed);
		pPlayLayout->addWidget(m_pPlayStatus, 1);

		QGroupBox *pPreview = new QGroupBox(tr("Preview:"), this);
		QVBoxLayout *pPreviewLayout = new QVBoxLayout(pPreview);
		pPreviewLayout->addWidget(m_pPreview, 1);
		pPreviewLayout->addLayout(pPlayLayout);

		QVBoxLayout *pRightLayout = new QVBoxLayout();
		pRightLayout->addWidget(m_pEditorStack);
		pRightLayout->addWidget(pPreview, 1);

		QDialogButtonBox *pButtons = new QDialogButtonBox(
			QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
		connect(pButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
		connect(pButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);

		QHBoxLayout *pContentLayout = new QHBoxLayout();
		pContentLayout->addWidget(pSequences, 1);
		pContentLayout->addLayout(pRightLayout, 2);

		QVBoxLayout *pLayout = new QVBoxLayout(this);
		pLayout->addLayout(pContentLayout, 1);
		pLayout->addWidget(pButtons);

		resize(820, 560);

		rebuildTree(m_Sheet.sequences().isEmpty() ? -1 : 0, -1);
	}

	QWidget *SheetDialog::createSequenceEditor()
	{
		QGroupBox *pEditor = new QGroupBox(tr("Sequence:"), this);
		QFormLayout *pForm = new QFormLayout(pEditor);

		m_pSequenceNumber = new QSpinBox(pEditor);
		m_pSequenceNumber->setRange(0, SheetMaxSequences - 1);
		m_pSequenceNumber->setToolTip(tr("The sequence number materials and particle systems refer to this animation by."));
		connect(m_pSequenceNumber, &QSpinBox::valueChanged, this, &SheetDialog::onSequenceEdited);
		pForm->addRow(tr("Number:"), m_pSequenceNumber);

		m_pSequenceLoop = new QCheckBox(tr("Loop"), pEditor);
		m_pSequenceLoop->setToolTip(tr("Wrap back to the first frame instead of holding the last one."));
		connect(m_pSequenceLoop, &QCheckBox::toggled, this, &SheetDialog::onSequenceEdited);
		pForm->addRow(QString(), m_pSequenceLoop);

		m_pSequenceSummary = new QLabel(pEditor);
		pForm->addRow(tr("Duration:"), m_pSequenceSummary);

		return pEditor;
	}

	QWidget *SheetDialog::createFrameEditor()
	{
		QGroupBox *pEditor = new QGroupBox(tr("Frame:"), this);
		QVBoxLayout *pLayout = new QVBoxLayout(pEditor);

		QFormLayout *pForm = new QFormLayout();

		m_pFrameDuration = new QDoubleSpinBox(pEditor);
		m_pFrameDuration->setDecimals(4);
		m_pFrameDuration->setSingleStep(0.1);
		m_pFrameDuration->setRange(0.0001, 10000.0);
		connect(m_pFrameDuration, &QDoubleSpinBox::valueChanged, this, &SheetDialog::onFrameEdited);
		pForm->addRow(tr("Duration:"), m_pFrameDuration);

		m_pLinkImages = new QCheckBox(tr("Use the first image for all four"), pEditor);
		m_pLinkImages->setToolTip(tr("A frame stores four images; most sheets repeat the first one, "
			"which is what mksheet does unless extra images are given."));
		connect(m_pLinkImages, &QCheckBox::toggled, this, &SheetDialog::onLinkImagesToggled);
		pForm->addRow(QString(), m_pLinkImages);

		pLayout->addLayout(pForm);

		QGridLayout *pGrid = new QGridLayout();
		pGrid->addWidget(new QLabel(tr("X"), pEditor), 0, 1);
		pGrid->addWidget(new QLabel(tr("Y"), pEditor), 0, 2);
		pGrid->addWidget(new QLabel(tr("Width"), pEditor), 0, 3);
		pGrid->addWidget(new QLabel(tr("Height"), pEditor), 0, 4);

		for(int i = 0; i < SheetImagesPerFrame; i++)
		{
			QSpinBox **ppBoxes[] = { &m_pImageX[i], &m_pImageY[i], &m_pImageWidth[i], &m_pImageHeight[i] };

			QLabel *pLabel = new QLabel(tr("Image %1:").arg(i), pEditor);
			pGrid->addWidget(pLabel, i + 1, 0);

			for(int j = 0; j < 4; j++)
			{
				// Columns are X, Y, width, height
				// odd ones run along the Y axis
				const bool bSize = j >= 2;
				const int iExtent = (j % 2) == 0 ? m_iImageWidth : m_iImageHeight;

				QSpinBox *pBox = new QSpinBox(pEditor);
				pBox->setRange(bSize ? 1 : 0, bSize ? iExtent : iExtent - 1);
				connect(pBox, &QSpinBox::valueChanged, this, &SheetDialog::onFrameEdited);
				pGrid->addWidget(pBox, i + 1, j + 1);
				*ppBoxes[j] = pBox;
			}

			m_pImageRows[i] = pLabel;
		}

		pLayout->addLayout(pGrid);
		pLayout->addWidget(new QLabel(tr("Coordinates are in pixels of the full size image (%1 x %2).")
			.arg(m_iImageWidth).arg(m_iImageHeight), pEditor));

		return pEditor;
	}

	//
	// Tree and editor state.
	//

	void SheetDialog::rebuildTree(int iSequence, int iFrame)
	{
		stopPlayback();

		const bool bUpdating = m_bUpdating;
		m_bUpdating = true;

		m_pTree->clear();

		QTreeWidgetItem *pSelect = nullptr;

		for(int i = 0; i < m_Sheet.sequences().count(); i++)
		{
			const SheetSequence &Sequence = m_Sheet.sequences().at(i);

			QTreeWidgetItem *pItem = new QTreeWidgetItem(m_pTree, QStringList(sequenceLabel(Sequence)));
			pItem->setData(0, SequenceRole, i);
			pItem->setData(0, FrameRole, -1);
			pItem->setExpanded(true);

			if(i == iSequence && iFrame < 0)
			{
				pSelect = pItem;
			}

			for(int j = 0; j < Sequence.Frames.count(); j++)
			{
				QTreeWidgetItem *pFrameItem = new QTreeWidgetItem(pItem,
					QStringList(frameLabel(j, Sequence.Frames.at(j))));
				pFrameItem->setData(0, SequenceRole, i);
				pFrameItem->setData(0, FrameRole, j);

				if(i == iSequence && j == iFrame)
				{
					pSelect = pFrameItem;
				}
			}
		}

		m_bUpdating = bUpdating;

		if(pSelect != nullptr)
		{
			m_pTree->setCurrentItem(pSelect);
		}
		else
		{
			onSelectionChanged();
		}
	}

	int SheetDialog::selectedSequence() const
	{
		const QTreeWidgetItem *pItem = m_pTree->currentItem();
		if(pItem == nullptr || !pItem->isSelected())
		{
			return -1;
		}

		const int iSequence = pItem->data(0, SequenceRole).toInt();
		return iSequence < static_cast<int>(m_Sheet.sequences().count()) ? iSequence : -1;
	}

	int SheetDialog::selectedFrame() const
	{
		const QTreeWidgetItem *pItem = m_pTree->currentItem();
		if(pItem == nullptr || selectedSequence() < 0)
		{
			return -1;
		}

		return pItem->data(0, FrameRole).toInt();
	}

	void SheetDialog::onSelectionChanged()
	{
		if(m_iPlaySequence >= 0 && selectedSequence() != m_iPlaySequence)
		{
			stopPlayback();
		}

		updateEditor();
		updateButtons();
		updatePlaybackControls();
	}

	void SheetDialog::updateEditor()
	{
		const int iSequence = selectedSequence();
		const int iFrame = selectedFrame();
		const bool bPlaying = m_iPlaySequence >= 0;

		if(iSequence < 0)
		{
			m_pEditorStack->setCurrentWidget(m_pEmptyEditor);
			if(!bPlaying)
			{
				m_pPreview->clearFrame();
			}
			return;
		}

		const SheetSequence &Sequence = m_Sheet.sequences().at(iSequence);

		m_bUpdating = true;

		if(iFrame < 0)
		{
			m_pSequenceNumber->setValue(Sequence.iNumber);
			m_pSequenceLoop->setChecked(!Sequence.bClamp);

			float fTotal = 0.0f;
			for(const SheetFrame &Frame : Sequence.Frames)
			{
				fTotal += Frame.fDuration;
			}

			m_pSequenceSummary->setText(tr("%1 s over %2 frames")
				.arg(QString::number(fTotal, 'g', 4)).arg(Sequence.Frames.count()));

			m_pEditorStack->setCurrentWidget(m_pSequenceEditor);
			if(!bPlaying)
			{
				m_pPreview->clearFrame();
			}
		}
		else
		{
			const SheetFrame &Frame = Sequence.Frames.at(iFrame);

			m_pFrameDuration->setValue(Frame.fDuration);

			bool bLinked = true;
			for(int i = 1; i < SheetImagesPerFrame; i++)
			{
				const SheetCoords &First = Frame.Images[0];
				const SheetCoords &Other = Frame.Images[i];

				if(First.fLeft != Other.fLeft 
					|| First.fTop != Other.fTop
					|| First.fRight != Other.fRight
					|| First.fBottom != Other.fBottom)
				{
					bLinked = false;
					break;
				}
			}

			m_pLinkImages->setChecked(bLinked);

			for(int i = 0; i < SheetImagesPerFrame; i++)
			{
				int iX = 0, iY = 0, iWidth = 0, iHeight = 0;
				SheetFile::coordsToRect(Frame.Images[i], m_iImageWidth, m_iImageHeight,
					iX, iY, iWidth, iHeight);

				m_pImageX[i]->setValue(iX);
				m_pImageY[i]->setValue(iY);
				m_pImageWidth[i]->setValue(iWidth);
				m_pImageHeight[i]->setValue(iHeight);

				const bool bEnabled = i == 0 || !bLinked;
				m_pImageRows[i]->setEnabled(bEnabled);
				m_pImageX[i]->setEnabled(bEnabled);
				m_pImageY[i]->setEnabled(bEnabled);
				m_pImageWidth[i]->setEnabled(bEnabled);
				m_pImageHeight[i]->setEnabled(bEnabled);
			}

			m_pEditorStack->setCurrentWidget(m_pFrameEditor);
			if(!bPlaying)
			{
				m_pPreview->setFrame(Frame, true);
			}
		}

		m_bUpdating = false;
	}

	void SheetDialog::updateButtons()
	{
		const int iSequence = selectedSequence();
		const int iFrame = selectedFrame();

		m_pAddFrameButton->setEnabled(iSequence >= 0);
		m_pDuplicateButton->setEnabled(iSequence >= 0);
		m_pRemoveButton->setEnabled(iSequence >= 0);

		bool bCanMoveUp = false;
		bool bCanMoveDown = false;

		if(iSequence >= 0)
		{
			if(iFrame < 0)
			{
				bCanMoveUp = iSequence > 0;
				bCanMoveDown = iSequence + 1 < m_Sheet.sequences().count();
			}
			else
			{
				bCanMoveUp = iFrame > 0;
				bCanMoveDown = iFrame + 1 < m_Sheet.sequences().at(iSequence).Frames.count();
			}
		}

		m_pMoveUpButton->setEnabled(bCanMoveUp);
		m_pMoveDownButton->setEnabled(bCanMoveDown);
	}

	//
	// Editing.
	//

	void SheetDialog::onAddSequence()
	{
		const int iNumber = m_Sheet.nextFreeSequenceNumber();
		if(iNumber < 0)
		{
			QMessageBox::warning(this, tr("Sprite Sheet"),
				tr("A sheet cannot hold more than %1 sequences.").arg(int(SheetMaxSequences)));
			return;
		}

		SheetSequence Sequence;
		Sequence.iNumber = iNumber;

		SheetFrame Frame;
		Frame.Images[0] = SheetFile::rectToCoords(0, 0, m_iImageWidth, m_iImageHeight,
			m_iImageWidth, m_iImageHeight);
		for(int i = 1; i < SheetImagesPerFrame; i++)
		{
			Frame.Images[i] = Frame.Images[0];
		}

		Sequence.Frames.append(Frame);
		m_Sheet.sequences().append(Sequence);

		rebuildTree(m_Sheet.sequences().count() - 1, -1);
	}

	void SheetDialog::onAddFrame()
	{
		const int iSequence = selectedSequence();
		if(iSequence < 0)
		{
			return;
		}

		SheetSequence &Sequence = m_Sheet.sequences()[iSequence];

		// new frame starts as a copy of the last one for qol
		SheetFrame Frame;
		if(!Sequence.Frames.isEmpty())
		{
			Frame = Sequence.Frames.last();
		}
		else
		{
			Frame.Images[0] = SheetFile::rectToCoords(0, 0, m_iImageWidth, m_iImageHeight,
				m_iImageWidth, m_iImageHeight);
			for(int i = 1; i < SheetImagesPerFrame; i++)
			{
				Frame.Images[i] = Frame.Images[0];
			}
		}

		Sequence.Frames.append(Frame);

		rebuildTree(iSequence, Sequence.Frames.count() - 1);
	}

	void SheetDialog::onDuplicate()
	{
		const int iSequence = selectedSequence();
		const int iFrame = selectedFrame();

		if(iSequence < 0)
		{
			return;
		}

		if(iFrame < 0)
		{
			const int iNumber = m_Sheet.nextFreeSequenceNumber();
			if(iNumber < 0)
			{
				QMessageBox::warning(this, tr("Sprite Sheet"),
					tr("A sheet cannot hold more than %1 sequences.").arg(int(SheetMaxSequences)));
				return;
			}

			SheetSequence Sequence = m_Sheet.sequences().at(iSequence);
			Sequence.iNumber = iNumber;
			m_Sheet.sequences().insert(iSequence + 1, Sequence);

			rebuildTree(iSequence + 1, -1);
		}
		else
		{
			SheetSequence &Sequence = m_Sheet.sequences()[iSequence];
			Sequence.Frames.insert(iFrame + 1, Sequence.Frames.at(iFrame));

			rebuildTree(iSequence, iFrame + 1);
		}
	}

	void SheetDialog::onRemove()
	{
		const int iSequence = selectedSequence();
		const int iFrame = selectedFrame();

		if(iSequence < 0)
		{
			return;
		}

		if(iFrame < 0)
		{
			m_Sheet.sequences().removeAt(iSequence);
			rebuildTree(qMin(iSequence, m_Sheet.sequences().count() - 1), -1);
		}
		else
		{
			SheetSequence &Sequence = m_Sheet.sequences()[iSequence];
			Sequence.Frames.removeAt(iFrame);

			if(Sequence.Frames.isEmpty())
			{
				rebuildTree(iSequence, -1);
			}
			else
			{
				rebuildTree(iSequence, qMin(iFrame, Sequence.Frames.count() - 1));
			}
		}
	}

	void SheetDialog::onMoveUp()
	{
		move(-1);
	}

	void SheetDialog::onMoveDown()
	{
		move(1);
	}

	void SheetDialog::move(int iDirection)
	{
		const int iSequence = selectedSequence();
		const int iFrame = selectedFrame();

		if(iSequence < 0)
		{
			return;
		}

		if(iFrame < 0)
		{
			const int iTarget = iSequence + iDirection;
			if(iTarget < 0 || iTarget >= m_Sheet.sequences().count())
			{
				return;
			}

			m_Sheet.sequences().swapItemsAt(iSequence, iTarget);
			rebuildTree(iTarget, -1);
		}
		else
		{
			SheetSequence &Sequence = m_Sheet.sequences()[iSequence];

			const int iTarget = iFrame + iDirection;
			if(iTarget < 0 || iTarget >= Sequence.Frames.count())
			{
				return;
			}

			Sequence.Frames.swapItemsAt(iFrame, iTarget);
			rebuildTree(iSequence, iTarget);
		}
	}

	void SheetDialog::onSequenceEdited()
	{
		if(m_bUpdating)
		{
			return;
		}

		const int iSequence = selectedSequence();
		if(iSequence < 0)
		{
			return;
		}

		const int iNumber = m_pSequenceNumber->value();

		for(int i = 0; i < m_Sheet.sequences().count(); i++)
		{
			if(i != iSequence && m_Sheet.sequences().at(i).iNumber == iNumber)
			{
				QMessageBox::warning(this, tr("Sprite Sheet"),
					tr("Sequence number %1 is already used.").arg(iNumber));

				m_bUpdating = true;
				m_pSequenceNumber->setValue(m_Sheet.sequences().at(iSequence).iNumber);
				m_bUpdating = false;
				return;
			}
		}

		SheetSequence &Sequence = m_Sheet.sequences()[iSequence];
		Sequence.iNumber = iNumber;
		Sequence.bClamp = !m_pSequenceLoop->isChecked();

		if(QTreeWidgetItem *pItem = m_pTree->currentItem())
		{
			pItem->setText(0, sequenceLabel(Sequence));
		}
	}

	void SheetDialog::onFrameEdited()
	{
		if(m_bUpdating)
		{
			return;
		}

		const int iSequence = selectedSequence();
		const int iFrame = selectedFrame();

		if(iSequence < 0 || iFrame < 0)
		{
			return;
		}

		SheetFrame &Frame = m_Sheet.sequences()[iSequence].Frames[iFrame];
		Frame.fDuration = static_cast<float>(m_pFrameDuration->value());

		const bool bLinked = m_pLinkImages->isChecked();

		m_bUpdating = true;

		for(int i = 0; i < SheetImagesPerFrame; i++)
		{
			if(bLinked && i > 0)
			{
				m_pImageX[i]->setValue(m_pImageX[0]->value());
				m_pImageY[i]->setValue(m_pImageY[0]->value());
				m_pImageWidth[i]->setValue(m_pImageWidth[0]->value());
				m_pImageHeight[i]->setValue(m_pImageHeight[0]->value());

				Frame.Images[i] = Frame.Images[0];
				continue;
			}

			// keep the rectangle inside the texture
			const int iX = qBound(0, m_pImageX[i]->value(), m_iImageWidth - 1);
			const int iY = qBound(0, m_pImageY[i]->value(), m_iImageHeight - 1);
			const int iWidth = qBound(1, m_pImageWidth[i]->value(), m_iImageWidth - iX);
			const int iHeight = qBound(1, m_pImageHeight[i]->value(), m_iImageHeight - iY);

			m_pImageX[i]->setValue(iX);
			m_pImageY[i]->setValue(iY);
			m_pImageWidth[i]->setValue(iWidth);
			m_pImageHeight[i]->setValue(iHeight);

			Frame.Images[i] = SheetFile::rectToCoords(iX, iY, iWidth, iHeight,
				m_iImageWidth, m_iImageHeight);
		}

		m_bUpdating = false;

		if(QTreeWidgetItem *pItem = m_pTree->currentItem())
		{
			pItem->setText(0, frameLabel(iFrame, Frame));
		}

		if(m_iPlaySequence < 0)
		{
			m_pPreview->setFrame(Frame, true);
		}
	}

	void SheetDialog::onLinkImagesToggled(bool bChecked)
	{
		for(int i = 1; i < SheetImagesPerFrame; i++)
		{
			m_pImageRows[i]->setEnabled(!bChecked);
			m_pImageX[i]->setEnabled(!bChecked);
			m_pImageY[i]->setEnabled(!bChecked);
			m_pImageWidth[i]->setEnabled(!bChecked);
			m_pImageHeight[i]->setEnabled(!bChecked);
		}

		onFrameEdited();
	}

	//
	// Playback.
	//

	void SheetDialog::onPlayClicked()
	{
		if(m_iPlaySequence >= 0)
		{
			stopPlayback();
		}
		else
		{
			startPlayback();
		}

		updatePlaybackControls();
	}

	void SheetDialog::startPlayback()
	{
		const int iSequence = selectedSequence();
		if(iSequence < 0 || m_Sheet.sequences().at(iSequence).Frames.isEmpty())
		{
			return;
		}

		m_iPlaySequence = iSequence;
		m_iPlayFrame = 0;

		m_pPreview->setAnimating(true);
		showPlaybackFrame();
		schedulePlaybackFrame();
	}

	void SheetDialog::stopPlayback()
	{
		if(m_iPlaySequence < 0)
		{
			return;
		}

		m_pPlayTimer->stop();
		m_iPlaySequence = -1;
		m_iPlayFrame = 0;

		m_pPreview->setAnimating(false);
		m_pPreview->clearFrame();

		updateEditor();
		updatePlaybackControls();
	}

	void SheetDialog::onPlaybackTimeout()
	{
		if(m_iPlaySequence < 0 || m_iPlaySequence >= m_Sheet.sequences().count())
		{
			stopPlayback();
			updatePlaybackControls();
			return;
		}

		const SheetSequence &Sequence = m_Sheet.sequences().at(m_iPlaySequence);

		if(m_iPlayFrame + 1 >= Sequence.Frames.count())
		{
			if(Sequence.bClamp)
			{
				// hold the last frame
				stopPlayback();
				updatePlaybackControls();
				return;
			}

			m_iPlayFrame = 0;
		}
		else
		{
			m_iPlayFrame++;
		}

		showPlaybackFrame();
		schedulePlaybackFrame();
		updatePlaybackControls();
	}

	void SheetDialog::showPlaybackFrame()
	{
		const SheetSequence &Sequence = m_Sheet.sequences().at(m_iPlaySequence);
		m_pPreview->setFrame(Sequence.Frames.at(m_iPlayFrame), true);
	}

	void SheetDialog::schedulePlaybackFrame()
	{
		const SheetSequence &Sequence = m_Sheet.sequences().at(m_iPlaySequence);

		const double fSpeed = qMax(0.01, m_pPlaySpeed->value());
		const double fDuration = Sequence.Frames.at(m_iPlayFrame).fDuration / fSpeed;

		m_pPlayTimer->start(qBound(10, qRound(fDuration * 1000.0), 60000));
	}

	void SheetDialog::updatePlaybackControls()
	{
		const int iSequence = selectedSequence();
		const bool bPlaying = m_iPlaySequence >= 0;

		m_pPlayButton->setText(bPlaying ? tr("&Stop") : tr("&Play"));
		m_pPlayButton->setEnabled(bPlaying
			|| (iSequence >= 0 && !m_Sheet.sequences().at(iSequence).Frames.isEmpty()));

		if(bPlaying)
		{
			m_pPlayStatus->setText(tr("Frame %1 of %2")
				.arg(m_iPlayFrame)
				.arg(m_Sheet.sequences().at(m_iPlaySequence).Frames.count() - 1));
		}
		else
		{
			m_pPlayStatus->clear();
		}
	}

	//
	// Grid generation.
	//

	void SheetDialog::onGenerateGrid()
	{
		QDialog Dialog(this);
		Dialog.setWindowTitle(tr("Generate Grid"));

		QSpinBox *pColumns = new QSpinBox(&Dialog);
		pColumns->setRange(1, m_iImageWidth);
		pColumns->setValue(qMin(4, m_iImageWidth));

		QSpinBox *pRows = new QSpinBox(&Dialog);
		pRows->setRange(1, m_iImageHeight);
		pRows->setValue(qMin(4, m_iImageHeight));

		QSpinBox *pCount = new QSpinBox(&Dialog);
		pCount->setRange(1, m_iImageWidth * m_iImageHeight);
		pCount->setValue(pColumns->value() * pRows->value());
		pCount->setToolTip(tr("How many cells to turn into frames, in left to right, top to bottom order."));

		QDoubleSpinBox *pDuration = new QDoubleSpinBox(&Dialog);
		pDuration->setDecimals(4);
		pDuration->setSingleStep(0.1);
		pDuration->setRange(0.0001, 10000.0);
		pDuration->setValue(1.0);

		QCheckBox *pLoop = new QCheckBox(tr("Loop"), &Dialog);
		pLoop->setChecked(true);

		QLabel *pCellSize = new QLabel(&Dialog);

		// keep cell size readout and the frame count ceiling in step with the grid
		auto UpdateGrid = [&]()
		{
			const int iCellWidth = m_iImageWidth / pColumns->value();
			const int iCellHeight = m_iImageHeight / pRows->value();
			const int iCells = pColumns->value() * pRows->value();

			pCellSize->setText(tr("%1 x %2 pixels").arg(iCellWidth).arg(iCellHeight));

			pCount->setMaximum(iCells);
			pCount->setValue(iCells);
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
		pForm->addRow(tr("Frames:"), pCount);
		pForm->addRow(tr("Cell size:"), pCellSize);
		pForm->addRow(tr("Frame duration:"), pDuration);
		pForm->addRow(QString(), pLoop);

		QVBoxLayout *pLayout = new QVBoxLayout(&Dialog);
		pLayout->addLayout(pForm);
		pLayout->addWidget(pButtons);

		if(Dialog.exec() != QDialog::Accepted)
		{
			return;
		}

		const int iNumber = m_Sheet.nextFreeSequenceNumber();
		if(iNumber < 0)
		{
			QMessageBox::warning(this, tr("Sprite Sheet"),
				tr("A sheet cannot hold more than %1 sequences.").arg(int(SheetMaxSequences)));
			return;
		}

		const int iColumns = pColumns->value();
		const int iCellWidth = m_iImageWidth / iColumns;
		const int iCellHeight = m_iImageHeight / pRows->value();

		SheetSequence Sequence;
		Sequence.iNumber = iNumber;
		Sequence.bClamp = !pLoop->isChecked();

		for(int i = 0; i < pCount->value(); i++)
		{
			SheetFrame Frame;
			Frame.fDuration = static_cast<float>(pDuration->value());
			Frame.Images[0] = SheetFile::rectToCoords(
				(i % iColumns) * iCellWidth, (i / iColumns) * iCellHeight,
				iCellWidth, iCellHeight, m_iImageWidth, m_iImageHeight);

			for(int j = 1; j < SheetImagesPerFrame; j++)
			{
				Frame.Images[j] = Frame.Images[0];
			}

			Sequence.Frames.append(Frame);
		}

		m_Sheet.sequences().append(Sequence);

		rebuildTree(m_Sheet.sequences().count() - 1, -1);
	}
}
