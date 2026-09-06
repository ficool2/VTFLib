/*
 * VTFEdit
 * Copyright (C) 2005-2010 Neil Jedrzejewski & Ryan Gregg
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

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "stdafx.h"

namespace VTFEdit
{
	// Formats without an alpha channel.
	static const VTFImageFormat NormalImageFormats[] =
	{
		IMAGE_FORMAT_RGB888, IMAGE_FORMAT_BGR888, IMAGE_FORMAT_RGB565, IMAGE_FORMAT_I8, IMAGE_FORMAT_P8,
		IMAGE_FORMAT_RGB888_BLUESCREEN, IMAGE_FORMAT_BGR888_BLUESCREEN, IMAGE_FORMAT_DXT1, IMAGE_FORMAT_BGRX8888,
		IMAGE_FORMAT_BGR565, IMAGE_FORMAT_BGRX5551, IMAGE_FORMAT_UV88, IMAGE_FORMAT_UVLX8888, IMAGE_FORMAT_R8,
		IMAGE_FORMAT_BC6H, IMAGE_FORMAT_BC6H_UNSIGNED
	};

	// Formats with an alpha channel.
	static const VTFImageFormat AlphaImageFormats[] =
	{
		IMAGE_FORMAT_RGBA8888, IMAGE_FORMAT_ABGR8888, IMAGE_FORMAT_IA88, IMAGE_FORMAT_A8, IMAGE_FORMAT_ARGB8888,
		IMAGE_FORMAT_BGRA8888, IMAGE_FORMAT_DXT3, IMAGE_FORMAT_DXT5, IMAGE_FORMAT_BGRA4444,
		IMAGE_FORMAT_DXT1_ONEBITALPHA, IMAGE_FORMAT_BGRA5551, IMAGE_FORMAT_UVWQ8888, IMAGE_FORMAT_RGBA16161616F,
		IMAGE_FORMAT_RGBA16161616, IMAGE_FORMAT_BC7
	};

	public ref class CVTFOptions : public System::Windows::Forms::Form
	{
	public:
		CVTFOptions(void)
		{
			this->InitializeComponent();

			this->btnReset_Click(this, System::EventArgs::Empty);
		}

	private: System::Windows::Forms::Button^ btnOK;
	private: System::Windows::Forms::Button^ btnCancel;
	private: System::Windows::Forms::Label^ lblFormat;
	private: System::Windows::Forms::ComboBox^ cboFormat;
	private: System::Windows::Forms::CheckBox^ chkMipmaps;
	private: System::Windows::Forms::ComboBox^ cboMipmapFilter;
	private: System::Windows::Forms::Label^ lblMipmapFilter;
	private: System::Windows::Forms::CheckBox^ chkThumbnail;
	private: System::Windows::Forms::ComboBox^ cboTextureType;
	private: System::Windows::Forms::Label^ lblTextureType;
	private: System::Windows::Forms::CheckBox^ chkSphereMap;
	private: System::Windows::Forms::CheckBox^ chkReflectivity;







	private: System::Windows::Forms::ContextMenu^ mnuReset;
	private: System::Windows::Forms::MenuItem^ btnReset;
	private: System::Windows::Forms::GroupBox^ grpGeneral;
	private: System::Windows::Forms::GroupBox^ grpMipmaps;

	private: System::Windows::Forms::GroupBox^ grpMiscellaneous;


	private: System::Windows::Forms::GroupBox^ grpGammaCorrection;
	private: System::Windows::Forms::CheckBox^ chkGammaCorrection;
	private: System::Windows::Forms::NumericUpDown^ numGammaCorrection;
	private: System::Windows::Forms::Label^ lblGammaCorrection;
	private: System::Windows::Forms::GroupBox^ grpResize;


	private: System::Windows::Forms::ComboBox^ cboResizeFilter;
	private: System::Windows::Forms::Label^ lblResizeFilter;
	private: System::Windows::Forms::CheckBox^ chkResize;
	private: System::Windows::Forms::ComboBox^ cboResizeMethod;
	private: System::Windows::Forms::Label^ lblResizeMethod;
	private: System::Windows::Forms::CheckBox^ chkResizeClamp;
	private: System::Windows::Forms::ToolTip^ tipMain;


	private: System::Windows::Forms::ComboBox^ cboMaximumWidth;
	private: System::Windows::Forms::Label^ lblMaximumWidth;
	private: System::Windows::Forms::ComboBox^ cboMaximumHeight;
	private: System::Windows::Forms::Label^ lblMaximumHeight;

	private: System::Windows::Forms::ComboBox^ cboAlphaFormat;
	private: System::Windows::Forms::Label^ lblAlphaFormat;
	private: System::Windows::Forms::GroupBox^ grpGeneralOptions;
	private: System::Windows::Forms::TabControl^ tabOptions;
	private: System::Windows::Forms::TabPage^ tabGeneral;
	private: System::Windows::Forms::TabPage^ tabAdvanced;
	private: System::Windows::Forms::GroupBox^ grpAdvancedOptions;















	private: System::Windows::Forms::GroupBox^ grpLuminanceWeights;
	private: System::Windows::Forms::Label^ lblLuminanceWeightsB;
	private: System::Windows::Forms::NumericUpDown^ numLuminanceWeightsR;
	private: System::Windows::Forms::Label^ lblLuminanceWeightsR;
	private: System::Windows::Forms::NumericUpDown^ numLuminanceWeightsG;
	private: System::Windows::Forms::NumericUpDown^ numLuminanceWeightsB;
	private: System::Windows::Forms::GroupBox^ grpVersion;
	private: System::Windows::Forms::ComboBox^ cboVersion;
	private: System::Windows::Forms::Label^ lblVersion;
	private: System::Windows::Forms::ComboBox^ cboCompressionLevel;
	private: System::Windows::Forms::Label^ lblCompressionLevel;
	private: System::Windows::Forms::ComboBox^ cboCompressionMethod;
	private: System::Windows::Forms::Label^ lblCompressionMethod;
	private: System::Windows::Forms::TabPage^ tabResources;
	private: System::Windows::Forms::GroupBox^ grpResourceOptions;
	private: System::Windows::Forms::GroupBox^ grpLODControlResource;
	private: System::Windows::Forms::CheckBox^ chkCreateLODControlResource;
	private: System::Windows::Forms::Label^ lblLuminanceWeightsG;
	private: System::Windows::Forms::NumericUpDown^ numLODControlClampV;
	private: System::Windows::Forms::Label^ lblLODControlClampV;
	private: System::Windows::Forms::NumericUpDown^ numLODControlClampU;
	private: System::Windows::Forms::Label^ lblLODControlClampU;
	private: System::Windows::Forms::GroupBox^ grpInformationResource;
	private: System::Windows::Forms::Label^ lblInformationContact;
	private: System::Windows::Forms::Label^ lblInformationAuthor;
	private: System::Windows::Forms::CheckBox^ chkCreateInformationResource;
	private: System::Windows::Forms::TextBox^ txtInformationContact;
	private: System::Windows::Forms::TextBox^ txtInformationAuthor;
	private: System::Windows::Forms::TextBox^ txtInformationComments;
	private: System::Windows::Forms::TextBox^ txtInformationDescription;
	private: System::Windows::Forms::Label^ lblInformationComments;
	private: System::Windows::Forms::Label^ lblInformationDescription;
	private: System::Windows::Forms::TextBox^ txtInformationModification;
	private: System::Windows::Forms::TextBox^ txtInformationVersion;
	private: System::Windows::Forms::Label^ lblInformationModification;
	private: System::Windows::Forms::Label^ lblInformationVersion;
private: System::Windows::Forms::CheckBox^ chkSrgb;
private: System::Windows::Forms::CheckBox^ chkStripAlpha;

	private: System::ComponentModel::IContainer^ components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(CVTFOptions::typeid));
			this->grpMiscellaneous = (gcnew System::Windows::Forms::GroupBox());
			this->mnuReset = (gcnew System::Windows::Forms::ContextMenu());
			this->btnReset = (gcnew System::Windows::Forms::MenuItem());
			this->chkReflectivity = (gcnew System::Windows::Forms::CheckBox());
			this->chkThumbnail = (gcnew System::Windows::Forms::CheckBox());
			this->chkSphereMap = (gcnew System::Windows::Forms::CheckBox());
			this->cboTextureType = (gcnew System::Windows::Forms::ComboBox());
			this->lblTextureType = (gcnew System::Windows::Forms::Label());
			this->cboMipmapFilter = (gcnew System::Windows::Forms::ComboBox());
			this->lblMipmapFilter = (gcnew System::Windows::Forms::Label());
			this->chkMipmaps = (gcnew System::Windows::Forms::CheckBox());
			this->cboFormat = (gcnew System::Windows::Forms::ComboBox());
			this->lblFormat = (gcnew System::Windows::Forms::Label());
			this->btnOK = (gcnew System::Windows::Forms::Button());
			this->btnCancel = (gcnew System::Windows::Forms::Button());
			this->grpGeneral = (gcnew System::Windows::Forms::GroupBox());
			this->chkSrgb = (gcnew System::Windows::Forms::CheckBox());
			this->chkStripAlpha = (gcnew System::Windows::Forms::CheckBox());
			this->cboAlphaFormat = (gcnew System::Windows::Forms::ComboBox());
			this->lblAlphaFormat = (gcnew System::Windows::Forms::Label());
			this->grpMipmaps = (gcnew System::Windows::Forms::GroupBox());
			this->grpGammaCorrection = (gcnew System::Windows::Forms::GroupBox());
			this->numGammaCorrection = (gcnew System::Windows::Forms::NumericUpDown());
			this->lblGammaCorrection = (gcnew System::Windows::Forms::Label());
			this->chkGammaCorrection = (gcnew System::Windows::Forms::CheckBox());
			this->grpGeneralOptions = (gcnew System::Windows::Forms::GroupBox());
			this->grpResize = (gcnew System::Windows::Forms::GroupBox());
			this->cboMaximumHeight = (gcnew System::Windows::Forms::ComboBox());
			this->lblMaximumHeight = (gcnew System::Windows::Forms::Label());
			this->cboMaximumWidth = (gcnew System::Windows::Forms::ComboBox());
			this->lblMaximumWidth = (gcnew System::Windows::Forms::Label());
			this->chkResizeClamp = (gcnew System::Windows::Forms::CheckBox());
			this->cboResizeMethod = (gcnew System::Windows::Forms::ComboBox());
			this->lblResizeMethod = (gcnew System::Windows::Forms::Label());
			this->cboResizeFilter = (gcnew System::Windows::Forms::ComboBox());
			this->lblResizeFilter = (gcnew System::Windows::Forms::Label());
			this->chkResize = (gcnew System::Windows::Forms::CheckBox());
			this->tipMain = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->cboVersion = (gcnew System::Windows::Forms::ComboBox());
			this->chkCreateLODControlResource = (gcnew System::Windows::Forms::CheckBox());
			this->chkCreateInformationResource = (gcnew System::Windows::Forms::CheckBox());
			this->numLuminanceWeightsR = (gcnew System::Windows::Forms::NumericUpDown());
			this->numLuminanceWeightsG = (gcnew System::Windows::Forms::NumericUpDown());
			this->numLuminanceWeightsB = (gcnew System::Windows::Forms::NumericUpDown());
			this->tabOptions = (gcnew System::Windows::Forms::TabControl());
			this->tabGeneral = (gcnew System::Windows::Forms::TabPage());
			this->tabAdvanced = (gcnew System::Windows::Forms::TabPage());
			this->grpAdvancedOptions = (gcnew System::Windows::Forms::GroupBox());
			this->grpVersion = (gcnew System::Windows::Forms::GroupBox());
			this->lblVersion = (gcnew System::Windows::Forms::Label());
			this->cboCompressionLevel = (gcnew System::Windows::Forms::ComboBox());
			this->lblCompressionLevel = (gcnew System::Windows::Forms::Label());
			this->cboCompressionMethod = (gcnew System::Windows::Forms::ComboBox());
			this->lblCompressionMethod = (gcnew System::Windows::Forms::Label());
			this->grpLuminanceWeights = (gcnew System::Windows::Forms::GroupBox());
			this->lblLuminanceWeightsB = (gcnew System::Windows::Forms::Label());
			this->lblLuminanceWeightsG = (gcnew System::Windows::Forms::Label());
			this->lblLuminanceWeightsR = (gcnew System::Windows::Forms::Label());
			this->tabResources = (gcnew System::Windows::Forms::TabPage());
			this->grpResourceOptions = (gcnew System::Windows::Forms::GroupBox());
			this->grpInformationResource = (gcnew System::Windows::Forms::GroupBox());
			this->txtInformationComments = (gcnew System::Windows::Forms::TextBox());
			this->txtInformationDescription = (gcnew System::Windows::Forms::TextBox());
			this->lblInformationComments = (gcnew System::Windows::Forms::Label());
			this->lblInformationDescription = (gcnew System::Windows::Forms::Label());
			this->txtInformationModification = (gcnew System::Windows::Forms::TextBox());
			this->txtInformationVersion = (gcnew System::Windows::Forms::TextBox());
			this->lblInformationModification = (gcnew System::Windows::Forms::Label());
			this->lblInformationVersion = (gcnew System::Windows::Forms::Label());
			this->txtInformationContact = (gcnew System::Windows::Forms::TextBox());
			this->txtInformationAuthor = (gcnew System::Windows::Forms::TextBox());
			this->lblInformationContact = (gcnew System::Windows::Forms::Label());
			this->lblInformationAuthor = (gcnew System::Windows::Forms::Label());
			this->grpLODControlResource = (gcnew System::Windows::Forms::GroupBox());
			this->numLODControlClampV = (gcnew System::Windows::Forms::NumericUpDown());
			this->lblLODControlClampV = (gcnew System::Windows::Forms::Label());
			this->numLODControlClampU = (gcnew System::Windows::Forms::NumericUpDown());
			this->lblLODControlClampU = (gcnew System::Windows::Forms::Label());
			this->grpMiscellaneous->SuspendLayout();
			this->grpGeneral->SuspendLayout();
			this->grpMipmaps->SuspendLayout();
			this->grpGammaCorrection->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numGammaCorrection))->BeginInit();
			this->grpGeneralOptions->SuspendLayout();
			this->grpResize->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numLuminanceWeightsR))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numLuminanceWeightsG))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numLuminanceWeightsB))->BeginInit();
			this->tabOptions->SuspendLayout();
			this->tabGeneral->SuspendLayout();
			this->tabAdvanced->SuspendLayout();
			this->grpAdvancedOptions->SuspendLayout();
			this->grpVersion->SuspendLayout();
			this->grpLuminanceWeights->SuspendLayout();
			this->tabResources->SuspendLayout();
			this->grpResourceOptions->SuspendLayout();
			this->grpInformationResource->SuspendLayout();
			this->grpLODControlResource->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numLODControlClampV))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numLODControlClampU))->BeginInit();
			this->SuspendLayout();
			// 
			// grpMiscellaneous
			// 
			this->grpMiscellaneous->ContextMenu = this->mnuReset;
			this->grpMiscellaneous->Controls->Add(this->chkReflectivity);
			this->grpMiscellaneous->Controls->Add(this->chkThumbnail);
			this->grpMiscellaneous->Controls->Add(this->chkSphereMap);
			this->grpMiscellaneous->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpMiscellaneous->Location = System::Drawing::Point(10, 250);
			this->grpMiscellaneous->Name = L"grpMiscellaneous";
			this->grpMiscellaneous->Size = System::Drawing::Size(326, 111);
			this->grpMiscellaneous->TabIndex = 1;
			this->grpMiscellaneous->TabStop = false;
			this->grpMiscellaneous->Text = L"Miscellaneous:";
			// 
			// mnuReset
			// 
			this->mnuReset->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(1) { this->btnReset });
			// 
			// btnReset
			// 
			this->btnReset->Index = 0;
			this->btnReset->Text = L"&Reset";
			this->btnReset->Click += gcnew System::EventHandler(this, &CVTFOptions::btnReset_Click);
			// 
			// chkReflectivity
			// 
			this->chkReflectivity->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->chkReflectivity->Location = System::Drawing::Point(10, 18);
			this->chkReflectivity->Name = L"chkReflectivity";
			this->chkReflectivity->Size = System::Drawing::Size(307, 28);
			this->chkReflectivity->TabIndex = 0;
			this->chkReflectivity->Text = L"Compute Reflectivity";
			this->tipMain->SetToolTip(this->chkReflectivity, L"Compute input image reflectivity.");
			// 
			// chkThumbnail
			// 
			this->chkThumbnail->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->chkThumbnail->Location = System::Drawing::Point(10, 46);
			this->chkThumbnail->Name = L"chkThumbnail";
			this->chkThumbnail->Size = System::Drawing::Size(307, 28);
			this->chkThumbnail->TabIndex = 1;
			this->chkThumbnail->Text = L"Generate Thumbnail";
			this->tipMain->SetToolTip(this->chkThumbnail, L"Generate a thumbnail image off of the mipmaps.  The Source engine uses this thumb"
				L"nail for image color sampling.");
			// 
			// chkSphereMap
			// 
			this->chkSphereMap->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->chkSphereMap->Location = System::Drawing::Point(10, 74);
			this->chkSphereMap->Name = L"chkSphereMap";
			this->chkSphereMap->Size = System::Drawing::Size(307, 23);
			this->chkSphereMap->TabIndex = 2;
			this->chkSphereMap->Text = L"Generate Sphere Map";
			this->tipMain->SetToolTip(this->chkSphereMap, L"Generate a sphere map off of the six cube map faces.");
			// 
			// cboTextureType
			// 
			this->cboTextureType->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboTextureType->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->cboTextureType->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Animated Texture", L"Environment Map", L"Volume Texture" });
			this->cboTextureType->Location = System::Drawing::Point(125, 83);
			this->cboTextureType->Name = L"cboTextureType";
			this->cboTextureType->Size = System::Drawing::Size(192, 28);
			this->cboTextureType->TabIndex = 5;
			this->tipMain->SetToolTip(this->cboTextureType, L"The output image type.");
			this->cboTextureType->SelectedIndexChanged += gcnew System::EventHandler(this, &CVTFOptions::cboTextureType_SelectedIndexChanged);
			// 
			// lblTextureType
			// 
			this->lblTextureType->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblTextureType->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->lblTextureType->Location = System::Drawing::Point(10, 83);
			this->lblTextureType->Name = L"lblTextureType";
			this->lblTextureType->Size = System::Drawing::Size(105, 19);
			this->lblTextureType->TabIndex = 4;
			this->lblTextureType->Text = L"Texture Type:";
			// 
			// cboMipmapFilter
			// 
			this->cboMipmapFilter->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboMipmapFilter->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->cboMipmapFilter->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Box", L"NICE" });
			this->cboMipmapFilter->Location = System::Drawing::Point(125, 55);
			this->cboMipmapFilter->Name = L"cboMipmapFilter";
			this->cboMipmapFilter->Size = System::Drawing::Size(192, 28);
			this->cboMipmapFilter->TabIndex = 2;
			this->tipMain->SetToolTip(this->cboMipmapFilter, L"The filter for resampling image data.");
			// 
			// lblMipmapFilter
			// 
			this->lblMipmapFilter->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblMipmapFilter->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->lblMipmapFilter->Location = System::Drawing::Point(10, 55);
			this->lblMipmapFilter->Name = L"lblMipmapFilter";
			this->lblMipmapFilter->Size = System::Drawing::Size(105, 19);
			this->lblMipmapFilter->TabIndex = 1;
			this->lblMipmapFilter->Text = L"Mipmap Filter:";
			// 
			// chkMipmaps
			// 
			this->chkMipmaps->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->chkMipmaps->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->chkMipmaps->Location = System::Drawing::Point(10, 18);
			this->chkMipmaps->Name = L"chkMipmaps";
			this->chkMipmaps->Size = System::Drawing::Size(307, 28);
			this->chkMipmaps->TabIndex = 0;
			this->chkMipmaps->Text = L"Generate Mipmaps";
			this->tipMain->SetToolTip(this->chkMipmaps, L"Generate mipmaps. Without mipmaps your texture will have no LOD scaling, but will"
				L" also be smaller (memory wise).");
			this->chkMipmaps->CheckedChanged += gcnew System::EventHandler(this, &CVTFOptions::chkMipmaps_CheckedChanged);
			// 
			// cboFormat
			// 
			this->cboFormat->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboFormat->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->cboFormat->Items->AddRange(gcnew cli::array< System::Object^  >(16) {
				L"RGB888", L"BGR888", L"RGB565", L"I8", L"P8 (Not supported)", L"RGB888 Bluescreen", L"BGR888 Bluescreen",
					L"DXT1", L"BGRX8888", L"BGR565", L"BGRX5551", L"UV88", L"UVLX8888", L"R8", L"BC6H", L"BC6H Unsigned"
			});
			this->cboFormat->Location = System::Drawing::Point(125, 28);
			this->cboFormat->Name = L"cboFormat";
			this->cboFormat->Size = System::Drawing::Size(192, 28);
			this->cboFormat->TabIndex = 1;
			this->tipMain->SetToolTip(this->cboFormat, L"The output image format for textures with no alpha channel. Common values are DXT"
				L"1, BGR888 and UV88.");
			// 
			// lblFormat
			// 
			this->lblFormat->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblFormat->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->lblFormat->Location = System::Drawing::Point(10, 28);
			this->lblFormat->Name = L"lblFormat";
			this->lblFormat->Size = System::Drawing::Size(105, 18);
			this->lblFormat->TabIndex = 0;
			this->lblFormat->Text = L"Colour Format:";
			// 
			// btnOK
			// 
			this->btnOK->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnOK->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->btnOK->Location = System::Drawing::Point(282, 589);
			this->btnOK->Name = L"btnOK";
			this->btnOK->Size = System::Drawing::Size(96, 33);
			this->btnOK->TabIndex = 0;
			this->btnOK->Text = L"&OK";
			this->btnOK->Click += gcnew System::EventHandler(this, &CVTFOptions::btnOK_Click);
			// 
			// btnCancel
			// 
			this->btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->btnCancel->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnCancel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->btnCancel->Location = System::Drawing::Point(180, 589);
			this->btnCancel->Name = L"btnCancel";
			this->btnCancel->Size = System::Drawing::Size(96, 33);
			this->btnCancel->TabIndex = 1;
			this->btnCancel->Text = L"&Cancel";
			this->btnCancel->Click += gcnew System::EventHandler(this, &CVTFOptions::btnCancel_Click);
			// 
			// grpGeneral
			// 
			this->grpGeneral->ContextMenu = this->mnuReset;
			this->grpGeneral->Controls->Add(this->chkSrgb);
			this->grpGeneral->Controls->Add(this->chkStripAlpha);
			this->grpGeneral->Controls->Add(this->cboAlphaFormat);
			this->grpGeneral->Controls->Add(this->lblAlphaFormat);
			this->grpGeneral->Controls->Add(this->cboFormat);
			this->grpGeneral->Controls->Add(this->lblFormat);
			this->grpGeneral->Controls->Add(this->cboTextureType);
			this->grpGeneral->Controls->Add(this->lblTextureType);
			this->grpGeneral->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpGeneral->Location = System::Drawing::Point(10, 18);
			this->grpGeneral->Name = L"grpGeneral";
			this->grpGeneral->Size = System::Drawing::Size(328, 171);
			this->grpGeneral->TabIndex = 0;
			this->grpGeneral->TabStop = false;
			this->grpGeneral->Text = L"General:";
			// 
			// chkStripAlpha
			//
			this->chkStripAlpha->AutoSize = true;
			this->chkStripAlpha->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->chkStripAlpha->Location = System::Drawing::Point(12, 113);
			this->chkStripAlpha->Name = L"chkStripAlpha";
			this->chkStripAlpha->Size = System::Drawing::Size(170, 24);
			this->chkStripAlpha->TabIndex = 6;
			this->chkStripAlpha->Text = L"Strip Alpha Channel";
			this->tipMain->SetToolTip(this->chkStripAlpha, L"Discard the alpha channel of imported images. The colour format is always used"
				L" , even if the image has alpha data.");
			this->chkStripAlpha->UseVisualStyleBackColor = true;
			//
			// chkSrgb
			//
			this->chkSrgb->AutoSize = true;
			this->chkSrgb->Checked = true;
			this->chkSrgb->CheckState = System::Windows::Forms::CheckState::Checked;
			this->chkSrgb->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->chkSrgb->Location = System::Drawing::Point(12, 137);
			this->chkSrgb->Name = L"chkSrgb";
			this->chkSrgb->Size = System::Drawing::Size(170, 24);
			this->chkSrgb->TabIndex = 7;
			this->chkSrgb->Text = L"sRGB Color Space";
			this->chkSrgb->UseVisualStyleBackColor = true;
			// 
			// cboAlphaFormat
			// 
			this->cboAlphaFormat->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboAlphaFormat->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->cboAlphaFormat->Items->AddRange(gcnew cli::array< System::Object^  >(15) {
				L"RGBA8888", L"ABGR8888", L"IA88", L"A8", L"ARGB8888", L"BGRA8888", L"DXT3", L"DXT5", L"BGRA4444",
					L"DXT1 With One Bit Alpha", L"BGRA5551", L"UVWQ8888", L"RGBA16161616F", L"RGBA16161616", L"BC7"
			});
			this->cboAlphaFormat->Location = System::Drawing::Point(125, 55);
			this->cboAlphaFormat->Name = L"cboAlphaFormat";
			this->cboAlphaFormat->Size = System::Drawing::Size(192, 28);
			this->cboAlphaFormat->TabIndex = 3;
			this->tipMain->SetToolTip(this->cboAlphaFormat, L"The output image format for textures with an alpha channel. Common values are DXT"
				L"5 and BGRA888.");
			// 
			// lblAlphaFormat
			// 
			this->lblAlphaFormat->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblAlphaFormat->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->lblAlphaFormat->Location = System::Drawing::Point(10, 55);
			this->lblAlphaFormat->Name = L"lblAlphaFormat";
			this->lblAlphaFormat->Size = System::Drawing::Size(105, 19);
			this->lblAlphaFormat->TabIndex = 2;
			this->lblAlphaFormat->Text = L"Alpha Format:";
			// 
			// grpMipmaps
			// 
			this->grpMipmaps->ContextMenu = this->mnuReset;
			this->grpMipmaps->Controls->Add(this->cboMipmapFilter);
			this->grpMipmaps->Controls->Add(this->lblMipmapFilter);
			this->grpMipmaps->Controls->Add(this->chkMipmaps);
			this->grpMipmaps->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpMipmaps->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->grpMipmaps->Location = System::Drawing::Point(13, 438);
			this->grpMipmaps->Name = L"grpMipmaps";
			this->grpMipmaps->Size = System::Drawing::Size(335, 97);
			this->grpMipmaps->TabIndex = 2;
			this->grpMipmaps->TabStop = false;
			this->grpMipmaps->Text = L"Mipmaps:";
			this->grpMipmaps->Enter += gcnew System::EventHandler(this, &CVTFOptions::grpMipmaps_Enter);
			// 
			// grpGammaCorrection
			// 
			this->grpGammaCorrection->ContextMenu = this->mnuReset;
			this->grpGammaCorrection->Controls->Add(this->numGammaCorrection);
			this->grpGammaCorrection->Controls->Add(this->lblGammaCorrection);
			this->grpGammaCorrection->Controls->Add(this->chkGammaCorrection);
			this->grpGammaCorrection->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpGammaCorrection->Location = System::Drawing::Point(10, 148);
			this->grpGammaCorrection->Name = L"grpGammaCorrection";
			this->grpGammaCorrection->Size = System::Drawing::Size(326, 93);
			this->grpGammaCorrection->TabIndex = 3;
			this->grpGammaCorrection->TabStop = false;
			this->grpGammaCorrection->Text = L"Gamma Correction:";
			// 
			// numGammaCorrection
			// 
			this->numGammaCorrection->DecimalPlaces = 2;
			this->numGammaCorrection->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 131072 });
			this->numGammaCorrection->Location = System::Drawing::Point(125, 55);
			this->numGammaCorrection->Name = L"numGammaCorrection";
			this->numGammaCorrection->Size = System::Drawing::Size(192, 26);
			this->numGammaCorrection->TabIndex = 2;
			this->tipMain->SetToolTip(this->numGammaCorrection, L"The amount of gamma correction to apply.");
			// 
			// lblGammaCorrection
			// 
			this->lblGammaCorrection->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblGammaCorrection->Location = System::Drawing::Point(10, 55);
			this->lblGammaCorrection->Name = L"lblGammaCorrection";
			this->lblGammaCorrection->Size = System::Drawing::Size(105, 19);
			this->lblGammaCorrection->TabIndex = 1;
			this->lblGammaCorrection->Text = L"Correction:";
			// 
			// chkGammaCorrection
			// 
			this->chkGammaCorrection->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->chkGammaCorrection->Location = System::Drawing::Point(10, 18);
			this->chkGammaCorrection->Name = L"chkGammaCorrection";
			this->chkGammaCorrection->Size = System::Drawing::Size(307, 28);
			this->chkGammaCorrection->TabIndex = 0;
			this->chkGammaCorrection->Text = L"Correct Gamma";
			this->tipMain->SetToolTip(this->chkGammaCorrection, L"Correct input image gamma.");
			this->chkGammaCorrection->CheckedChanged += gcnew System::EventHandler(this, &CVTFOptions::chkGammaCorrection_CheckedChanged);
			// 
			// grpGeneralOptions
			// 
			this->grpGeneralOptions->Controls->Add(this->grpResize);
			this->grpGeneralOptions->Controls->Add(this->grpGeneral);
			this->grpGeneralOptions->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpGeneralOptions->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->grpGeneralOptions->Location = System::Drawing::Point(10, 9);
			this->grpGeneralOptions->Name = L"grpGeneralOptions";
			this->grpGeneralOptions->Size = System::Drawing::Size(351, 423);
			this->grpGeneralOptions->TabIndex = 2;
			this->grpGeneralOptions->TabStop = false;
			this->grpGeneralOptions->Text = L"General Options:";
			// 
			// grpResize
			// 
			this->grpResize->ContextMenu = this->mnuReset;
			this->grpResize->Controls->Add(this->cboMaximumHeight);
			this->grpResize->Controls->Add(this->lblMaximumHeight);
			this->grpResize->Controls->Add(this->cboMaximumWidth);
			this->grpResize->Controls->Add(this->lblMaximumWidth);
			this->grpResize->Controls->Add(this->chkResizeClamp);
			this->grpResize->Controls->Add(this->cboResizeMethod);
			this->grpResize->Controls->Add(this->lblResizeMethod);
			this->grpResize->Controls->Add(this->cboResizeFilter);
			this->grpResize->Controls->Add(this->lblResizeFilter);
			this->grpResize->Controls->Add(this->chkResize);
			this->grpResize->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpResize->Location = System::Drawing::Point(12, 195);
			this->grpResize->Name = L"grpResize";
			this->grpResize->Size = System::Drawing::Size(326, 214);
			this->grpResize->TabIndex = 1;
			this->grpResize->TabStop = false;
			this->grpResize->Text = L"Resize:";
			// 
			// cboMaximumHeight
			// 
			this->cboMaximumHeight->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboMaximumHeight->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->cboMaximumHeight->Items->AddRange(gcnew cli::array< System::Object^  >(13) {
				L"1", L"2", L"4", L"8", L"16", L"32", L"64",
					L"128", L"256", L"512", L"1024", L"2048", L"4096"
			});
			this->cboMaximumHeight->Location = System::Drawing::Point(125, 178);
			this->cboMaximumHeight->Name = L"cboMaximumHeight";
			this->cboMaximumHeight->Size = System::Drawing::Size(192, 28);
			this->cboMaximumHeight->TabIndex = 11;
			this->tipMain->SetToolTip(this->cboMaximumHeight, L"Maximum height.");
			// 
			// lblMaximumHeight
			// 
			this->lblMaximumHeight->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblMaximumHeight->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->lblMaximumHeight->Location = System::Drawing::Point(10, 178);
			this->lblMaximumHeight->Name = L"lblMaximumHeight";
			this->lblMaximumHeight->Size = System::Drawing::Size(105, 19);
			this->lblMaximumHeight->TabIndex = 10;
			this->lblMaximumHeight->Text = L"Maximum Height:";
			// 
			// cboMaximumWidth
			// 
			this->cboMaximumWidth->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboMaximumWidth->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->cboMaximumWidth->Items->AddRange(gcnew cli::array< System::Object^  >(13) {
				L"1", L"2", L"4", L"8", L"16", L"32", L"64",
					L"128", L"256", L"512", L"1024", L"2048", L"4096"
			});
			this->cboMaximumWidth->Location = System::Drawing::Point(125, 150);
			this->cboMaximumWidth->Name = L"cboMaximumWidth";
			this->cboMaximumWidth->Size = System::Drawing::Size(192, 28);
			this->cboMaximumWidth->TabIndex = 9;
			this->tipMain->SetToolTip(this->cboMaximumWidth, L"Maximum width.");
			// 
			// lblMaximumWidth
			// 
			this->lblMaximumWidth->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblMaximumWidth->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->lblMaximumWidth->Location = System::Drawing::Point(10, 150);
			this->lblMaximumWidth->Name = L"lblMaximumWidth";
			this->lblMaximumWidth->Size = System::Drawing::Size(105, 19);
			this->lblMaximumWidth->TabIndex = 8;
			this->lblMaximumWidth->Text = L"Maximum Width:";
			// 
			// chkResizeClamp
			// 
			this->chkResizeClamp->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->chkResizeClamp->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->chkResizeClamp->Location = System::Drawing::Point(10, 113);
			this->chkResizeClamp->Name = L"chkResizeClamp";
			this->chkResizeClamp->Size = System::Drawing::Size(307, 28);
			this->chkResizeClamp->TabIndex = 7;
			this->chkResizeClamp->Text = L"Clamp";
			this->tipMain->SetToolTip(this->chkResizeClamp, L"Clamp the resized image\'s size.");
			this->chkResizeClamp->CheckedChanged += gcnew System::EventHandler(this, &CVTFOptions::chkResizeClamp_CheckedChanged);
			// 
			// cboResizeMethod
			// 
			this->cboResizeMethod->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboResizeMethod->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->cboResizeMethod->Items->AddRange(gcnew cli::array< System::Object^  >(3) {
				L"Nearest Power Of 2", L"Biggest Power Of 2",
					L"Smallest Power Of 2"
			});
			this->cboResizeMethod->Location = System::Drawing::Point(125, 55);
			this->cboResizeMethod->Name = L"cboResizeMethod";
			this->cboResizeMethod->Size = System::Drawing::Size(192, 28);
			this->cboResizeMethod->TabIndex = 2;
			this->tipMain->SetToolTip(this->cboResizeMethod, L"The method for choosing which power of 2 to use.");
			// 
			// lblResizeMethod
			// 
			this->lblResizeMethod->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblResizeMethod->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->lblResizeMethod->Location = System::Drawing::Point(10, 55);
			this->lblResizeMethod->Name = L"lblResizeMethod";
			this->lblResizeMethod->Size = System::Drawing::Size(105, 19);
			this->lblResizeMethod->TabIndex = 1;
			this->lblResizeMethod->Text = L"Resize Method:";
			// 
			// cboResizeFilter
			// 
			this->cboResizeFilter->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboResizeFilter->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->cboResizeFilter->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Box", L"NICE" });
			this->cboResizeFilter->Location = System::Drawing::Point(125, 83);
			this->cboResizeFilter->Name = L"cboResizeFilter";
			this->cboResizeFilter->Size = System::Drawing::Size(192, 28);
			this->cboResizeFilter->TabIndex = 4;
			this->tipMain->SetToolTip(this->cboResizeFilter, L"The filter for resampling image data.");
			// 
			// lblResizeFilter
			// 
			this->lblResizeFilter->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblResizeFilter->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->lblResizeFilter->Location = System::Drawing::Point(10, 83);
			this->lblResizeFilter->Name = L"lblResizeFilter";
			this->lblResizeFilter->Size = System::Drawing::Size(105, 19);
			this->lblResizeFilter->TabIndex = 3;
			this->lblResizeFilter->Text = L"Resize Filter:";
			// 
			// chkResize
			// 
			this->chkResize->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->chkResize->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->chkResize->Location = System::Drawing::Point(10, 18);
			this->chkResize->Name = L"chkResize";
			this->chkResize->Size = System::Drawing::Size(307, 28);
			this->chkResize->TabIndex = 0;
			this->chkResize->Text = L"Resize";
			this->tipMain->SetToolTip(this->chkResize, L"Resize the input image to be a power of 2 (if necessary).");
			this->chkResize->CheckedChanged += gcnew System::EventHandler(this, &CVTFOptions::chkResize_CheckedChanged);
			// 
			// tipMain
			// 
			this->tipMain->AutoPopDelay = 10000;
			this->tipMain->InitialDelay = 500;
			this->tipMain->ReshowDelay = 100;
			// 
			// cboVersion
			// 
			this->cboVersion->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboVersion->Items->AddRange(gcnew cli::array< System::Object^  >(7) { L"7.6", L"7.5", L"7.4", L"7.3", L"7.2", L"7.1", L"7.0" });
			this->cboVersion->Location = System::Drawing::Point(125, 28);
			this->cboVersion->Name = L"cboVersion";
			this->cboVersion->Size = System::Drawing::Size(192, 28);
			this->cboVersion->TabIndex = 1;
			this->cboVersion->SelectedIndexChanged += gcnew System::EventHandler(this, &CVTFOptions::cboVersion_SelectedIndexChanged);
			this->tipMain->SetToolTip(this->cboVersion, L"VTF Version number. Higher versions support more features but are less compatible"
				L" with older games.");
			// 
			// chkCreateLODControlResource
			// 
			this->chkCreateLODControlResource->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->chkCreateLODControlResource->Location = System::Drawing::Point(10, 18);
			this->chkCreateLODControlResource->Name = L"chkCreateLODControlResource";
			this->chkCreateLODControlResource->Size = System::Drawing::Size(307, 28);
			this->chkCreateLODControlResource->TabIndex = 0;
			this->chkCreateLODControlResource->Text = L"Create LOD Control Resource";
			this->tipMain->SetToolTip(this->chkCreateLODControlResource, L"Clamp texture LODs on the U and V axes.");
			this->chkCreateLODControlResource->CheckedChanged += gcnew System::EventHandler(this, &CVTFOptions::chkCreateLODControlResource_CheckedChanged);
			// 
			// chkCreateInformationResource
			// 
			this->chkCreateInformationResource->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->chkCreateInformationResource->Location = System::Drawing::Point(10, 18);
			this->chkCreateInformationResource->Name = L"chkCreateInformationResource";
			this->chkCreateInformationResource->Size = System::Drawing::Size(307, 28);
			this->chkCreateInformationResource->TabIndex = 0;
			this->chkCreateInformationResource->Text = L"Create Information Resource";
			this->tipMain->SetToolTip(this->chkCreateInformationResource, L"Embed texture metadata.\r\nNote: Creates an unofficial KVF resource.");
			this->chkCreateInformationResource->CheckedChanged += gcnew System::EventHandler(this, &CVTFOptions::chkCreateInformationResource_CheckedChanged);
			// 
			// numLuminanceWeightsR
			// 
			this->numLuminanceWeightsR->DecimalPlaces = 3;
			this->numLuminanceWeightsR->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 196608 });
			this->numLuminanceWeightsR->Location = System::Drawing::Point(125, 28);
			this->numLuminanceWeightsR->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numLuminanceWeightsR->Name = L"numLuminanceWeightsR";
			this->numLuminanceWeightsR->Size = System::Drawing::Size(192, 26);
			this->numLuminanceWeightsR->TabIndex = 1;
			// 
			// numLuminanceWeightsG
			// 
			this->numLuminanceWeightsG->DecimalPlaces = 3;
			this->numLuminanceWeightsG->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 196608 });
			this->numLuminanceWeightsG->Location = System::Drawing::Point(125, 55);
			this->numLuminanceWeightsG->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numLuminanceWeightsG->Name = L"numLuminanceWeightsG";
			this->numLuminanceWeightsG->Size = System::Drawing::Size(192, 26);
			this->numLuminanceWeightsG->TabIndex = 3;
			// 
			// numLuminanceWeightsB
			// 
			this->numLuminanceWeightsB->DecimalPlaces = 3;
			this->numLuminanceWeightsB->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 196608 });
			this->numLuminanceWeightsB->Location = System::Drawing::Point(125, 83);
			this->numLuminanceWeightsB->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numLuminanceWeightsB->Name = L"numLuminanceWeightsB";
			this->numLuminanceWeightsB->Size = System::Drawing::Size(192, 26);
			this->numLuminanceWeightsB->TabIndex = 5;
			// 
			// tabOptions
			// 
			this->tabOptions->Controls->Add(this->tabGeneral);
			this->tabOptions->Controls->Add(this->tabAdvanced);
			this->tabOptions->Controls->Add(this->tabResources);
			this->tabOptions->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->tabOptions->Location = System::Drawing::Point(10, 9);
			this->tabOptions->Name = L"tabOptions";
			this->tabOptions->SelectedIndex = 0;
			this->tabOptions->Size = System::Drawing::Size(372, 574);
			this->tabOptions->TabIndex = 3;
			// 
			// tabGeneral
			// 
			this->tabGeneral->Controls->Add(this->grpGeneralOptions);
			this->tabGeneral->Controls->Add(this->grpMipmaps);
			this->tabGeneral->Location = System::Drawing::Point(4, 29);
			this->tabGeneral->Name = L"tabGeneral";
			this->tabGeneral->Size = System::Drawing::Size(364, 541);
			this->tabGeneral->TabIndex = 0;
			this->tabGeneral->Text = L"General";
			// 
			// tabAdvanced
			// 
			this->tabAdvanced->Controls->Add(this->grpAdvancedOptions);
			this->tabAdvanced->Controls->Add(this->grpLuminanceWeights);
			this->tabAdvanced->Location = System::Drawing::Point(4, 29);
			this->tabAdvanced->Name = L"tabAdvanced";
			this->tabAdvanced->Size = System::Drawing::Size(364, 541);
			this->tabAdvanced->TabIndex = 1;
			this->tabAdvanced->Text = L"Advanced";
			// 
			// grpAdvancedOptions
			// 
			this->grpAdvancedOptions->Controls->Add(this->grpVersion);
			this->grpAdvancedOptions->Controls->Add(this->grpGammaCorrection);
			this->grpAdvancedOptions->Controls->Add(this->grpMiscellaneous);
			this->grpAdvancedOptions->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpAdvancedOptions->Location = System::Drawing::Point(10, 9);
			this->grpAdvancedOptions->Name = L"grpAdvancedOptions";
			this->grpAdvancedOptions->Size = System::Drawing::Size(347, 373);
			this->grpAdvancedOptions->TabIndex = 0;
			this->grpAdvancedOptions->TabStop = false;
			this->grpAdvancedOptions->Text = L"Advanced Options:";
			// 
			// grpVersion
			// 
			this->grpVersion->ContextMenu = this->mnuReset;
			this->grpVersion->Controls->Add(this->cboVersion);
			this->grpVersion->Controls->Add(this->lblVersion);
			this->grpVersion->Controls->Add(this->cboCompressionLevel);
			this->grpVersion->Controls->Add(this->lblCompressionLevel);
			this->grpVersion->Controls->Add(this->cboCompressionMethod);
			this->grpVersion->Controls->Add(this->lblCompressionMethod);
			this->grpVersion->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpVersion->Location = System::Drawing::Point(10, 18);
			this->grpVersion->Name = L"grpVersion";
			this->grpVersion->Size = System::Drawing::Size(326, 121);
			this->grpVersion->TabIndex = 0;
			this->grpVersion->TabStop = false;
			this->grpVersion->Text = L"Version:";
			// 
			// cboCompressionLevel
			// 
			this->cboCompressionLevel->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboCompressionLevel->Items->AddRange(gcnew cli::array< System::Object^  >(11) { L"None", L"Default", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9" });
			this->cboCompressionLevel->Location = System::Drawing::Point(125, 56);
			this->cboCompressionLevel->Name = L"cboCompressionLevel";
			this->cboCompressionLevel->Size = System::Drawing::Size(192, 28);
			this->cboCompressionLevel->TabIndex = 3;
			this->cboCompressionLevel->SelectedIndexChanged += gcnew System::EventHandler(this, &CVTFOptions::cboVersion_SelectedIndexChanged);
			this->tipMain->SetToolTip(this->cboCompressionLevel, L"Strength of the CPU compression applied to the image data"
				L" Higher values compress better but take longer. VTF version 7.6 only.");
			// 
			// lblCompressionLevel
			// 
			this->lblCompressionLevel->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblCompressionLevel->Location = System::Drawing::Point(10, 56);
			this->lblCompressionLevel->Name = L"lblCompressionLevel";
			this->lblCompressionLevel->Size = System::Drawing::Size(105, 18);
			this->lblCompressionLevel->TabIndex = 2;
			this->lblCompressionLevel->Text = L"Compression:";
			// 
			// cboCompressionMethod
			// 
			this->cboCompressionMethod->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboCompressionMethod->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Deflate", L"Zstandard" });
			this->cboCompressionMethod->Location = System::Drawing::Point(125, 84);
			this->cboCompressionMethod->Name = L"cboCompressionMethod";
			this->cboCompressionMethod->Size = System::Drawing::Size(192, 28);
			this->cboCompressionMethod->TabIndex = 5;
			this->tipMain->SetToolTip(this->cboCompressionMethod, L"Compression method. Deflate has the widest compatibility.");
			// 
			// lblCompressionMethod
			// 
			this->lblCompressionMethod->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblCompressionMethod->Location = System::Drawing::Point(10, 84);
			this->lblCompressionMethod->Name = L"lblCompressionMethod";
			this->lblCompressionMethod->Size = System::Drawing::Size(105, 18);
			this->lblCompressionMethod->TabIndex = 4;
			this->lblCompressionMethod->Text = L"Method:";
			// 
			// lblVersion
			// 
			this->lblVersion->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblVersion->Location = System::Drawing::Point(10, 28);
			this->lblVersion->Name = L"lblVersion";
			this->lblVersion->Size = System::Drawing::Size(105, 18);
			this->lblVersion->TabIndex = 0;
			this->lblVersion->Text = L"Version:";
			// 
			// grpLuminanceWeights
			// 
			this->grpLuminanceWeights->ContextMenu = this->mnuReset;
			this->grpLuminanceWeights->Controls->Add(this->numLuminanceWeightsB);
			this->grpLuminanceWeights->Controls->Add(this->numLuminanceWeightsG);
			this->grpLuminanceWeights->Controls->Add(this->lblLuminanceWeightsB);
			this->grpLuminanceWeights->Controls->Add(this->lblLuminanceWeightsG);
			this->grpLuminanceWeights->Controls->Add(this->numLuminanceWeightsR);
			this->grpLuminanceWeights->Controls->Add(this->lblLuminanceWeightsR);
			this->grpLuminanceWeights->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpLuminanceWeights->Location = System::Drawing::Point(20, 388);
			this->grpLuminanceWeights->Name = L"grpLuminanceWeights";
			this->grpLuminanceWeights->Size = System::Drawing::Size(326, 120);
			this->grpLuminanceWeights->TabIndex = 4;
			this->grpLuminanceWeights->TabStop = false;
			this->grpLuminanceWeights->Text = L"Luminance Weights:";
			// 
			// lblLuminanceWeightsB
			// 
			this->lblLuminanceWeightsB->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblLuminanceWeightsB->Location = System::Drawing::Point(10, 83);
			this->lblLuminanceWeightsB->Name = L"lblLuminanceWeightsB";
			this->lblLuminanceWeightsB->Size = System::Drawing::Size(105, 19);
			this->lblLuminanceWeightsB->TabIndex = 4;
			this->lblLuminanceWeightsB->Text = L"Blue:";
			// 
			// lblLuminanceWeightsG
			// 
			this->lblLuminanceWeightsG->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblLuminanceWeightsG->Location = System::Drawing::Point(10, 55);
			this->lblLuminanceWeightsG->Name = L"lblLuminanceWeightsG";
			this->lblLuminanceWeightsG->Size = System::Drawing::Size(105, 19);
			this->lblLuminanceWeightsG->TabIndex = 2;
			this->lblLuminanceWeightsG->Text = L"Green:";
			// 
			// lblLuminanceWeightsR
			// 
			this->lblLuminanceWeightsR->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblLuminanceWeightsR->Location = System::Drawing::Point(10, 28);
			this->lblLuminanceWeightsR->Name = L"lblLuminanceWeightsR";
			this->lblLuminanceWeightsR->Size = System::Drawing::Size(105, 18);
			this->lblLuminanceWeightsR->TabIndex = 0;
			this->lblLuminanceWeightsR->Text = L"Red:";
			// 
			// tabResources
			// 
			this->tabResources->Controls->Add(this->grpResourceOptions);
			this->tabResources->Location = System::Drawing::Point(4, 29);
			this->tabResources->Name = L"tabResources";
			this->tabResources->Size = System::Drawing::Size(364, 541);
			this->tabResources->TabIndex = 2;
			this->tabResources->Text = L"Resources";
			// 
			// grpResourceOptions
			// 
			this->grpResourceOptions->Controls->Add(this->grpInformationResource);
			this->grpResourceOptions->Controls->Add(this->grpLODControlResource);
			this->grpResourceOptions->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpResourceOptions->Location = System::Drawing::Point(10, 9);
			this->grpResourceOptions->Name = L"grpResourceOptions";
			this->grpResourceOptions->Size = System::Drawing::Size(681, 397);
			this->grpResourceOptions->TabIndex = 0;
			this->grpResourceOptions->TabStop = false;
			this->grpResourceOptions->Text = L"Resource Options:";
			// 
			// grpInformationResource
			// 
			this->grpInformationResource->ContextMenu = this->mnuReset;
			this->grpInformationResource->Controls->Add(this->txtInformationComments);
			this->grpInformationResource->Controls->Add(this->txtInformationDescription);
			this->grpInformationResource->Controls->Add(this->lblInformationComments);
			this->grpInformationResource->Controls->Add(this->lblInformationDescription);
			this->grpInformationResource->Controls->Add(this->txtInformationModification);
			this->grpInformationResource->Controls->Add(this->txtInformationVersion);
			this->grpInformationResource->Controls->Add(this->lblInformationModification);
			this->grpInformationResource->Controls->Add(this->lblInformationVersion);
			this->grpInformationResource->Controls->Add(this->txtInformationContact);
			this->grpInformationResource->Controls->Add(this->txtInformationAuthor);
			this->grpInformationResource->Controls->Add(this->lblInformationContact);
			this->grpInformationResource->Controls->Add(this->lblInformationAuthor);
			this->grpInformationResource->Controls->Add(this->chkCreateInformationResource);
			this->grpInformationResource->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpInformationResource->Location = System::Drawing::Point(10, 145);
			this->grpInformationResource->Name = L"grpInformationResource";
			this->grpInformationResource->Size = System::Drawing::Size(326, 231);
			this->grpInformationResource->TabIndex = 1;
			this->grpInformationResource->TabStop = false;
			this->grpInformationResource->Text = L"Information Resource:";
			// 
			// txtInformationComments
			// 
			this->txtInformationComments->Location = System::Drawing::Point(125, 194);
			this->txtInformationComments->MaxLength = 8192;
			this->txtInformationComments->Name = L"txtInformationComments";
			this->txtInformationComments->Size = System::Drawing::Size(192, 26);
			this->txtInformationComments->TabIndex = 12;
			// 
			// txtInformationDescription
			// 
			this->txtInformationDescription->Location = System::Drawing::Point(125, 166);
			this->txtInformationDescription->MaxLength = 8192;
			this->txtInformationDescription->Name = L"txtInformationDescription";
			this->txtInformationDescription->Size = System::Drawing::Size(192, 26);
			this->txtInformationDescription->TabIndex = 10;
			// 
			// lblInformationComments
			// 
			this->lblInformationComments->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblInformationComments->Location = System::Drawing::Point(10, 194);
			this->lblInformationComments->Name = L"lblInformationComments";
			this->lblInformationComments->Size = System::Drawing::Size(105, 18);
			this->lblInformationComments->TabIndex = 11;
			this->lblInformationComments->Text = L"Comments:";
			// 
			// lblInformationDescription
			// 
			this->lblInformationDescription->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblInformationDescription->Location = System::Drawing::Point(10, 166);
			this->lblInformationDescription->Name = L"lblInformationDescription";
			this->lblInformationDescription->Size = System::Drawing::Size(105, 19);
			this->lblInformationDescription->TabIndex = 9;
			this->lblInformationDescription->Text = L"Description:";
			// 
			// txtInformationModification
			// 
			this->txtInformationModification->Location = System::Drawing::Point(125, 138);
			this->txtInformationModification->MaxLength = 8192;
			this->txtInformationModification->Name = L"txtInformationModification";
			this->txtInformationModification->Size = System::Drawing::Size(192, 26);
			this->txtInformationModification->TabIndex = 8;
			// 
			// txtInformationVersion
			// 
			this->txtInformationVersion->Location = System::Drawing::Point(125, 111);
			this->txtInformationVersion->MaxLength = 8192;
			this->txtInformationVersion->Name = L"txtInformationVersion";
			this->txtInformationVersion->Size = System::Drawing::Size(192, 26);
			this->txtInformationVersion->TabIndex = 6;
			// 
			// lblInformationModification
			// 
			this->lblInformationModification->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblInformationModification->Location = System::Drawing::Point(10, 138);
			this->lblInformationModification->Name = L"lblInformationModification";
			this->lblInformationModification->Size = System::Drawing::Size(105, 19);
			this->lblInformationModification->TabIndex = 7;
			this->lblInformationModification->Text = L"Modification:";
			// 
			// lblInformationVersion
			// 
			this->lblInformationVersion->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblInformationVersion->Location = System::Drawing::Point(10, 111);
			this->lblInformationVersion->Name = L"lblInformationVersion";
			this->lblInformationVersion->Size = System::Drawing::Size(105, 18);
			this->lblInformationVersion->TabIndex = 5;
			this->lblInformationVersion->Text = L"Version:";
			// 
			// txtInformationContact
			// 
			this->txtInformationContact->Location = System::Drawing::Point(125, 83);
			this->txtInformationContact->MaxLength = 8192;
			this->txtInformationContact->Name = L"txtInformationContact";
			this->txtInformationContact->Size = System::Drawing::Size(192, 26);
			this->txtInformationContact->TabIndex = 4;
			// 
			// txtInformationAuthor
			// 
			this->txtInformationAuthor->Location = System::Drawing::Point(125, 55);
			this->txtInformationAuthor->MaxLength = 8192;
			this->txtInformationAuthor->Name = L"txtInformationAuthor";
			this->txtInformationAuthor->Size = System::Drawing::Size(192, 26);
			this->txtInformationAuthor->TabIndex = 2;
			// 
			// lblInformationContact
			// 
			this->lblInformationContact->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblInformationContact->Location = System::Drawing::Point(10, 83);
			this->lblInformationContact->Name = L"lblInformationContact";
			this->lblInformationContact->Size = System::Drawing::Size(105, 19);
			this->lblInformationContact->TabIndex = 3;
			this->lblInformationContact->Text = L"Contact:";
			// 
			// lblInformationAuthor
			// 
			this->lblInformationAuthor->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblInformationAuthor->Location = System::Drawing::Point(10, 55);
			this->lblInformationAuthor->Name = L"lblInformationAuthor";
			this->lblInformationAuthor->Size = System::Drawing::Size(105, 19);
			this->lblInformationAuthor->TabIndex = 1;
			this->lblInformationAuthor->Text = L"Author:";
			// 
			// grpLODControlResource
			// 
			this->grpLODControlResource->ContextMenu = this->mnuReset;
			this->grpLODControlResource->Controls->Add(this->numLODControlClampV);
			this->grpLODControlResource->Controls->Add(this->lblLODControlClampV);
			this->grpLODControlResource->Controls->Add(this->numLODControlClampU);
			this->grpLODControlResource->Controls->Add(this->lblLODControlClampU);
			this->grpLODControlResource->Controls->Add(this->chkCreateLODControlResource);
			this->grpLODControlResource->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpLODControlResource->Location = System::Drawing::Point(10, 18);
			this->grpLODControlResource->Name = L"grpLODControlResource";
			this->grpLODControlResource->Size = System::Drawing::Size(326, 120);
			this->grpLODControlResource->TabIndex = 0;
			this->grpLODControlResource->TabStop = false;
			this->grpLODControlResource->Text = L"LOD Control Resource:";
			// 
			// numLODControlClampV
			// 
			this->numLODControlClampV->Location = System::Drawing::Point(125, 83);
			this->numLODControlClampV->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 31, 0, 0, 0 });
			this->numLODControlClampV->Name = L"numLODControlClampV";
			this->numLODControlClampV->Size = System::Drawing::Size(192, 26);
			this->numLODControlClampV->TabIndex = 4;
			// 
			// lblLODControlClampV
			// 
			this->lblLODControlClampV->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblLODControlClampV->Location = System::Drawing::Point(10, 83);
			this->lblLODControlClampV->Name = L"lblLODControlClampV";
			this->lblLODControlClampV->Size = System::Drawing::Size(105, 19);
			this->lblLODControlClampV->TabIndex = 3;
			this->lblLODControlClampV->Text = L"Clamp V:";
			// 
			// numLODControlClampU
			// 
			this->numLODControlClampU->Location = System::Drawing::Point(125, 55);
			this->numLODControlClampU->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 31, 0, 0, 0 });
			this->numLODControlClampU->Name = L"numLODControlClampU";
			this->numLODControlClampU->Size = System::Drawing::Size(192, 26);
			this->numLODControlClampU->TabIndex = 2;
			// 
			// lblLODControlClampU
			// 
			this->lblLODControlClampU->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->lblLODControlClampU->Location = System::Drawing::Point(10, 55);
			this->lblLODControlClampU->Name = L"lblLODControlClampU";
			this->lblLODControlClampU->Size = System::Drawing::Size(105, 19);
			this->lblLODControlClampU->TabIndex = 1;
			this->lblLODControlClampU->Text = L"Clamp U:";
			// 
			// CVTFOptions
			// 
			this->AcceptButton = this->btnOK;
			this->AutoScaleBaseSize = System::Drawing::Size(8, 19);
			this->CancelButton = this->btnCancel;
			this->ClientSize = System::Drawing::Size(391, 634);
			this->Controls->Add(this->tabOptions);
			this->Controls->Add(this->btnCancel);
			this->Controls->Add(this->btnOK);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"CVTFOptions";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"VTF Options";
			this->Load += gcnew System::EventHandler(this, &CVTFOptions::CVTFOptions_Load);
			this->grpMiscellaneous->ResumeLayout(false);
			this->grpGeneral->ResumeLayout(false);
			this->grpGeneral->PerformLayout();
			this->grpMipmaps->ResumeLayout(false);
			this->grpGammaCorrection->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numGammaCorrection))->EndInit();
			this->grpGeneralOptions->ResumeLayout(false);
			this->grpResize->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numLuminanceWeightsR))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numLuminanceWeightsG))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numLuminanceWeightsB))->EndInit();
			this->tabOptions->ResumeLayout(false);
			this->tabGeneral->ResumeLayout(false);
			this->tabAdvanced->ResumeLayout(false);
			this->grpAdvancedOptions->ResumeLayout(false);
			this->grpVersion->ResumeLayout(false);
			this->grpLuminanceWeights->ResumeLayout(false);
			this->tabResources->ResumeLayout(false);
			this->grpResourceOptions->ResumeLayout(false);
			this->grpInformationResource->ResumeLayout(false);
			this->grpInformationResource->PerformLayout();
			this->grpLODControlResource->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numLODControlClampV))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numLODControlClampU))->EndInit();
			this->ResumeLayout(false);

		}
	public: property VTFImageFormat NormalFormat
	{
		VTFImageFormat get()
		{
			int iIndex = this->cboFormat->SelectedIndex;
			return iIndex >= 0 ? NormalImageFormats[iIndex] : IMAGE_FORMAT_NONE;
		}
		void set(VTFImageFormat ImageFormat)
		{
			for (int i = 0; i < this->cboFormat->Items->Count; i++)
			{
				if (NormalImageFormats[i] == ImageFormat)
				{
					this->cboFormat->SelectedIndex = i;
					break;
				}
			}
		}
	}

	public: property VTFImageFormat AlphaFormat
	{
		VTFImageFormat get()
		{
			int iIndex = this->cboAlphaFormat->SelectedIndex;
			return iIndex >= 0 ? AlphaImageFormats[iIndex] : IMAGE_FORMAT_NONE;
		}
		void set(VTFImageFormat ImageFormat)
		{
			for (int i = 0; i < this->cboAlphaFormat->Items->Count; i++)
			{
				if (AlphaImageFormats[i] == ImageFormat)
				{
					this->cboAlphaFormat->SelectedIndex = i;
					break;
				}
			}
		}
	}

	public: property int TextureType
	{
		int get()
		{
			return this->cboTextureType->SelectedIndex;
		}
		void set(int iTextureType)
		{
			if (iTextureType >= 0 && iTextureType < this->cboTextureType->Items->Count)
			{
				this->cboTextureType->SelectedIndex = iTextureType;
			}
		}
	}

		  //
		  // Resize.
		  //

	public:property vlBool ResizeImage
	{
		vlBool get()
		{
			return this->chkResize->Checked == true;
		}
		void set(vlBool bResize)
		{
			this->chkResize->Checked = bResize > 0;
		}
	}

	public: property VTFResizeMethod ResizeMethod
	{
		VTFResizeMethod get()
		{
			return (VTFResizeMethod)this->cboResizeMethod->SelectedIndex;
		}
		void set(VTFResizeMethod ResizeMethod)
		{
			int iIndex = Convert::ToInt32(ResizeMethod);
			if (iIndex >= 0 && iIndex < this->cboResizeMethod->Items->Count)
			{
				this->cboResizeMethod->SelectedIndex = iIndex;
			}
		}
	}

	public: property VTFMipmapFilter ResizeFilter
	{
		VTFMipmapFilter get()
		{
			return this->cboResizeFilter->SelectedIndex == 1 ? MIPMAP_FILTER_NICE : MIPMAP_FILTER_BOX;
		}
		void set(VTFMipmapFilter ResizeFilter)
		{
			this->cboResizeFilter->SelectedIndex = ResizeFilter == MIPMAP_FILTER_NICE ? 1 : 0;
		}
	}

	public: property vlBool ResizeClamp
	{
		vlBool get()
		{
			return this->chkResizeClamp->Checked == true;
		}
		void set(vlBool bResize)
		{
			this->chkResizeClamp->Checked = bResize > 0;
		}
	}

	public: property vlUInt ResizeClampWidth
	{
		vlUInt get()
		{
			return Convert::ToUInt32(this->cboMaximumWidth->Text);
		}
		void set(vlUInt uiResizeClampWidth)
		{
			this->cboMaximumWidth->Text = uiResizeClampWidth.ToString();
			if (this->cboMaximumWidth->SelectedIndex == -1)
			{
				this->cboMaximumWidth->SelectedIndex = 12;
			}
		}
	}

	public: property vlUInt ResizeClampHeight
	{
		vlUInt get()
		{
			return Convert::ToUInt32(this->cboMaximumHeight->Text);
		}
		void set(vlUInt uiResizeClampHeight)
		{
			this->cboMaximumHeight->Text = uiResizeClampHeight.ToString();
			if (this->cboMaximumHeight->SelectedIndex == -1)
			{
				this->cboMaximumHeight->SelectedIndex = 12;
			}
		}
	}

		  //
		  // Mipmaps.
		  //

	public: property vlBool GenerateMipmaps
	{
		vlBool get()
		{
			return this->chkMipmaps->Checked == true;
		}
		void set(vlBool bGenerateMipmaps)
		{
			this->chkMipmaps->Checked = bGenerateMipmaps > 0;
		}
	}

	public: property VTFMipmapFilter MipmapFilter
	{
		VTFMipmapFilter get()
		{
			return this->cboMipmapFilter->SelectedIndex == 1 ? MIPMAP_FILTER_NICE : MIPMAP_FILTER_BOX;
		}
		void set(VTFMipmapFilter MipmapFilter)
		{
			this->cboMipmapFilter->SelectedIndex = MipmapFilter == MIPMAP_FILTER_NICE ? 1 : 0;
		}
	}

		  // Version.

	public: property String^ Version
	{
		String^  get()
		{
			return this->cboVersion->Text;
		}
		void set(String^ sVersion)
		{
			this->cboVersion->Text = sVersion;
			if (this->cboVersion->SelectedIndex == -1)
			{
				this->cboVersion->SelectedIndex = 2; // 7.4
			}
		}
	}

		  // Auxiliary compression.

	public: property vlShort AuxCompressionLevel
	{
		vlShort get()
		{
			switch(this->cboCompressionLevel->SelectedIndex)
			{
			case 1:
				return VTF_AUX_COMPRESSION_LEVEL_DEFAULT;
			case -1:
			case 0:
				return VTF_AUX_COMPRESSION_LEVEL_NONE;
			default:
				return (vlShort)(this->cboCompressionLevel->SelectedIndex - 1);
			}
		}
		void set(vlShort sLevel)
		{
			if(sLevel == VTF_AUX_COMPRESSION_LEVEL_DEFAULT)
			{
				this->cboCompressionLevel->SelectedIndex = 1;
			}
			else if(sLevel <= VTF_AUX_COMPRESSION_LEVEL_NONE || sLevel > VTF_AUX_COMPRESSION_LEVEL_MAX)
			{
				this->cboCompressionLevel->SelectedIndex = 0;
			}
			else
			{
				this->cboCompressionLevel->SelectedIndex = sLevel + 1;
			}
		}
	}

	public: property vlShort AuxCompressionMethod
	{
		vlShort get()
		{
			return this->cboCompressionMethod->SelectedIndex == 1 ? AUX_COMPRESSION_METHOD_ZSTD : AUX_COMPRESSION_METHOD_DEFLATE;
		}
		void set(vlShort sMethod)
		{
			this->cboCompressionMethod->SelectedIndex = sMethod == AUX_COMPRESSION_METHOD_ZSTD ? 1 : 0;
		}
	}

		  //
		  // Misc.
		  //

	public: property vlBool ComputeReflectivity
	{
		vlBool get()
		{
			return this->chkReflectivity->Checked == true;
		}
		void set(vlBool bComputeReflectivity)
		{
			this->chkReflectivity->Checked = bComputeReflectivity > 0;
		}
	}

	public: property vlBool GenerateThumbnail
	{
		vlBool get()
		{
			return this->chkThumbnail->Checked == true;
		}
		void set(vlBool bGenerateThumbnail)
		{
			this->chkThumbnail->Checked = bGenerateThumbnail > 0;
		}
	}

	public: property vlBool GenerateSphereMap
	{
		vlBool get()
		{
			return this->chkSphereMap->Enabled && this->chkSphereMap->Checked == true;
		}
		void set(vlBool bGenerateSphereMap)
		{
			this->chkSphereMap->Checked = bGenerateSphereMap > 0;
		}
	}

	public: property vlBool StripAlpha
	{
		vlBool get()
		{
			return this->chkStripAlpha->Checked == true;
		}
		void set(vlBool bStripAlpha)
		{
			this->chkStripAlpha->Checked = bStripAlpha > 0;
		}
	}

	public: property vlBool sRGB
	{
		vlBool get()
		{
			return this->chkSrgb->Enabled && this->chkSrgb->Checked == true;
		}
		void set(vlBool bSRGB)
		{
			this->chkSrgb->Checked = bSRGB > 0;
		}
	}

		  //
		  // Gamma correction.
		  //

	public: property vlBool CorrectGamma
	{
		vlBool get()
		{
			return this->chkGammaCorrection->Checked == true;
		}
		void set(vlBool bGammaCorrection)
		{
			this->chkGammaCorrection->Checked = bGammaCorrection > 0;
		}
	}

	public: property vlSingle GammaCorrection
	{
		vlSingle get()
		{
			return Convert::ToSingle(this->numGammaCorrection->Value);
		}
		void set(vlSingle sGammaCorrection)
		{
			this->SetNumericUpDownValue(this->numGammaCorrection, Convert::ToDecimal(sGammaCorrection));
		}
	}

		  //
		  // Luminance Weights.
		  //

	public: property vlSingle LuminanceWeightR
	{
		vlSingle get()
		{
			return Convert::ToSingle(this->numLuminanceWeightsR->Value);
		}
		void set(vlSingle sLuminanceWeightR)
		{
			this->SetNumericUpDownValue(this->numLuminanceWeightsR, Convert::ToDecimal(sLuminanceWeightR));
		}
	}

	public: property vlSingle LuminanceWeightG
	{
		vlSingle get()
		{
			return Convert::ToSingle(this->numLuminanceWeightsG->Value);
		}
		void set(vlSingle sLuminanceWeightG)
		{
			this->SetNumericUpDownValue(this->numLuminanceWeightsG, Convert::ToDecimal(sLuminanceWeightG));
		}
	}

	public: property vlSingle LuminanceWeightB
	{
		vlSingle get()
		{
			return Convert::ToSingle(this->numLuminanceWeightsB->Value);
		}
		void set(vlSingle sLuminanceWeightB)
		{
			this->SetNumericUpDownValue(this->numLuminanceWeightsB, Convert::ToDecimal(sLuminanceWeightB));
		}
	}

		  // LOD Control Resource

	public: property vlBool CreateLODControlResource
	{
		vlBool get()
		{
			return this->chkCreateLODControlResource->Checked == true;
		}
		void set(vlBool bCreateLODControlResource)
		{
			this->chkCreateLODControlResource->Checked = bCreateLODControlResource > 0;
		}
	}

	public: property vlUInt LODControlClampU
	{
		vlUInt get()
		{
			return Convert::ToUInt32(this->numLODControlClampU->Value);
		}
		void set(vlUInt uiLODControlClampU)
		{
			this->SetNumericUpDownValue(this->numLODControlClampU, Convert::ToDecimal(uiLODControlClampU));
		}
	}

	public: property vlUInt LODControlClampV
	{
		vlUInt get()
		{
			return Convert::ToUInt32(this->numLODControlClampV->Value);
		}
		void set(vlUInt uiLODControlClampV)
		{
			this->SetNumericUpDownValue(this->numLODControlClampV, Convert::ToDecimal(uiLODControlClampV));
		}
	}

	private: System::Void SetNumericUpDownValue(NumericUpDown^ Control, System::Decimal dValue)
	{
		if (dValue < Control->Minimum)
		{
			Control->Value = Control->Minimum;
		}
		else if (dValue > Control->Maximum)
		{
			Control->Value = Control->Maximum;
		}
		else
		{
			Control->Value = dValue;
		}
	}

		   // LOD Control Resource

	public: property vlBool CreateInformationResource
	{
		vlBool get()
		{
			return this->chkCreateInformationResource->Checked == true;
		}
		void set(vlBool bCreateInformationResource)
		{
			this->chkCreateInformationResource->Checked = bCreateInformationResource > 0;
		}
	}

	public: property String^ InformationAuthor
	{
		String^  get()
		{
			return this->txtInformationAuthor->Text;
		}
		void set(String^ sInformationAuthor)
		{
			this->txtInformationAuthor->Text = sInformationAuthor;
		}
	}

	public: property String^ InformationContact
	{
		String^  get()
		{
			return this->txtInformationContact->Text;
		}
		void set(String^ sInformationContact)
		{
			this->txtInformationContact->Text = sInformationContact;
		}
	}

	public: property String^ InformationVersion
	{
		String^ get()
		{
			return this->txtInformationVersion->Text;
		}
		void set(String^ sInformationVersion)
		{
			this->txtInformationVersion->Text = sInformationVersion;
		}
	}

	public: property String^ InformationModification
	{
		String^ get()
		{
			return this->txtInformationModification->Text;
		}
		void set(String^ sInformationModification)
		{
			this->txtInformationModification->Text = sInformationModification;
		}
	}

	public: property String^ InformationDescription
	{
		String^ get()
		{
			return this->txtInformationDescription->Text;
		}
		void set(String^ sInformationDescription)
		{
			this->txtInformationDescription->Text = sInformationDescription;
		}
	}

	public: property String^ InformationComments
	{
		String^ get()
		{
			return this->txtInformationComments->Text;
		}
		void set(String^ sInformationComments)
		{
			this->txtInformationComments->Text = sInformationComments;
		}
	}

	private: System::Void CVTFOptions_Load(System::Object^ sender, System::EventArgs^ e)
	{
		/*this->cboFormat->SelectedIndex = 0;
		this->cboMipmapFilter->SelectedIndex = 1;
		this->cboSharpenFilter->SelectedIndex = 7;
		this->cboTextureType->SelectedIndex = 0;
		this->cboKernelFilter->SelectedIndex = 1;
		this->cboHeightSource->SelectedIndex = 1;*/
	}

	private: System::Void cboVersion_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		// hack
		vlBool bSupported = System::String::Compare(this->cboVersion->Text, L"7.6") == 0;

		this->lblCompressionLevel->Enabled = bSupported;
		this->cboCompressionLevel->Enabled = bSupported;
		this->lblCompressionMethod->Enabled = bSupported;
		this->cboCompressionMethod->Enabled = bSupported && this->AuxCompressionLevel != VTF_AUX_COMPRESSION_LEVEL_NONE;
	}

	private: System::Void cboTextureType_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->chkSphereMap->Enabled = this->cboTextureType->SelectedIndex == 1;
	}

	private: System::Void chkResize_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->cboResizeMethod->Enabled = this->chkResize->Checked;
		this->cboResizeFilter->Enabled = this->chkResize->Checked;

		this->chkResizeClamp->Enabled = this->chkResize->Checked;
		this->cboMaximumWidth->Enabled = this->chkResize->Checked && this->chkResizeClamp->Checked;
		this->cboMaximumHeight->Enabled = this->chkResize->Checked && this->chkResizeClamp->Checked;
	}

	private: System::Void chkResizeClamp_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->cboMaximumWidth->Enabled = this->chkResize->Checked && this->chkResizeClamp->Checked;
		this->cboMaximumHeight->Enabled = this->chkResize->Checked && this->chkResizeClamp->Checked;
	}

	private: System::Void chkMipmaps_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->cboMipmapFilter->Enabled = this->chkMipmaps->Checked;
	}

	private: System::Void chkNormalMap_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
	}

	private: System::Void chkGammaCorrection_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->numGammaCorrection->Enabled = this->chkGammaCorrection->Checked;
	}

	private: System::Void chkCreateLODControlResource_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->numLODControlClampU->Enabled = this->chkCreateLODControlResource->Checked;
		this->numLODControlClampV->Enabled = this->chkCreateLODControlResource->Checked;
	}

	private: System::Void chkCreateInformationResource_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->txtInformationAuthor->Enabled = this->chkCreateInformationResource->Checked;
		this->txtInformationContact->Enabled = this->chkCreateInformationResource->Checked;
		this->txtInformationVersion->Enabled = this->chkCreateInformationResource->Checked;
		this->txtInformationModification->Enabled = this->chkCreateInformationResource->Checked;
		this->txtInformationDescription->Enabled = this->chkCreateInformationResource->Checked;
		this->txtInformationComments->Enabled = this->chkCreateInformationResource->Checked;
	}

	private: System::Void btnReset_Click(System::Object^ sender, System::EventArgs^ e)
	{
		this->cboFormat->SelectedIndex = 7;			// DXT1
		this->cboAlphaFormat->SelectedIndex = 7;	// DXT5
		this->cboTextureType->SelectedIndex = 0;

		this->chkResize->Checked = true;
		this->cboResizeMethod->SelectedIndex = 0;
		this->cboResizeFilter->SelectedIndex = 1; // NICE

		this->chkResizeClamp->Checked = true;
		this->cboMaximumWidth->SelectedIndex = 12;
		this->cboMaximumHeight->SelectedIndex = 12;

		this->chkMipmaps->Checked = true;
		this->cboMipmapFilter->SelectedIndex = 1; // NICE

		this->cboVersion->SelectedIndex = 2; // 7.4

		this->cboCompressionLevel->SelectedIndex = 0; // None
		this->cboCompressionMethod->SelectedIndex = 0; // Deflate

		this->chkReflectivity->Checked = true;
		this->chkThumbnail->Checked = true;
		this->chkSphereMap->Checked = true;
		this->chkStripAlpha->Checked = false;
		this->chkSrgb->Checked = true;

		this->chkGammaCorrection->Checked = false;
		this->numGammaCorrection->Value = System::Decimal(2.2f);

		this->numLuminanceWeightsR->Value = System::Decimal(0.299f);
		this->numLuminanceWeightsG->Value = System::Decimal(0.587f);
		this->numLuminanceWeightsB->Value = System::Decimal(0.114f);

		this->chkCreateLODControlResource->Checked = false;
		this->numLODControlClampU->Value = this->numLODControlClampU->Maximum;
		this->numLODControlClampV->Value = this->numLODControlClampV->Maximum;

		this->chkCreateInformationResource->Checked = false;
		this->txtInformationAuthor->Clear();
		this->txtInformationContact->Clear();
		this->txtInformationVersion->Clear();
		this->txtInformationModification->Clear();
		this->txtInformationDescription->Clear();
		this->txtInformationComments->Clear();

		this->cboTextureType_SelectedIndexChanged(this, System::EventArgs::Empty);
		this->chkResize_CheckedChanged(this, System::EventArgs::Empty);
		this->chkResizeClamp_CheckedChanged(this, System::EventArgs::Empty);
		this->chkGammaCorrection_CheckedChanged(this, System::EventArgs::Empty);
		this->chkMipmaps_CheckedChanged(this, System::EventArgs::Empty);
		this->chkNormalMap_CheckedChanged(this, System::EventArgs::Empty);
		this->chkCreateLODControlResource_CheckedChanged(this, System::EventArgs::Empty);
		this->chkCreateInformationResource_CheckedChanged(this, System::EventArgs::Empty);
		this->cboVersion_SelectedIndexChanged(this, System::EventArgs::Empty);
	}

	private: System::Void btnOK_Click(System::Object^ sender, System::EventArgs^ e)
	{
		this->DialogResult = System::Windows::Forms::DialogResult::OK;
	}

	private: System::Void btnCancel_Click(System::Object^ sender, System::EventArgs^ e)
	{
		this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
	}
	private: System::Void grpMipmaps_Enter(System::Object^ sender, System::EventArgs^ e) {
	}
};
}