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

#include "VmtCreateDialog.h"

#include "FileDialogHistory.h"
#include "VmtFileUtility.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

#include <utility>

namespace VTFEdit
{
	namespace
	{
		const char *const ShaderNames[] =
		{
			"Cable", 
			"Decal",
			"DecalModulate",
			"LightmappedGeneric", 
			"Modulate",
			"MonitorScreen",
			"Predator",
			"Refract", 
			"ShatteredGlass",
			"Sprite",
			"UnlitGeneric"
			, "VertexlitGeneric",
			"Water"
		};

		const char *const SurfaceNames[] =
		{
			"AlienFlesh", 
			"ArmorFlesh", 
			"BloodyFlesh", 
			"Boulder", 
			"Brick", 
			"Chain", 
			"Chainlink",
			"Computer", 
			"Concrete", 
			"Concrete_Block",
			"Default",
			"Dirt",
			"Flesh",
			"Glass", 
			"Grass",
			"Gravel", 
			"Ice",
			"Ladder",
			"Metal_Box",
			"Metal", 
			"MetalGrate",
			"MetalPanel",
			"MetalVent",
			"MudSlipperySlime",
			"Porcelain",
			"Quicksand", 
			"Rock",
			"Slime", 
			"SlipperyMetal",
			"Snow",
			"SolidMetal", 
			"Tile",
			"Wade", 
			"Water",
			"Watermelon", 
			"Wood_Box",
			"Wood_Crate",
			"Wood_Furniture",
			"Wood_Panel",
			"Wood_Plank", 
			"Wood_Solid",
			"WoodWood_Lowdensity"
		};

		struct { const char *pLabel; const char *pParameter; } TextureFields[] =
		{
			{ "Base Texture 1:", "$basetexture" },
			{ "Bump Map 1:", "$bumpmap" },
			{ "Base Texture 2:", "$basetexture2" },
			{ "Bump Map 2:", "$bumpmap2" },
			{ "Enviroment:", "$envmap" },
			{ "Enviroment Mask:", "$envmapmask" },
			{ "Detail Texture:", "$detail" },
			{ "Tool Texture:", "%tooltexture" },
			{ "Normal Map:", "$normalmap" },
			{ "DuDv Map:", "$dudvmap" },
		};

		struct { const char *pLabel; const char *pParameter; } OptionFields[] =
		{
			{ "Additive", "$additive" },
			{ "Alpha Test", "$alphatest" },
			{ "Enviroment Map Contrast", "$envmapcontrast" },
			{ "Enviroment Map Saturation", "$envmapsaturation" },
			{ "No Cull", "$nocull" },
			{ "No Decal", "$nodecal" },
			{ "No LOD", "$nolod" },
			{ "Translucent", "$translucent" },
			{ "Vertex Alpha", "$vertexalpha" },
			{ "Vertex Color", "$vertexcolor" },
		};
	}

	VmtCreateDialog::VmtCreateDialog(QWidget *pParent)
		: QDialog(pParent)
	{
		setWindowTitle(tr("Create VMT File"));

		QTabWidget *pTabs = new QTabWidget(this);
		pTabs->addTab(createTexturesTab(), tr("Textures"));
		pTabs->addTab(createOptionsTab(), tr("Options"));

		QDialogButtonBox *pButtons = new QDialogButtonBox(this);
		m_pCreateButton = pButtons->addButton(tr("C&reate"), QDialogButtonBox::ActionRole);
		QPushButton *pClear = pButtons->addButton(tr("C&lear"), QDialogButtonBox::ResetRole);
		QPushButton *pClose = pButtons->addButton(tr("&Close"), QDialogButtonBox::AcceptRole);

		connect(m_pCreateButton, &QPushButton::clicked, this, &VmtCreateDialog::onCreate);
		connect(pClear, &QPushButton::clicked, this, &VmtCreateDialog::onClear);
		connect(pClose, &QPushButton::clicked, this, &QDialog::accept);

		QVBoxLayout *pLayout = new QVBoxLayout(this);
		pLayout->addWidget(pTabs);
		pLayout->addWidget(pButtons);

		connect(m_pShader, &QComboBox::currentTextChanged, this, [this](const QString &sText)
		{
			m_pCreateButton->setEnabled(!sText.isEmpty());
		});

		onClear();
	}

	QLineEdit *VmtCreateDialog::addTextureRow(QFormLayout *pForm, const QString &sLabel, const QString &sToolTip)
	{
		QWidget *pRow = new QWidget(pForm->parentWidget());
		QHBoxLayout *pRowLayout = new QHBoxLayout(pRow);
		pRowLayout->setContentsMargins(0, 0, 0, 0);

		QLineEdit *pEdit = new QLineEdit(pRow);
		pEdit->setToolTip(sToolTip);

		QPushButton *pBrowse = new QPushButton(QStringLiteral("..."), pRow);
		pBrowse->setFixedWidth(32);
		pBrowse->setProperty("targetField", sToolTip);
		connect(pBrowse, &QPushButton::clicked, this, &VmtCreateDialog::onBrowseTexture);

		pRowLayout->addWidget(pEdit, 1);
		pRowLayout->addWidget(pBrowse);

		pForm->addRow(sLabel, pRow);

		return pEdit;
	}

	QWidget *VmtCreateDialog::createTexturesTab()
	{
		QWidget *pTab = new QWidget(this);
		QVBoxLayout *pLayout = new QVBoxLayout(pTab);

		QGroupBox *pTextures = new QGroupBox(tr("Textures:"), pTab);
		QFormLayout *pForm = new QFormLayout(pTextures);

		for(const auto &Field : TextureFields)
		{
			const QString sParameter = QString::fromLatin1(Field.pParameter);

			m_Textures.insert(sParameter,
				addTextureRow(pForm, tr(Field.pLabel), sParameter));
			m_TextureOrder.append(sParameter);
		}

		pLayout->addWidget(pTextures);
		pLayout->addStretch();

		return pTab;
	}

	QWidget *VmtCreateDialog::createOptionsTab()
	{
		QWidget *pTab = new QWidget(this);
		QVBoxLayout *pLayout = new QVBoxLayout(pTab);

		QGroupBox *pGeneral = new QGroupBox(tr("Options:"), pTab);
		QFormLayout *pForm = new QFormLayout(pGeneral);

		m_pShader = new QComboBox(pGeneral);
		m_pShader->setEditable(true);
		for(const char *pName : ShaderNames)
		{
			m_pShader->addItem(QString::fromLatin1(pName));
		}
		pForm->addRow(tr("Shader:"), m_pShader);

		m_pSurface1 = new QComboBox(pGeneral);
		m_pSurface1->setEditable(true);
		m_pSurface2 = new QComboBox(pGeneral);
		m_pSurface2->setEditable(true);
		for(const char *pName : SurfaceNames)
		{
			m_pSurface1->addItem(QString::fromLatin1(pName));
			m_pSurface2->addItem(QString::fromLatin1(pName));
		}
		m_pSurface1->setToolTip(QStringLiteral("$surfaceprop"));
		m_pSurface2->setToolTip(QStringLiteral("$surfaceprop2"));
		pForm->addRow(tr("Surface 1:"), m_pSurface1);
		pForm->addRow(tr("Surface 2:"), m_pSurface2);

		m_pKeywords = new QLineEdit(pGeneral);
		m_pKeywords->setToolTip(QStringLiteral("%keywords"));
		pForm->addRow(tr("Keywords:"), m_pKeywords);

		for(const auto &Field : OptionFields)
		{
			const QString sParameter = QString::fromLatin1(Field.pParameter);

			QCheckBox *pCheck = new QCheckBox(tr(Field.pLabel), pGeneral);
			pCheck->setToolTip(sParameter);
			pForm->addRow(pCheck);

			m_Options.insert(sParameter, pCheck);
			m_OptionOrder.append(sParameter);
		}

		pLayout->addWidget(pGeneral);
		pLayout->addStretch();

		return pTab;
	}

	void VmtCreateDialog::setFromTexture(const QString &sFileName, VTFLib::CVTFFile &VTFFile)
	{
		m_Textures.value(QStringLiteral("$basetexture"))
			->setText(VmtFileUtility::GetTexturePathFromSystemPath(sFileName));

		m_Options.value(QStringLiteral("$alphatest"))
			->setChecked(VTFFile.GetFlag(TEXTUREFLAGS_ONEBITALPHA) != 0);
		m_Options.value(QStringLiteral("$translucent"))
			->setChecked(VTFFile.GetFlag(TEXTUREFLAGS_EIGHTBITALPHA) != 0);
	}

	void VmtCreateDialog::onBrowseTexture()
	{
		QPushButton *pButton = qobject_cast<QPushButton *>(sender());
		if(pButton == nullptr)
		{
			return;
		}

		QLineEdit *pEdit = m_Textures.value(pButton->property("targetField").toString(), nullptr);
		if(pEdit == nullptr)
		{
			return;
		}

		const QString sFileName = QFileDialog::getOpenFileName(this, tr("Open VTF File"),
			FileDialogHistory::s_sFileDirectory, tr("VTF Files (*.vtf)"));

		if(!sFileName.isEmpty())
		{
			FileDialogHistory::remember(FileDialogHistory::s_sFileDirectory, sFileName);

			pEdit->setText(VmtFileUtility::GetTexturePathFromSystemPath(sFileName));
		}
	}

	void VmtCreateDialog::onClear()
	{
		for(QLineEdit *pEdit : std::as_const(m_Textures))
		{
			pEdit->clear();
		}

		for(QCheckBox *pCheck : std::as_const(m_Options))
		{
			pCheck->setChecked(false);
		}

		m_pShader->setCurrentText(QStringLiteral("LightmappedGeneric"));
		m_pSurface1->setCurrentText(QString());
		m_pSurface2->setCurrentText(QString());
		m_pKeywords->clear();
	}

	void VmtCreateDialog::onCreate()
	{
		// Default the file name to the base texture's leaf name
		QString sSuggested = m_Textures.value(QStringLiteral("$basetexture"))->text();
		const int iSlash = sSuggested.lastIndexOf(QLatin1Char('/'));
		if(iSlash != -1)
		{
			sSuggested = sSuggested.mid(iSlash + 1);
		}

		const QString sFileName = QFileDialog::getSaveFileName(this, tr("Save VMT File"),
			FileDialogHistory::path(FileDialogHistory::s_sFileDirectory, sSuggested),
			tr("VMT Files (*.vmt)"));

		if(sFileName.isEmpty())
		{
			return;
		}

		FileDialogHistory::remember(FileDialogHistory::s_sFileDirectory, sFileName);

		VTFLib::CVMTFile VMTFile;
		VMTFile.Create(m_pShader->currentText().toLocal8Bit().constData());

		const auto addString = [&VMTFile](const QString &sName, const QString &sValue)
		{
			if(sValue.isEmpty())
			{
				return;
			}

			VMTFile.GetRoot()->AddStringNode(sName.toLocal8Bit().constData(),
				sValue.toLocal8Bit().constData());
		};

		for(const QString &sParameter : std::as_const(m_TextureOrder))
		{
			addString(sParameter, m_Textures.value(sParameter)->text());
		}

		addString(QStringLiteral("$surfaceprop"), m_pSurface1->currentText());
		addString(QStringLiteral("$surfaceprop2"), m_pSurface2->currentText());
		addString(QStringLiteral("%keywords"), m_pKeywords->text());

		// Options are only written when they differ from the default
		for(const QString &sParameter : std::as_const(m_OptionOrder))
		{
			if(m_Options.value(sParameter)->isChecked())
			{
				VMTFile.GetRoot()->AddIntegerNode(sParameter.toLocal8Bit().constData(), 1);
			}
		}

		VMTFile.Save(sFileName.toLocal8Bit().constData());
	}
}
