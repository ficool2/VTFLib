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

#include "BatchConvertDialog.h"

#include "VmtFileUtility.h"
#include "VtfFileUtility.h"
#include "VtfOptionsDialog.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include <vector>

namespace VTFEdit
{
	namespace
	{
		const QColor LogGreen(0, 128, 0);
		const QColor LogRed(255, 0, 0);
		const QColor LogGray(128, 128, 128);

		QStringList splitFilter(const QString &sFilter)
		{
			if(sFilter.isEmpty())
			{
				return QStringList(QStringLiteral("^"));
			}

			return sFilter.split(QLatin1Char(';'), Qt::SkipEmptyParts);
		}
	}

	BatchConvertDialog::BatchConvertDialog(VtfOptions *pOptions, BatchConvertSettings *pSettings, QWidget *pParent)
		: QDialog(pParent)
		, m_pOptions(pOptions)
		, m_pSettings(pSettings)
		, m_pOptionsDialog(nullptr)
	{
		setWindowTitle(tr("Batch Convert"));
		resize(640, 560);

		QGroupBox *pOptionsGroup = new QGroupBox(tr("Options:"), this);
		QFormLayout *pOptionsForm = new QFormLayout(pOptionsGroup);

		const auto addFolderRow = [this, pOptionsGroup, pOptionsForm](const QString &sLabel, void (BatchConvertDialog::*pSlot)())
		{
			QWidget *pRow = new QWidget(pOptionsGroup);
			QHBoxLayout *pRowLayout = new QHBoxLayout(pRow);
			pRowLayout->setContentsMargins(0, 0, 0, 0);

			QLineEdit *pEdit = new QLineEdit(pRow);
			QPushButton *pBrowse = new QPushButton(QStringLiteral("..."), pRow);
			pBrowse->setFixedWidth(32);
			connect(pBrowse, &QPushButton::clicked, this, pSlot);

			pRowLayout->addWidget(pEdit, 1);
			pRowLayout->addWidget(pBrowse);

			pOptionsForm->addRow(sLabel, pRow);

			return pEdit;
		};

		m_pInputFolder = addFolderRow(tr("Input Folder:"), &BatchConvertDialog::onBrowseInputFolder);
		m_pOutputFolder = addFolderRow(tr("Output Folder:"), &BatchConvertDialog::onBrowseOutputFolder);

		m_pToVTF = new QRadioButton(tr("To VTF"), pOptionsGroup);
		m_pToVTFFilter = new QLineEdit(pOptionsGroup);
		m_pToVTFFilter->setToolTip(tr("Wildcard filter.  Examples include *.* or *.tga or *.bmp;*.tga."));
		pOptionsForm->addRow(m_pToVTF, m_pToVTFFilter);

		QWidget *pFromRow = new QWidget(pOptionsGroup);
		QHBoxLayout *pFromLayout = new QHBoxLayout(pFromRow);
		pFromLayout->setContentsMargins(0, 0, 0, 0);
		m_pFromVTF = new QRadioButton(tr("To"), pFromRow);
		m_pFromVTFFormat = new QComboBox(pFromRow);
		m_pFromVTFFormat->addItems({ QStringLiteral("bmp"), QStringLiteral("exr"),
			QStringLiteral("hdr"), QStringLiteral("jpg"), QStringLiteral("pfm"),
			QStringLiteral("png"), QStringLiteral("tga") });
		pFromLayout->addWidget(m_pFromVTF);
		pFromLayout->addWidget(m_pFromVTFFormat);
		m_pFromVTFFilter = new QLineEdit(pOptionsGroup);
		m_pFromVTFFilter->setToolTip(tr("Wildcard filter.  Examples include *.* or *.vtf."));
		pOptionsForm->addRow(pFromRow, m_pFromVTFFilter);

		m_pRecursive = new QCheckBox(tr("&Recursive"), pOptionsGroup);
		m_pCreateVMTFiles = new QCheckBox(tr("Create &VMT Files"), pOptionsGroup);
		pOptionsForm->addRow(m_pRecursive);
		pOptionsForm->addRow(m_pCreateVMTFiles);

		QGroupBox *pProgressGroup = new QGroupBox(tr("Progress:"), this);
		QVBoxLayout *pProgressLayout = new QVBoxLayout(pProgressGroup);
		m_pProgress = new QProgressBar(pProgressGroup);
		pProgressLayout->addWidget(m_pProgress);

		QGroupBox *pLogGroup = new QGroupBox(tr("Log:"), this);
		QVBoxLayout *pLogLayout = new QVBoxLayout(pLogGroup);
		m_pLog = new QTextEdit(pLogGroup);
		m_pLog->setReadOnly(true);
		pLogLayout->addWidget(m_pLog);

		QDialogButtonBox *pButtons = new QDialogButtonBox(this);
		QPushButton *pConvert = pButtons->addButton(tr("Con&vert"), QDialogButtonBox::ActionRole);
		QPushButton *pOptionsButton = pButtons->addButton(tr("&Options"), QDialogButtonBox::ActionRole);
		QPushButton *pClose = pButtons->addButton(tr("&Close"), QDialogButtonBox::RejectRole);

		connect(pConvert, &QPushButton::clicked, this, &BatchConvertDialog::onConvert);
		connect(pOptionsButton, &QPushButton::clicked, this, &BatchConvertDialog::onOptions);
		connect(pClose, &QPushButton::clicked, this, &QDialog::reject);

		QVBoxLayout *pLayout = new QVBoxLayout(this);
		pLayout->addWidget(pOptionsGroup);
		pLayout->addWidget(pProgressGroup);
		pLayout->addWidget(pLogGroup, 1);
		pLayout->addWidget(pButtons);
	}

	int BatchConvertDialog::exec()
	{
		settingsToControls();

		const int iResult = QDialog::exec();

		controlsToSettings();

		return iResult;
	}

	void BatchConvertDialog::settingsToControls()
	{
		m_pInputFolder->setText(m_pSettings->sInputFolder);
		m_pOutputFolder->setText(m_pSettings->sOutputFolder);
		m_pToVTF->setChecked(m_pSettings->bToVTF);
		m_pFromVTF->setChecked(!m_pSettings->bToVTF);
		m_pToVTFFilter->setText(m_pSettings->sToVTFFilter);
		m_pFromVTFFilter->setText(m_pSettings->sFromVTFFilter);
		m_pRecursive->setChecked(m_pSettings->bRecurse);
		m_pCreateVMTFiles->setChecked(m_pSettings->bCreateVMTFiles);

		const int iFormat = m_pFromVTFFormat->findText(m_pSettings->sFromVTFFormat);
		m_pFromVTFFormat->setCurrentIndex(iFormat >= 0 ? iFormat : 6);	// tga
	}

	void BatchConvertDialog::controlsToSettings()
	{
		m_pSettings->sInputFolder = m_pInputFolder->text();
		m_pSettings->sOutputFolder = m_pOutputFolder->text();
		m_pSettings->bToVTF = m_pToVTF->isChecked();
		m_pSettings->sToVTFFilter = m_pToVTFFilter->text();
		m_pSettings->sFromVTFFilter = m_pFromVTFFilter->text();
		m_pSettings->sFromVTFFormat = m_pFromVTFFormat->currentText();
		m_pSettings->bRecurse = m_pRecursive->isChecked();
		m_pSettings->bCreateVMTFiles = m_pCreateVMTFiles->isChecked();
	}

	void BatchConvertDialog::onBrowseInputFolder()
	{
		const QString sFolder = QFileDialog::getExistingDirectory(this, tr("Input Folder"),
			m_pInputFolder->text());

		if(!sFolder.isEmpty())
		{
			m_pInputFolder->setText(QDir::toNativeSeparators(sFolder));
		}
	}

	void BatchConvertDialog::onBrowseOutputFolder()
	{
		const QString sFolder = QFileDialog::getExistingDirectory(this, tr("Output Folder"),
			m_pOutputFolder->text());

		if(!sFolder.isEmpty())
		{
			m_pOutputFolder->setText(QDir::toNativeSeparators(sFolder));
		}
	}

	void BatchConvertDialog::onOptions()
	{
		if(m_pOptionsDialog == nullptr)
		{
			m_pOptionsDialog = new VtfOptionsDialog(m_pOptions, this);
		}

		m_pOptionsDialog->exec();
	}

	void BatchConvertDialog::onConvert()
	{
		QApplication::setOverrideCursor(Qt::WaitCursor);

		const QStringList sFilters = splitFilter(m_pToVTF->isChecked()
			? m_pToVTFFilter->text() : m_pFromVTFFilter->text());

		const QString sInputFolder = m_pInputFolder->text();
		const QString sOutputFolder = m_pOutputFolder->text().isEmpty()
			? sInputFolder : m_pOutputFolder->text();

		m_pProgress->setValue(0);
		m_pProgress->setMaximum(countFiles(sInputFolder, sFilters, m_pRecursive->isChecked()));
		m_pLog->clear();

		SVTFCreateOptions VTFCreateOptions = VtfFileUtility::GetCreateOptions(*m_pOptions);

		convertFolder(sInputFolder, sOutputFolder, sFilters, m_pRecursive->isChecked(), VTFCreateOptions);

		QApplication::restoreOverrideCursor();
	}

	int BatchConvertDialog::countFiles(const QString &sFolder, const QStringList &sFilters, bool bRecursive) const
	{
		QDir Folder(sFolder);
		int iCount = 0;

		if(bRecursive)
		{
			const QStringList sSubFolders = Folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			for(const QString &sSubFolder : sSubFolders)
			{
				iCount += countFiles(Folder.filePath(sSubFolder), sFilters, bRecursive);
			}
		}

		iCount += Folder.entryList(sFilters, QDir::Files).count();

		return iCount;
	}

	void BatchConvertDialog::convertFolder(const QString &sInputFolder, const QString &sOutputFolder,
		const QStringList &sFilters, bool bRecursive, SVTFCreateOptions &VTFCreateOptions)
	{
		VTFLib::CVTFFile VTFFile;

		QDir InputDir(sInputFolder);

		log(tr("Entering %1...").arg(sInputFolder), LogGreen);

		if(bRecursive)
		{
			const QStringList sSubFolders = InputDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			for(const QString &sSubFolder : sSubFolders)
			{
				convertFolder(InputDir.filePath(sSubFolder),
					QDir(sOutputFolder).filePath(sSubFolder), sFilters, bRecursive, VTFCreateOptions);
			}
		}

		const QFileInfoList Files = InputDir.entryInfoList(sFilters, QDir::Files);

		for(const QFileInfo &File : Files)
		{
			const bool bIsVTF = File.suffix().compare(QLatin1String("vtf"), Qt::CaseInsensitive) == 0;
			const QString sName = File.fileName();
			const QByteArray Path = QDir::toNativeSeparators(File.absoluteFilePath()).toLocal8Bit();

			log(tr("Processing %1...").arg(sName), LogGray);

			if(m_pToVTF->isChecked() && !bIsVTF)
			{
				const QString sVTFFile = QDir::toNativeSeparators(
					QDir(sOutputFolder).filePath(File.completeBaseName() + QStringLiteral(".vtf")));
				const QString sVMTFile = QDir::toNativeSeparators(
					QDir(sOutputFolder).filePath(File.completeBaseName() + QStringLiteral(".vmt")));

				if(ilLoadImage(Path.constData()))
				{
					bool bHasAlpha = false;
					bool bError = false;

					const ILuint uiImage = static_cast<ILuint>(ilGetInteger(IL_CUR_IMAGE));
					const vlUInt uiImages = static_cast<vlUInt>(ilGetInteger(IL_NUM_IMAGES)) + 1;
					vlUInt uiWidth = 0, uiHeight = 0;

					const bool bFloat = VtfFileUtility::IsFloatImage();
					const vlUInt uiPixelSize = bFloat ? 4 * static_cast<vlUInt>(sizeof(vlSingle)) : 4;

					std::vector<vlByte *> vImageData;

					for(vlUInt k = 0; k < uiImages; k++)
					{
						ilBindImage(uiImage);
						ilActiveImage(static_cast<ILuint>(k));

						if(!ilConvertImage(IL_RGBA, bFloat ? IL_FLOAT : IL_UNSIGNED_BYTE))
						{
							log(tr("Error converting %1.").arg(sName), LogRed);
							bError = true;
							break;
						}

						if(vImageData.empty())
						{
							uiWidth = static_cast<vlUInt>(ilGetInteger(IL_IMAGE_WIDTH));
							uiHeight = static_cast<vlUInt>(ilGetInteger(IL_IMAGE_HEIGHT));
						}
						else if(uiWidth != static_cast<vlUInt>(ilGetInteger(IL_IMAGE_WIDTH))
							|| uiHeight != static_cast<vlUInt>(ilGetInteger(IL_IMAGE_HEIGHT)))
						{
							log(tr("Error converting %1. All frames must be the same size.").arg(sName), LogRed);
							bError = true;
							break;
						}

						const size_t uiFrameSize = static_cast<size_t>( uiWidth ) * uiHeight * uiPixelSize;

						vlByte *lpFrameData = new vlByte[uiFrameSize];
						memcpy(lpFrameData, ilGetData(), uiFrameSize);
						vImageData.push_back(lpFrameData);

						if(!m_pOptions->StripAlpha)
						{
							bHasAlpha = bHasAlpha || (bFloat
								? VtfFileUtility::HasAlphaDataRGBA32F(reinterpret_cast<vlSingle *>(lpFrameData), uiWidth, uiHeight)
								: VtfFileUtility::HasAlphaData(lpFrameData, uiWidth, uiHeight));
						}
					}

					// Leave the base image bound for the next file.
					ilBindImage(uiImage);

					if(!bError && !vImageData.empty() && m_pOptions->DistanceAlpha && !bFloat)
					{
						VtfFileUtility::ApplyDistanceAlpha(vImageData, uiWidth, uiHeight, *m_pOptions);
						bHasAlpha = true;
					}

					if(!bError && !vImageData.empty())
					{
						VTFCreateOptions.ImageFormat = bHasAlpha ? m_pOptions->AlphaFormat : m_pOptions->NormalFormat;

						const bool bCreated = VTFFile.Create(uiWidth, uiHeight, static_cast<vlUInt>(vImageData.size()),
							1, 1, &vImageData[0], VTFCreateOptions,
							bFloat ? IMAGE_FORMAT_RGBA32323232F : IMAGE_FORMAT_RGBA8888) != vlFalse;
						if(bCreated)
						{
							VtfFileUtility::ApplyFlags(*m_pOptions, &VTFFile);
						}

						if(bCreated && VtfFileUtility::CreateResources(*m_pOptions, &VTFFile))
						{
							QDir().mkpath(sOutputFolder);

							if(VTFFile.Save(sVTFFile.toLocal8Bit().constData()))
							{
								log(tr("Wrote %1.").arg(sVTFFile), LogGreen);

								if(m_pCreateVMTFiles->isChecked()
									&& VmtFileUtility::CreateDefaultMaterial(sVTFFile,
										QStringLiteral("LightmappedGeneric"), bHasAlpha))
								{
									log(tr("Wrote %1.").arg(sVMTFile), LogGreen);
								}
							}
							else
							{
								log(tr("Error writing %1.%2").arg(sName,
									QString::fromLatin1(vlGetLastError()).replace(QLatin1Char('\n'), QLatin1Char(' '))), LogRed);
							}
						}
						else
						{
							log(tr("Error creating %1.%2").arg(sName,
								QString::fromLatin1(vlGetLastError()).replace(QLatin1Char('\n'), QLatin1Char(' '))), LogRed);
						}
					}

					for(vlByte *lpFrameData : vImageData)
					{
						delete[] lpFrameData;
					}
				}
				else
				{
					log(tr("Error loading %1.").arg(sName), LogRed);
				}

				log(tr("%1 processed.").arg(sName), LogGray);
			}
			else if(bIsVTF)
			{
				if(VTFFile.Load(Path.constData()))
				{
					const vlUInt uiWidth = VTFFile.GetWidth();
					const vlUInt uiHeight = VTFFile.GetHeight();

					const bool bFloat = VtfFileUtility::IsFloatImageFileName(
						QLatin1Char('.') + m_pFromVTFFormat->currentText());
					const VTFImageFormat DestFormat = bFloat ? IMAGE_FORMAT_RGBA32323232F : IMAGE_FORMAT_RGBA8888;

					std::vector<vlByte> ImageData(
						VTFFile.ComputeImageSize(uiWidth, uiHeight, 1, DestFormat));

					const vlUInt uiFrameCount = VTFFile.GetFrameCount();
					const vlUInt uiFaceCount = VTFFile.GetFaceCount();
					const vlUInt uiSliceCount = VTFFile.GetDepth();

					const bool bSingleImage = uiFrameCount <= 1 && uiFaceCount <= 1 && uiSliceCount <= 1;

					for(vlUInt uiFrame = 0; uiFrame < uiFrameCount; uiFrame++)
					{
						for(vlUInt uiFace = 0; uiFace < uiFaceCount; uiFace++)
						{
							for(vlUInt uiSlice = 0; uiSlice < uiSliceCount; uiSlice++)
							{
								const QString sSuffix = bSingleImage ? QString()
									: QStringLiteral("_%1_%2_%3")
										.arg(uiFrame, 2, 10, QLatin1Char('0'))
										.arg(uiFace, 2, 10, QLatin1Char('0'))
										.arg(uiSlice, 2, 10, QLatin1Char('0'));

								const QString sOtherFile = QDir::toNativeSeparators(QDir(sOutputFolder).filePath(
									File.completeBaseName() + sSuffix
										+ QLatin1Char('.') + m_pFromVTFFormat->currentText()));

								if(!VTFFile.Convert(VTFFile.GetData(uiFrame, uiFace, uiSlice, 0),
									ImageData.data(), uiWidth, uiHeight, VTFFile.GetDecodeFormat(), DestFormat))
								{
									log(tr("Error converting %1.%2").arg(sName,
										QString::fromLatin1(vlGetLastError()).replace(QLatin1Char('\n'), QLatin1Char(' '))), LogRed);
									continue;
								}

								// DevIL likes image data upside down...
								if(bFloat)
								{
									VTFFile.FlipImageRGBA32F(
										reinterpret_cast<vlSingle *>(ImageData.data()), uiWidth, uiHeight);
								}
								else
								{
									VTFFile.FlipImage(ImageData.data(), uiWidth, uiHeight);
								}

								if(!ilTexImage(uiWidth, uiHeight, 1, 4, IL_RGBA,
									bFloat ? IL_FLOAT : IL_UNSIGNED_BYTE, ImageData.data()))
								{
									log(tr("Error creating %1.").arg(sName), LogRed);
									continue;
								}

								QDir().mkpath(sOutputFolder);

								if(ilSaveImage(sOtherFile.toLocal8Bit().constData()))
								{
									log(tr("Wrote %1.").arg(sOtherFile), LogGreen);
								}
								else
								{
									log(tr("Error writing %1.").arg(sName), LogRed);
								}
							}
						}
					}
				}
				else
				{
					log(tr("Error loading %1.%2").arg(sName,
						QString::fromLatin1(vlGetLastError()).replace(QLatin1Char('\n'), QLatin1Char(' '))), LogRed);
				}

				log(tr("%1 processed.").arg(sName), LogGray);
			}
			else
			{
				log(tr("%1 skipped.").arg(sName), LogGray);
			}

			m_pProgress->setValue(m_pProgress->value() + 1);

			// pump events.
			QApplication::processEvents();
		}

		log(tr("Exiting %1.").arg(sInputFolder), LogGreen);
	}

	void BatchConvertDialog::log(const QString &sMessage, const QColor &Color)
	{
		m_pLog->setTextColor(Color);
		m_pLog->append(sMessage);
	}
}
