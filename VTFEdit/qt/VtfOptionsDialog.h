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

#include <QDialog>

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QSpinBox;

namespace VTFEdit
{
	class VtfOptionsDialog : public QDialog
	{
		Q_OBJECT

	public:
		explicit VtfOptionsDialog(VtfOptions *pOptions, QWidget *pParent = nullptr);

		int exec();

	private slots:
		void onResetClicked();
		void updateEnabledState();

	private:
		QWidget *createGeneralTab();
		QWidget *createAdvancedTab();
		QWidget *createResourcesTab();

		void optionsToControls();
		void controlsToOptions();

		VtfOptions *m_pOptions;

		QComboBox *m_pFormat;
		QComboBox *m_pAlphaFormat;
		QLabel *m_pAlphaFormatLabel;
		QComboBox *m_pTextureType;

		QCheckBox *m_pFlagClampS;
		QCheckBox *m_pFlagClampT;
		QCheckBox *m_pFlagNoLOD;
		QCheckBox *m_pFlagPointSample;
		QCheckBox *m_pSrgb;

		QCheckBox *m_pResize;
		QComboBox *m_pResizeMethod;
		QComboBox *m_pResizeFilter;
		QCheckBox *m_pResizeClamp;
		QComboBox *m_pMaximumWidth;
		QComboBox *m_pMaximumHeight;

		QCheckBox *m_pMipmaps;
		QComboBox *m_pMipmapFilter;

		QComboBox *m_pVersion;
		QComboBox *m_pCompressionLevel;
		QComboBox *m_pCompressionMethod;

		QCheckBox *m_pReflectivity;
		QCheckBox *m_pThumbnail;
		QCheckBox *m_pSphereMap;
		QCheckBox *m_pStripAlpha;

		QCheckBox *m_pDistanceAlpha;
		QDoubleSpinBox *m_pDistanceAlphaSpread;
		QComboBox *m_pDistanceAlphaReduce;
		QSpinBox *m_pDistanceAlphaThreshold;

		QCheckBox *m_pGammaCorrection;
		QDoubleSpinBox *m_pGammaCorrectionValue;

		QDoubleSpinBox *m_pLuminanceWeightR;
		QDoubleSpinBox *m_pLuminanceWeightG;
		QDoubleSpinBox *m_pLuminanceWeightB;

		QCheckBox *m_pCreateLODControlResource;
		QSpinBox *m_pLODControlClampU;
		QSpinBox *m_pLODControlClampV;

		QCheckBox *m_pCreateInformationResource;
		QLineEdit *m_pInformationAuthor;
		QLineEdit *m_pInformationContact;
		QLineEdit *m_pInformationVersion;
		QLineEdit *m_pInformationModification;
		QLineEdit *m_pInformationDescription;
		QLineEdit *m_pInformationComments;
	};
}
