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

#include "VtfOptionsDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QVBoxLayout>

namespace VTFEdit
{
	namespace
	{
		struct ResizeMethodEntry
		{
			const char *pName;
			VTFResizeMethod Method;
		};

		const ResizeMethodEntry ResizeMethods[] = {
			{ "Nearest Power Of 2", RESIZE_NEAREST_POWER2 },
			{ "Biggest Power Of 2", RESIZE_BIGGEST_POWER2 },
			{ "Smallest Power Of 2", RESIZE_SMALLEST_POWER2 },
			{ "Nearest Multiple Of 4", RESIZE_NEAREST_MULTIPLE4 },
			{ "Biggest Multiple Of 4", RESIZE_BIGGEST_MULTIPLE4 },
			{ "Smallest Multiple Of 4", RESIZE_SMALLEST_MULTIPLE4 },
		};
		struct MipmapFilterEntry
		{
			const char *pName;
			VTFMipmapFilter Filter;
		};

		const MipmapFilterEntry MipmapFilters[] = {
			{ "Mitchell", MIPMAP_FILTER_MITCHELL },
			{ "Box", MIPMAP_FILTER_BOX },
			{ "Triangle", MIPMAP_FILTER_TRIANGLE },
			{ "Cubic", MIPMAP_FILTER_CUBIC },
			{ "Catrom", MIPMAP_FILTER_CATROM },
			{ "NICE", MIPMAP_FILTER_NICE },
		};
		const char *const VersionNames[] = { "7.6", "7.5", "7.4", "7.3", "7.2", "7.1", "7.0" };
		const char *const CompressionLevelNames[] = { "None", "Default", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
		const char *const CompressionMethodNames[] = { "Deflate", "Zstandard" };

		QStringList powerOfTwoList()
		{
			QStringList list;
			for(int i = 1; i <= 16384; i *= 2)
			{
				list << QString::number(i);
			}
			return list;
		}

		void fillMipmapFilters(QComboBox *pCombo)
		{
			for(const MipmapFilterEntry &Entry : MipmapFilters)
			{
				pCombo->addItem(QString::fromLatin1(Entry.pName), static_cast<int>(Entry.Filter));
			}
		}

		void setMipmapFilter(QComboBox *pCombo, VTFMipmapFilter Filter)
		{
			const int iIndex = pCombo->findData(static_cast<int>(Filter));
			pCombo->setCurrentIndex(iIndex >= 0 ? iIndex : 0);
		}

		VTFMipmapFilter mipmapFilter(const QComboBox *pCombo)
		{
			return static_cast<VTFMipmapFilter>(pCombo->currentData().toInt());
		}

		void fillResizeMethods(QComboBox *pCombo)
		{
			for(const ResizeMethodEntry &Entry : ResizeMethods)
			{
				pCombo->addItem(QString::fromLatin1(Entry.pName), static_cast<int>(Entry.Method));
			}
		}

		template <int N>
		void fill(QComboBox *pCombo, const char *const (&names)[N])
		{
			for(int i = 0; i < N; i++)
			{
				pCombo->addItem(QString::fromLatin1(names[i]));
			}
		}

		template <int N>
		void fillImageFormats(QComboBox *pCombo, const ImageFormatEntry (&Entries)[N])
		{
			for(int i = 0; i < N; i++)
			{
				pCombo->addItem(QString::fromLatin1(Entries[i].pName));
			}
		}

		template <int N>
		void updateImageFormatsEnabledState(QComboBox *pCombo, const ImageFormatEntry (&Entries)[N], bool bIsVersion76)
		{
			QStandardItemModel *pModel = qobject_cast<QStandardItemModel *>(pCombo->model());
			if(pModel == nullptr)
			{
				return;
			}

			for(int i = 0; i < N; i++)
			{
				const bool bEnabled = bIsVersion76 || !Entries[i].bRequiresVersion76;
				QStandardItem *pItem = pModel->item(i);
				if(pItem != nullptr)
				{
					pItem->setFlags(bEnabled
						? pItem->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable
						: pItem->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable));
				}
			}
		}
	}

	VtfOptionsDialog::VtfOptionsDialog(VtfOptions *pOptions, QWidget *pParent)
		: QDialog(pParent)
		, m_pOptions(pOptions)
	{
		setWindowTitle(tr("VTF Options"));

		QTabWidget *pTabs = new QTabWidget(this);
		pTabs->addTab(createGeneralTab(), tr("General"));
		pTabs->addTab(createAdvancedTab(), tr("Advanced"));
		pTabs->addTab(createResourcesTab(), tr("Resources"));

		QDialogButtonBox *pButtons = new QDialogButtonBox(
			QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset, this);
		connect(pButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
		connect(pButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
		connect(pButtons->button(QDialogButtonBox::Reset), &QPushButton::clicked,
			this, &VtfOptionsDialog::onResetClicked);

		QVBoxLayout *pLayout = new QVBoxLayout(this);
		pLayout->addWidget(pTabs);
		pLayout->addWidget(pButtons);

		connect(m_pStripAlpha, &QCheckBox::toggled, this, &VtfOptionsDialog::updateEnabledState);
		connect(m_pResize, &QCheckBox::toggled, this, &VtfOptionsDialog::updateEnabledState);
		connect(m_pResizeClamp, &QCheckBox::toggled, this, &VtfOptionsDialog::updateEnabledState);
		connect(m_pMipmaps, &QCheckBox::toggled, this, &VtfOptionsDialog::updateEnabledState);
		connect(m_pGammaCorrection, &QCheckBox::toggled, this, &VtfOptionsDialog::updateEnabledState);
		connect(m_pDistanceAlpha, &QCheckBox::toggled, this, &VtfOptionsDialog::updateEnabledState);
		connect(m_pCreateLODControlResource, &QCheckBox::toggled, this, &VtfOptionsDialog::updateEnabledState);
		connect(m_pCreateInformationResource, &QCheckBox::toggled, this, &VtfOptionsDialog::updateEnabledState);
		connect(m_pTextureType, &QComboBox::currentIndexChanged, this, &VtfOptionsDialog::updateEnabledState);
		connect(m_pVersion, &QComboBox::currentIndexChanged, this, &VtfOptionsDialog::updateEnabledState);
		connect(m_pCompressionLevel, &QComboBox::currentIndexChanged, this, &VtfOptionsDialog::updateEnabledState);
	}

	QWidget *VtfOptionsDialog::createGeneralTab()
	{
		QWidget *pTab = new QWidget(this);
		QHBoxLayout *pLayout = new QHBoxLayout(pTab);
		QVBoxLayout *pLeft = new QVBoxLayout();
		QVBoxLayout *pRight = new QVBoxLayout();
		pLayout->addLayout(pLeft);
		pLayout->addLayout(pRight);

		QGroupBox *pGeneral = new QGroupBox(tr("General:"), pTab);
		QFormLayout *pGeneralForm = new QFormLayout(pGeneral);
		m_pFormat = new QComboBox(pGeneral);
		fillImageFormats(m_pFormat, NormalImageFormats);
		m_pFormat->setToolTip(tr("The output image format for textures with no alpha channel. "
			"Common values are DXT1 and RGB888."));
		m_pAlphaFormat = new QComboBox(pGeneral);
		fillImageFormats(m_pAlphaFormat, AlphaImageFormats);
		m_pAlphaFormat->setToolTip(tr("The output image format for textures with an alpha channel. "
			"Common values are DXT5 and RGBA8888."));
		m_pTextureType = new QComboBox(pGeneral);
		m_pTextureType->addItems({ tr("Animated Texture"), tr("Environment Map"), tr("Volume Texture") });
		m_pVersion = new QComboBox(pGeneral);
		fill(m_pVersion, VersionNames);
		m_pVersion->setToolTip(tr("The VTF file version. 7.4 has the best compatibility, "
			"7.5 is only supported in newer branches, and 7.6 is Strata Source only."));
		pGeneralForm->addRow(tr("Version:"), m_pVersion);
		pGeneralForm->addRow(tr("Color Format:"), m_pFormat);
		m_pAlphaFormatLabel = new QLabel(tr("Alpha Format:"), pGeneral);
		pGeneralForm->addRow(m_pAlphaFormatLabel, m_pAlphaFormat);
		pGeneralForm->addRow(tr("Texture Type:"), m_pTextureType);

		m_pFlagClampS = new QCheckBox(tr("Clamp S"), pGeneral);
		m_pFlagClampS->setToolTip(tr("Clamps the texture horizontally instead of tiling it."));
		m_pFlagClampT = new QCheckBox(tr("Clamp T"), pGeneral);
		m_pFlagClampT->setToolTip(tr("Clamps the texture vertically instead of tiling it."));
		m_pFlagNoLOD = new QCheckBox(tr("No LOD"), pGeneral);
		m_pFlagNoLOD->setToolTip(tr("Makes the texture ignore the user's texture detail setting."));
		m_pFlagPointSample = new QCheckBox(tr("Point Sample"), pGeneral);
		m_pFlagPointSample->setToolTip(tr("Disables texture filtering, giving the texture a blocky appearance."));
		QWidget *pFlags = new QWidget(pGeneral);
		QGridLayout *pFlagsGrid = new QGridLayout(pFlags);
		pFlagsGrid->setContentsMargins(0, 0, 0, 0);
		pFlagsGrid->addWidget(m_pFlagClampS, 0, 0);
		pFlagsGrid->addWidget(m_pFlagClampT, 0, 1);
		pFlagsGrid->addWidget(m_pFlagNoLOD, 1, 0);
		pFlagsGrid->addWidget(m_pFlagPointSample, 1, 1);
		pFlagsGrid->setColumnStretch(0, 1);
		pFlagsGrid->setColumnStretch(1, 1);

		m_pStripAlpha = new QCheckBox(tr("Strip alpha channel"), pGeneral);
		m_pStripAlpha->setToolTip(tr("Discard the alpha channel of imported images.\n"
			"The colour format is always used when this is enabled."));
		pGeneralForm->addRow(m_pStripAlpha);

		m_pSrgb = new QCheckBox(tr("sRGB"), pGeneral);
		m_pSrgb->setToolTip(tr("Marks the texture as storing standard image colours.\n"
			"This affects how mipmaps and resizing filters are computed.\n"
			"Leave this off for textures that store data rather than color,\n"
			"such as normal maps, masks, exponent maps and UI icons."));
		pGeneralForm->addRow(m_pSrgb);

		pGeneralForm->addRow(new QLabel(tr("Flags:"), pGeneral));
		pGeneralForm->addRow(pFlags);

		QGroupBox *pResize = new QGroupBox(tr("Resize:"), pTab);
		QFormLayout *pResizeForm = new QFormLayout(pResize);
		m_pResize = new QCheckBox(tr("Resize image"), pResize);
		m_pResizeMethod = new QComboBox(pResize);
		fillResizeMethods(m_pResizeMethod);
		m_pResizeMethod->setToolTip(tr("The size to round the image dimensions to. DXT compressed formats only require multiples of 4."));
		m_pResizeFilter = new QComboBox(pResize);
		fillMipmapFilters(m_pResizeFilter);
		m_pResizeClamp = new QCheckBox(tr("Clamp resize dimensions"), pResize);
		m_pMaximumWidth = new QComboBox(pResize);
		m_pMaximumWidth->addItems(powerOfTwoList());
		m_pMaximumWidth->setToolTip(tr("Maximum width."));
		m_pMaximumHeight = new QComboBox(pResize);
		m_pMaximumHeight->addItems(powerOfTwoList());
		m_pMaximumHeight->setToolTip(tr("Maximum height."));
		pResizeForm->addRow(m_pResize);
		pResizeForm->addRow(tr("Resize Method:"), m_pResizeMethod);
		pResizeForm->addRow(tr("Resize Filter:"), m_pResizeFilter);
		pResizeForm->addRow(m_pResizeClamp);
		pResizeForm->addRow(tr("Maximum Width:"), m_pMaximumWidth);
		pResizeForm->addRow(tr("Maximum Height:"), m_pMaximumHeight);

		QGroupBox *pMipmaps = new QGroupBox(tr("Mipmaps:"), pTab);
		QFormLayout *pMipmapsForm = new QFormLayout(pMipmaps);
		m_pMipmaps = new QCheckBox(tr("Generate mipmaps"), pMipmaps);
		m_pMipmapFilter = new QComboBox(pMipmaps);
		fillMipmapFilters(m_pMipmapFilter);
		pMipmapsForm->addRow(m_pMipmaps);
		pMipmapsForm->addRow(tr("Mipmap Filter:"), m_pMipmapFilter);

		pLeft->addWidget(pGeneral);
		pLeft->addStretch();
		pRight->addWidget(pResize);
		pRight->addWidget(pMipmaps);
		pRight->addStretch();

		return pTab;
	}

	QWidget *VtfOptionsDialog::createAdvancedTab()
	{
		QWidget *pTab = new QWidget(this);
		QHBoxLayout *pLayout = new QHBoxLayout(pTab);
		QVBoxLayout *pLeft = new QVBoxLayout();
		QVBoxLayout *pRight = new QVBoxLayout();
		pLayout->addLayout(pLeft);
		pLayout->addLayout(pRight);

		QGroupBox *pGamma = new QGroupBox(tr("Gamma Correction:"), pTab);
		QFormLayout *pGammaForm = new QFormLayout(pGamma);
		m_pGammaCorrection = new QCheckBox(tr("Correct gamma"), pGamma);
		m_pGammaCorrectionValue = new QDoubleSpinBox(pGamma);
		m_pGammaCorrectionValue->setDecimals(2);
		m_pGammaCorrectionValue->setSingleStep(0.05);
		m_pGammaCorrectionValue->setRange(0.0, 100.0);
		pGammaForm->addRow(m_pGammaCorrection);
		pGammaForm->addRow(tr("Gamma Correction:"), m_pGammaCorrectionValue);

		QGroupBox *pMisc = new QGroupBox(tr("Miscellaneous:"), pTab);
		QVBoxLayout *pMiscLayout = new QVBoxLayout(pMisc);
		m_pReflectivity = new QCheckBox(tr("Compute reflectivity"), pMisc);
		m_pThumbnail = new QCheckBox(tr("Generate thumbnail"), pMisc);
		m_pSphereMap = new QCheckBox(tr("Generate sphere map"), pMisc);
		pMiscLayout->addWidget(m_pReflectivity);
		pMiscLayout->addWidget(m_pThumbnail);
		pMiscLayout->addWidget(m_pSphereMap);

		QGroupBox *pDistanceAlpha = new QGroupBox(tr("Distance Alpha:"), pTab);
		QFormLayout *pDistanceAlphaForm = new QFormLayout(pDistanceAlpha);
		m_pDistanceAlpha = new QCheckBox(tr("Encode alpha as a distance field"), pDistanceAlpha);
		m_pDistanceAlpha->setToolTip(tr("Replaces the alpha channel with a signed distance field, for use with $distancealpha. "
			"Mipmaps and compression should usually be disabled."));
		m_pDistanceAlphaSpread = new QDoubleSpinBox(pDistanceAlpha);
		m_pDistanceAlphaSpread->setDecimals(1);
		m_pDistanceAlphaSpread->setSingleStep(0.5);
		m_pDistanceAlphaSpread->setRange(0.1, 64.0);
		m_pDistanceAlphaSpread->setToolTip(tr("Width in pixels of the gradient either side of the edge."));
		m_pDistanceAlphaReduce = new QComboBox(pDistanceAlpha);
		for(int i = 0; i < 6; i++)
		{
			m_pDistanceAlphaReduce->addItem(QStringLiteral("1/%1").arg(1 << i), 1 << i);
		}
		m_pDistanceAlphaReduce->setToolTip(tr("Shrinks the image after the distance field has been computed "
			"at the source resolution."));
		pDistanceAlphaForm->addRow(m_pDistanceAlpha);
		pDistanceAlphaForm->addRow(tr("Spread:"), m_pDistanceAlphaSpread);
		m_pDistanceAlphaThreshold = new QSpinBox(pDistanceAlpha);
		m_pDistanceAlphaThreshold->setRange(0, 255);
		m_pDistanceAlphaThreshold->setToolTip(tr("Source alpha above which a pixel counts as being inside "
			"the shape. Raise this for soft or anti-aliased edges."));
		pDistanceAlphaForm->addRow(tr("Scale:"), m_pDistanceAlphaReduce);
		pDistanceAlphaForm->addRow(tr("Threshold:"), m_pDistanceAlphaThreshold);

		QGroupBox *pLuminance = new QGroupBox(tr("Luminance Weights:"), pTab);
		QFormLayout *pLuminanceForm = new QFormLayout(pLuminance);
		QDoubleSpinBox **ppWeights[] = { &m_pLuminanceWeightR, &m_pLuminanceWeightG, &m_pLuminanceWeightB };
		const QString sLabels[] = { tr("Red:"), tr("Green:"), tr("Blue:") };
		for(int i = 0; i < 3; i++)
		{
			QDoubleSpinBox *pWeight = new QDoubleSpinBox(pLuminance);
			pWeight->setDecimals(3);
			pWeight->setSingleStep(0.001);
			pWeight->setRange(0.0, 1.0);
			*ppWeights[i] = pWeight;
			pLuminanceForm->addRow(sLabels[i], pWeight);
		}

		pLeft->addWidget(pGamma);
		pLeft->addWidget(pLuminance);
		pLeft->addStretch();
		pRight->addWidget(pMisc);
		pRight->addWidget(pDistanceAlpha);
		pRight->addStretch();

		return pTab;
	}

	QWidget *VtfOptionsDialog::createResourcesTab()
	{
		QWidget *pTab = new QWidget(this);
		QHBoxLayout *pLayout = new QHBoxLayout(pTab);
		QVBoxLayout *pLeft = new QVBoxLayout();
		QVBoxLayout *pRight = new QVBoxLayout();
		pLayout->addLayout(pLeft);
		pLayout->addLayout(pRight);

		QGroupBox *pLOD = new QGroupBox(tr("LOD Control Resource:"), pTab);
		QFormLayout *pLODForm = new QFormLayout(pLOD);
		m_pCreateLODControlResource = new QCheckBox(tr("Create LOD control resource"), pLOD);
		m_pLODControlClampU = new QSpinBox(pLOD);
		m_pLODControlClampU->setRange(0, 31);
		m_pLODControlClampV = new QSpinBox(pLOD);
		m_pLODControlClampV->setRange(0, 31);
		pLODForm->addRow(m_pCreateLODControlResource);
		pLODForm->addRow(tr("Clamp U:"), m_pLODControlClampU);
		pLODForm->addRow(tr("Clamp V:"), m_pLODControlClampV);

		QGroupBox *pCompression = new QGroupBox(tr("Compression (version 7.6 only):"), pTab);
		pCompression->setToolTip(tr("CPU compression is only available in version 7.6 files"));
		QFormLayout *pCompressionForm = new QFormLayout(pCompression);
		m_pCompressionLevel = new QComboBox(pCompression);
		fill(m_pCompressionLevel, CompressionLevelNames);
		m_pCompressionMethod = new QComboBox(pCompression);
		fill(m_pCompressionMethod, CompressionMethodNames);
		pCompressionForm->addRow(tr("Compression Level:"), m_pCompressionLevel);
		pCompressionForm->addRow(tr("Compression Method:"), m_pCompressionMethod);

		QGroupBox *pInformation = new QGroupBox(tr("Information Resource:"), pTab);
		QFormLayout *pInformationForm = new QFormLayout(pInformation);
		m_pCreateInformationResource = new QCheckBox(tr("Create information resource"), pInformation);
		m_pInformationAuthor = new QLineEdit(pInformation);
		m_pInformationContact = new QLineEdit(pInformation);
		m_pInformationVersion = new QLineEdit(pInformation);
		m_pInformationModification = new QLineEdit(pInformation);
		m_pInformationDescription = new QLineEdit(pInformation);
		m_pInformationComments = new QLineEdit(pInformation);
		pInformationForm->addRow(m_pCreateInformationResource);
		pInformationForm->addRow(tr("Author:"), m_pInformationAuthor);
		pInformationForm->addRow(tr("Contact:"), m_pInformationContact);
		pInformationForm->addRow(tr("Version:"), m_pInformationVersion);
		pInformationForm->addRow(tr("Modification:"), m_pInformationModification);
		pInformationForm->addRow(tr("Description:"), m_pInformationDescription);
		pInformationForm->addRow(tr("Comments:"), m_pInformationComments);

		pLeft->addWidget(pLOD);
		pLeft->addWidget(pCompression);
		pLeft->addStretch();
		pRight->addWidget(pInformation);
		pRight->addStretch();

		return pTab;
	}

	int VtfOptionsDialog::exec()
	{
		optionsToControls();
		updateEnabledState();

		const int iResult = QDialog::exec();

		if(iResult == QDialog::Accepted)
		{
			controlsToOptions();
		}

		return iResult;
	}

	void VtfOptionsDialog::onResetClicked()
	{
		VtfOptions Defaults;
		*m_pOptions = Defaults;

		optionsToControls();
		updateEnabledState();
	}

	void VtfOptionsDialog::updateEnabledState()
	{
		m_pSphereMap->setEnabled(static_cast<VtfTextureType>(m_pTextureType->currentIndex()) == VtfTextureType::EnvironmentMap);

		const bool bAlphaFormat = !m_pStripAlpha->isChecked();
		m_pAlphaFormat->setEnabled(bAlphaFormat);
		if(m_pAlphaFormatLabel != nullptr)
			m_pAlphaFormatLabel->setEnabled(bAlphaFormat);

		m_pResizeMethod->setEnabled(m_pResize->isChecked());
		m_pResizeFilter->setEnabled(m_pResize->isChecked());
		m_pResizeClamp->setEnabled(m_pResize->isChecked());
		m_pMaximumWidth->setEnabled(m_pResize->isChecked() && m_pResizeClamp->isChecked());
		m_pMaximumHeight->setEnabled(m_pResize->isChecked() && m_pResizeClamp->isChecked());

		m_pMipmapFilter->setEnabled(m_pMipmaps->isChecked());

		m_pGammaCorrectionValue->setEnabled(m_pGammaCorrection->isChecked());

		m_pDistanceAlphaSpread->setEnabled(m_pDistanceAlpha->isChecked());
		m_pDistanceAlphaReduce->setEnabled(m_pDistanceAlpha->isChecked());
		m_pDistanceAlphaThreshold->setEnabled(m_pDistanceAlpha->isChecked());

		m_pLODControlClampU->setEnabled(m_pCreateLODControlResource->isChecked());
		m_pLODControlClampV->setEnabled(m_pCreateLODControlResource->isChecked());

		const bool bInformation = m_pCreateInformationResource->isChecked();
		m_pInformationAuthor->setEnabled(bInformation);
		m_pInformationContact->setEnabled(bInformation);
		m_pInformationVersion->setEnabled(bInformation);
		m_pInformationModification->setEnabled(bInformation);
		m_pInformationDescription->setEnabled(bInformation);
		m_pInformationComments->setEnabled(bInformation);

		const bool bIsVersion76 = m_pVersion->currentText() == QLatin1String("7.6");
		updateImageFormatsEnabledState(m_pFormat, NormalImageFormats, bIsVersion76);
		updateImageFormatsEnabledState(m_pAlphaFormat, AlphaImageFormats, bIsVersion76);

		const bool bCompressionSupported = bIsVersion76;
		m_pCompressionLevel->setEnabled(bCompressionSupported);
		m_pCompressionMethod->setEnabled(bCompressionSupported && m_pCompressionLevel->currentIndex() != 0);
	}

	void VtfOptionsDialog::optionsToControls()
	{
		for(int i = 0; i < NormalImageFormatCount; i++)
		{
			if(NormalImageFormats[i].Format == m_pOptions->NormalFormat)
			{
				m_pFormat->setCurrentIndex(i);
			}
		}

		for(int i = 0; i < AlphaImageFormatCount; i++)
		{
			if(AlphaImageFormats[i].Format == m_pOptions->AlphaFormat)
			{
				m_pAlphaFormat->setCurrentIndex(i);
			}
		}

		m_pTextureType->setCurrentIndex(static_cast<int>(m_pOptions->TextureType));

		m_pFlagClampS->setChecked(m_pOptions->FlagClampS != vlFalse);
		m_pFlagClampT->setChecked(m_pOptions->FlagClampT != vlFalse);
		m_pFlagNoLOD->setChecked(m_pOptions->FlagNoLOD != vlFalse);
		m_pFlagPointSample->setChecked(m_pOptions->FlagPointSample != vlFalse);

		m_pResize->setChecked(m_pOptions->ResizeImage != vlFalse);
		const int iResizeMethodIndex = m_pResizeMethod->findData(static_cast<int>(m_pOptions->ResizeMethod));
		m_pResizeMethod->setCurrentIndex(iResizeMethodIndex >= 0 ? iResizeMethodIndex : 0);
		setMipmapFilter(m_pResizeFilter, m_pOptions->ResizeFilter);
		m_pResizeClamp->setChecked(m_pOptions->ResizeClamp != vlFalse);

		const int iWidthIndex = m_pMaximumWidth->findText(QString::number(m_pOptions->ResizeClampWidth));
		m_pMaximumWidth->setCurrentIndex(iWidthIndex >= 0 ? iWidthIndex : m_pMaximumWidth->count() - 1);
		const int iHeightIndex = m_pMaximumHeight->findText(QString::number(m_pOptions->ResizeClampHeight));
		m_pMaximumHeight->setCurrentIndex(iHeightIndex >= 0 ? iHeightIndex : m_pMaximumHeight->count() - 1);

		m_pMipmaps->setChecked(m_pOptions->GenerateMipmaps != vlFalse);
		setMipmapFilter(m_pMipmapFilter, m_pOptions->MipmapFilter);

		const int iVersionIndex = m_pVersion->findText(m_pOptions->Version);
		m_pVersion->setCurrentIndex(iVersionIndex >= 0 ? iVersionIndex : 2); // 7.4

		if(m_pOptions->AuxCompressionLevel == VTF_AUX_COMPRESSION_LEVEL_DEFAULT)
		{
			m_pCompressionLevel->setCurrentIndex(1);
		}
		else if(m_pOptions->AuxCompressionLevel <= VTF_AUX_COMPRESSION_LEVEL_NONE
			|| m_pOptions->AuxCompressionLevel > VTF_AUX_COMPRESSION_LEVEL_MAX)
		{
			m_pCompressionLevel->setCurrentIndex(0);
		}
		else
		{
			m_pCompressionLevel->setCurrentIndex(m_pOptions->AuxCompressionLevel + 1);
		}
		m_pCompressionMethod->setCurrentIndex(
			m_pOptions->AuxCompressionMethod == AUX_COMPRESSION_METHOD_ZSTD ? 1 : 0);

		m_pReflectivity->setChecked(m_pOptions->ComputeReflectivity != vlFalse);
		m_pThumbnail->setChecked(m_pOptions->GenerateThumbnail != vlFalse);
		m_pSphereMap->setChecked(m_pOptions->GenerateSphereMap != vlFalse);
		m_pStripAlpha->setChecked(m_pOptions->StripAlpha != vlFalse);
		m_pSrgb->setChecked(m_pOptions->sRGB != vlFalse);

		m_pDistanceAlpha->setChecked(m_pOptions->DistanceAlpha != vlFalse);
		m_pDistanceAlphaSpread->setValue(m_pOptions->DistanceAlphaSpread);
		const int iReduceIndex = m_pDistanceAlphaReduce->findData(static_cast<int>(m_pOptions->DistanceAlphaReduce));
		m_pDistanceAlphaReduce->setCurrentIndex(iReduceIndex >= 0 ? iReduceIndex : 0);
		m_pDistanceAlphaThreshold->setValue(static_cast<int>(m_pOptions->DistanceAlphaThreshold));

		m_pGammaCorrection->setChecked(m_pOptions->CorrectGamma != vlFalse);
		m_pGammaCorrectionValue->setValue(m_pOptions->GammaCorrection);

		m_pLuminanceWeightR->setValue(m_pOptions->LuminanceWeightR);
		m_pLuminanceWeightG->setValue(m_pOptions->LuminanceWeightG);
		m_pLuminanceWeightB->setValue(m_pOptions->LuminanceWeightB);

		m_pCreateLODControlResource->setChecked(m_pOptions->CreateLODControlResource != vlFalse);
		m_pLODControlClampU->setValue(static_cast<int>(m_pOptions->LODControlClampU));
		m_pLODControlClampV->setValue(static_cast<int>(m_pOptions->LODControlClampV));

		m_pCreateInformationResource->setChecked(m_pOptions->CreateInformationResource != vlFalse);
		m_pInformationAuthor->setText(m_pOptions->InformationAuthor);
		m_pInformationContact->setText(m_pOptions->InformationContact);
		m_pInformationVersion->setText(m_pOptions->InformationVersion);
		m_pInformationModification->setText(m_pOptions->InformationModification);
		m_pInformationDescription->setText(m_pOptions->InformationDescription);
		m_pInformationComments->setText(m_pOptions->InformationComments);
	}

	void VtfOptionsDialog::controlsToOptions()
	{
		m_pOptions->NormalFormat = m_pFormat->currentIndex() >= 0
			? NormalImageFormats[m_pFormat->currentIndex()].Format : IMAGE_FORMAT_NONE;
		m_pOptions->AlphaFormat = m_pAlphaFormat->currentIndex() >= 0
			? AlphaImageFormats[m_pAlphaFormat->currentIndex()].Format : IMAGE_FORMAT_NONE;
		m_pOptions->TextureType = static_cast<VtfTextureType>(m_pTextureType->currentIndex());

		m_pOptions->FlagClampS = m_pFlagClampS->isChecked() ? vlTrue : vlFalse;
		m_pOptions->FlagClampT = m_pFlagClampT->isChecked() ? vlTrue : vlFalse;
		m_pOptions->FlagNoLOD = m_pFlagNoLOD->isChecked() ? vlTrue : vlFalse;
		m_pOptions->FlagPointSample = m_pFlagPointSample->isChecked() ? vlTrue : vlFalse;

		m_pOptions->ResizeImage = m_pResize->isChecked() ? vlTrue : vlFalse;
		m_pOptions->ResizeMethod = static_cast<VTFResizeMethod>(m_pResizeMethod->currentData().toInt());
		m_pOptions->ResizeFilter = mipmapFilter(m_pResizeFilter);
		m_pOptions->ResizeClamp = m_pResizeClamp->isChecked() ? vlTrue : vlFalse;
		m_pOptions->ResizeClampWidth = m_pMaximumWidth->currentText().toUInt();
		m_pOptions->ResizeClampHeight = m_pMaximumHeight->currentText().toUInt();

		m_pOptions->GenerateMipmaps = m_pMipmaps->isChecked() ? vlTrue : vlFalse;
		m_pOptions->MipmapFilter = mipmapFilter(m_pMipmapFilter);

		m_pOptions->Version = m_pVersion->currentText();

		switch(m_pCompressionLevel->currentIndex())
		{
		case 1:
			m_pOptions->AuxCompressionLevel = VTF_AUX_COMPRESSION_LEVEL_DEFAULT;
			break;
		case -1:
		case 0:
			m_pOptions->AuxCompressionLevel = VTF_AUX_COMPRESSION_LEVEL_NONE;
			break;
		default:
			m_pOptions->AuxCompressionLevel = static_cast<vlShort>(m_pCompressionLevel->currentIndex() - 1);
			break;
		}
		m_pOptions->AuxCompressionMethod = m_pCompressionMethod->currentIndex() == 1
			? AUX_COMPRESSION_METHOD_ZSTD : AUX_COMPRESSION_METHOD_DEFLATE;

		m_pOptions->ComputeReflectivity = m_pReflectivity->isChecked() ? vlTrue : vlFalse;
		m_pOptions->GenerateThumbnail = m_pThumbnail->isChecked() ? vlTrue : vlFalse;
		m_pOptions->GenerateSphereMap = m_pSphereMap->isChecked() ? vlTrue : vlFalse;
		m_pOptions->StripAlpha = m_pStripAlpha->isChecked() ? vlTrue : vlFalse;
		m_pOptions->sRGB = m_pSrgb->isChecked() ? vlTrue : vlFalse;

		m_pOptions->DistanceAlpha = m_pDistanceAlpha->isChecked() ? vlTrue : vlFalse;
		m_pOptions->DistanceAlphaSpread = static_cast<vlSingle>(m_pDistanceAlphaSpread->value());
		m_pOptions->DistanceAlphaReduce = static_cast<vlUInt>(m_pDistanceAlphaReduce->currentData().toInt());
		m_pOptions->DistanceAlphaThreshold = static_cast<vlUInt>(m_pDistanceAlphaThreshold->value());

		m_pOptions->CorrectGamma = m_pGammaCorrection->isChecked() ? vlTrue : vlFalse;
		m_pOptions->GammaCorrection = static_cast<vlSingle>(m_pGammaCorrectionValue->value());

		m_pOptions->LuminanceWeightR = static_cast<vlSingle>(m_pLuminanceWeightR->value());
		m_pOptions->LuminanceWeightG = static_cast<vlSingle>(m_pLuminanceWeightG->value());
		m_pOptions->LuminanceWeightB = static_cast<vlSingle>(m_pLuminanceWeightB->value());

		m_pOptions->CreateLODControlResource = m_pCreateLODControlResource->isChecked() ? vlTrue : vlFalse;
		m_pOptions->LODControlClampU = static_cast<vlUInt>(m_pLODControlClampU->value());
		m_pOptions->LODControlClampV = static_cast<vlUInt>(m_pLODControlClampV->value());

		m_pOptions->CreateInformationResource = m_pCreateInformationResource->isChecked() ? vlTrue : vlFalse;
		m_pOptions->InformationAuthor = m_pInformationAuthor->text();
		m_pOptions->InformationContact = m_pInformationContact->text();
		m_pOptions->InformationVersion = m_pInformationVersion->text();
		m_pOptions->InformationModification = m_pInformationModification->text();
		m_pOptions->InformationDescription = m_pInformationDescription->text();
		m_pOptions->InformationComments = m_pInformationComments->text();
	}
}
