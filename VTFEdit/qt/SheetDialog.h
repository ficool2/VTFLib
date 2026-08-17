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

#include "SheetFile.h"

#include <QDialog>
#include <QImage>
#include <QVector>
#include <QWidget>

class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QStackedWidget;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;

namespace VTFEdit
{
	class SheetPreview : public QWidget
	{
	public:
		explicit SheetPreview(QWidget *pParent = nullptr);

		void setImage(const QImage &Image);
		void setFrame(const SheetFrame &Frame, bool bValid);
		void clearFrame();

		void setAnimating(bool bAnimating);

	protected:
		void paintEvent(QPaintEvent *pEvent) override;

	private:
		QImage m_Image;
		SheetFrame m_Frame;
		bool m_bHasFrame;
		bool m_bAnimating;
	};

	class SheetDialog : public QDialog
	{
		Q_OBJECT

	public:
		SheetDialog(const SheetFile &Sheet, const QImage &Image,
			int iImageWidth, int iImageHeight, QWidget *pParent = nullptr);

		const SheetFile &sheet() const { return m_Sheet; }

	private slots:
		void onSelectionChanged();
		void onAddSequence();
		void onAddFrame();
		void onDuplicate();
		void onRemove();
		void onMoveUp();
		void onMoveDown();
		void onGenerateGrid();
		void onSequenceEdited();
		void onFrameEdited();
		void onLinkImagesToggled(bool bChecked);
		void onPlayClicked();
		void onPlaybackTimeout();

	private:
		void startPlayback();
		void stopPlayback();
		void showPlaybackFrame();
		void schedulePlaybackFrame();
		void updatePlaybackControls();

		QWidget *createSequenceEditor();
		QWidget *createFrameEditor();

		void rebuildTree(int iSequence, int iFrame);
		void updateEditor();
		void updateButtons();

		int selectedSequence() const;
		int selectedFrame() const;

		void move(int iDirection);

		SheetFile m_Sheet;
		QImage m_Image;
		int m_iImageWidth;
		int m_iImageHeight;
		bool m_bUpdating;

		QTreeWidget *m_pTree;
		QStackedWidget *m_pEditorStack;
		QWidget *m_pEmptyEditor;
		QWidget *m_pSequenceEditor;
		QWidget *m_pFrameEditor;
		SheetPreview *m_pPreview;

		QPushButton *m_pPlayButton;
		QDoubleSpinBox *m_pPlaySpeed;
		QLabel *m_pPlayStatus;
		QTimer *m_pPlayTimer;
		int m_iPlaySequence;
		int m_iPlayFrame;

		QPushButton *m_pAddFrameButton;
		QPushButton *m_pDuplicateButton;
		QPushButton *m_pRemoveButton;
		QPushButton *m_pMoveUpButton;
		QPushButton *m_pMoveDownButton;

		QSpinBox *m_pSequenceNumber;
		QCheckBox *m_pSequenceLoop;
		QLabel *m_pSequenceSummary;

		QDoubleSpinBox *m_pFrameDuration;
		QCheckBox *m_pLinkImages;
		QSpinBox *m_pImageX[SheetImagesPerFrame];
		QSpinBox *m_pImageY[SheetImagesPerFrame];
		QSpinBox *m_pImageWidth[SheetImagesPerFrame];
		QSpinBox *m_pImageHeight[SheetImagesPerFrame];
		QWidget *m_pImageRows[SheetImagesPerFrame];
	};
}
