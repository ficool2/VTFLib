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

#include "stdafx.h"
#include "About.h"
#include "VTFOptions.h"
#include "VMTCreate.h"
#include "BatchConvert.h"
#include "Utility.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace VTFEdit
{
	public ref class CVTFEdit : public System::Windows::Forms::Form, public System::Windows::Forms::IMessageFilter
	{
	private:
		System::String ^sFileName;

		VTFLib::CVMTFile *VMTFile;
		VTFLib::CVTFFile *VTFFile;

		bool bHDRReseting;
		float fImageScale;
		float fEffectiveImageScale;
		unsigned char *ucImageData;

		bool bImagePanning;
		System::Drawing::Point ImagePanStartMouse;
		System::Drawing::Point ImagePanStartScroll;

		CVMTFileUtility::CSyntaxHilighter ^SyntaxHilighter;

		unsigned int uiMaximumRecentFiles;
		System::Collections::ArrayList ^RecentFiles;

		bool bFormRestoring;
		System::Drawing::Point FormSaveLocation;
		System::Drawing::Size FormSaveSize;
		FormWindowState FormSaveWindowState;
		int iFormSaveSidebarSplitPosition;
		int iFormSaveSidebarRightSplitPosition;

		CVTFOptions ^Options;
		CVMTCreate ^VMTCreate;
		CBatchConvert ^BatchConvert;
		CAbout ^About;

		HWND hWndNewViewer;

	public: 
		CVTFEdit()
		{
			this->sFileName = nullptr;

			this->VMTFile = nullptr;
			this->VTFFile = nullptr;

			this->bHDRReseting = false;
			this->fImageScale = 1.0f;
			this->fEffectiveImageScale = 1.0f;
			this->ucImageData = nullptr;

			this->bImagePanning = false;

			this->uiMaximumRecentFiles = 8;
			this->RecentFiles = gcnew System::Collections::ArrayList();

			this->bFormRestoring = false;
			this->iFormSaveSidebarSplitPosition = 258;
			this->iFormSaveSidebarRightSplitPosition = 258;

			this->Options = gcnew CVTFOptions();
			this->VMTCreate = gcnew CVMTCreate();
			this->BatchConvert = gcnew CBatchConvert(this->Options);
			this->About = gcnew CAbout();

			this->hWndNewViewer = 0;

			this->InitializeComponent();

			this->SyntaxHilighter = gcnew CVMTFileUtility::CSyntaxHilighter(this->txtVMTFile);

		}

	private: System::Windows::Forms::MenuItem ^  btnVMTFileValidateStrict;
	private: System::Windows::Forms::MenuItem ^  btnVMTFileValidateLoose;
	private: System::Windows::Forms::TabPage ^  tabResources;
	private: System::Windows::Forms::GroupBox ^  grpResourceInfo;
	private: System::Windows::Forms::Label ^  lblResourceCount;
	private: System::Windows::Forms::Label ^  lblResourceCountLabel;
	private: System::Windows::Forms::GroupBox ^  grpResources;
	private: System::Windows::Forms::TreeView ^  treResources;
	private: System::Windows::Forms::ContextMenu ^  mnuHDR;
	private: System::Windows::Forms::MenuItem ^  btnHDRReset;
	private: System::Windows::Forms::StatusBarPanel ^  pnlInfo1;
	private: System::Windows::Forms::StatusBarPanel ^  pnlInfo2;




	private: System::Windows::Forms::MenuItem ^  btnExportAll;
	private: System::Windows::Forms::TrackBar^ trkHDRExposure;

	private: System::Windows::Forms::Label ^  lblHDRKey;
	private: System::Windows::Forms::Label ^  lblSlice;
	private: System::Windows::Forms::NumericUpDown ^  numSlice;
	private: System::Windows::Forms::Label ^  lblImageSlices;
	private: System::Windows::Forms::Label ^  lblImageSlicesLabel;
	private: System::Windows::Forms::StatusBarPanel ^  pnlFileName;
	private: System::Windows::Forms::MenuItem ^  btnVMTFileSpace3;
	private: System::Windows::Forms::MenuItem ^  btnVMTFileValidate;
	private: System::Windows::Forms::MenuItem ^  btnNew;
	private: System::Windows::Forms::ContextMenu ^  mnuVMTFile;
	private: System::Windows::Forms::MenuItem ^  btnVMTFileUndo;
	private: System::Windows::Forms::MenuItem ^  btnVMTFileSpace1;
	private: System::Windows::Forms::MenuItem ^  btnVMTFileCut;
	private: System::Windows::Forms::MenuItem ^  btnVMTFileCopy;
	private: System::Windows::Forms::MenuItem ^  btnVMTFilePaste;
	private: System::Windows::Forms::MenuItem ^  btnVMTFileDelete;
	private: System::Windows::Forms::MenuItem ^  btnVMTFileSpace2;
	private: System::Windows::Forms::MenuItem ^  btnVMTFileSelectAll;
	private: System::Windows::Forms::MenuItem ^  btnConvertFolder;
	private: System::Windows::Forms::RichTextBox ^  txtVMTFile;
	private: System::Windows::Forms::MenuItem ^  btnAutoCreateVMTFile;
	private: System::Windows::Forms::MenuItem ^  btnFileSpace4;
	private: System::Windows::Forms::MenuItem ^  btnCreateVMTFile;
	private: System::Windows::Forms::MenuItem ^  btnToolsMenu;
	private: System::Windows::Forms::GroupBox ^  grpFileInfo;
	private: System::Windows::Forms::Label ^  lblFileSize;
	private: System::Windows::Forms::Label ^  lblFileSizeLabel;
	private: System::Windows::Forms::Label ^  lblFileVersion;
	private: System::Windows::Forms::Label ^  lblFileVersionLabel;
	private: System::Windows::Forms::Label ^  lblFileCompression;
	private: System::Windows::Forms::Label ^  lblFileCompressionLabel;
	private: System::Windows::Forms::MenuItem ^  btnFileSpace3;
	private: System::Windows::Forms::MenuItem ^  btnVTFFileZoomIn;
	private: System::Windows::Forms::MenuItem ^  btnVTFFileZoomOut;
	private: System::Windows::Forms::MenuItem ^  btnVTFFileSpace1;
	private: System::Windows::Forms::MenuItem ^  btnVTFFileZoomReset;
	private: System::Windows::Forms::MenuItem ^  btnVTFFileSpace2;
	private: System::Windows::Forms::MenuItem ^  btnVTFFileCopy;
	private: System::Windows::Forms::ContextMenu ^  mnuVTFFile;
	private: System::Windows::Forms::PictureBox ^  picVTFFileTL;
	private: System::Windows::Forms::PictureBox ^  picVTFFileTR;
	private: System::Windows::Forms::PictureBox ^  picVTFFileBL;
	private: System::Windows::Forms::PictureBox ^  picVTFFileBR;
	private: System::Windows::Forms::MenuItem ^  btnTile;
	private: System::Windows::Forms::MenuItem ^  btnMipmapFullSize;
	private: System::Windows::Forms::MenuItem ^  btnOptionsMenu;
	private: System::Windows::Forms::MenuItem ^  btnMask;
	private: System::Windows::Forms::Splitter ^  splSidebar;
	private: System::Windows::Forms::Splitter ^  splSidebarRight;
	private: System::Windows::Forms::StatusBar ^  barStatus;
	private: System::Windows::Forms::ToolBar ^  barTool;
	private: System::Windows::Forms::ImageList ^  imgTool;
	private: System::Windows::Forms::ToolBarButton ^  btnToolImport;
	private: System::Windows::Forms::ToolBarButton ^  btnToolOpen;
	private: System::Windows::Forms::ToolBarButton ^  btnToolSave;
	private: System::Windows::Forms::ToolBarButton ^  btnSeparator1;
	private: System::Windows::Forms::ToolBarButton ^  btnToolCopy;
	private: System::Windows::Forms::ToolBarButton ^  btnToolPaste;
	private: System::Windows::Forms::MenuItem ^  btnPaste;
	private: System::Windows::Forms::NumericUpDown ^  numImageStartFrame;
	private: System::Windows::Forms::NumericUpDown ^  numImageBumpmapScale;
	private: System::Windows::Forms::MenuItem ^  btnRecentFiles;
	private: System::Windows::Forms::OpenFileDialog ^  dlgImportFile;
	private: System::Windows::Forms::MenuItem ^  btnFileSpace1;
	private: System::Windows::Forms::MenuItem ^  btnImport;
	private: System::Windows::Forms::MenuItem ^  btnExport;
	private: System::Windows::Forms::SaveFileDialog ^  dlgExportFile;
	private: System::Windows::Forms::MenuItem ^  btnViewMenu;
	private: System::Windows::Forms::MenuItem ^  btnChannelMenu;
	private: System::Windows::Forms::MenuItem ^  btnChannelRGB;
	private: System::Windows::Forms::MenuItem ^  btnChannelR;
	private: System::Windows::Forms::MenuItem ^  btnChannelG;
	private: System::Windows::Forms::MenuItem ^  btnChannelB;
	private: System::Windows::Forms::MenuItem ^  btnChannelA;
	private: System::Windows::Forms::TabControl ^  tabSidebar;
	private: System::Windows::Forms::TabControl ^  tabSidebarRight;
	private: System::Windows::Forms::TabPage ^  tabImage;
	private: System::Windows::Forms::TabPage ^  tabInfo;
	private: System::Windows::Forms::Label ^  lblImageFrames;
	private: System::Windows::Forms::Label ^  lblImageFramesLabel;
	private: System::Windows::Forms::Label ^  lblImageFaces;
	private: System::Windows::Forms::Label ^  lblImageFacesLabel;
	private: System::Windows::Forms::Label ^  lblImageMipmaps;
	private: System::Windows::Forms::Label ^  lblImageMipmapsLabel;
	private: System::Windows::Forms::Label ^  lblImageStartFrameLabel;
	private: System::Windows::Forms::Label ^  lblImageBumpmapScaleLabel;
	private: System::Windows::Forms::Label ^  lblImageReflectivity;
	private: System::Windows::Forms::Label ^  lblImageReflectivityLabel;
	private: System::Windows::Forms::MenuItem ^  btnEditMenu;
	private: System::Windows::Forms::MenuItem ^  btnCopy;
	private: System::Windows::Forms::Panel ^  pnlMain;
	private: System::Windows::Forms::SaveFileDialog ^  dlgSaveVTFFile;
	private: System::Windows::Forms::SaveFileDialog ^  dlgSaveVMTFile;
	private: System::Windows::Forms::MenuItem ^  btnFileSpace2;
	private: System::Windows::Forms::MenuItem ^  btnSave;
	private: System::Windows::Forms::MenuItem ^  btnSaveAs;
	private: System::Windows::Forms::MenuItem ^  btnFileMenu;
	private: System::Windows::Forms::MenuItem ^  btnHelpMenu;
	private: System::Windows::Forms::MenuItem ^  btnAbout;
	private: System::Windows::Forms::GroupBox ^  grpImageInfo;
	private: System::Windows::Forms::Label ^  lblImageWidthLabel;
	private: System::Windows::Forms::Label ^  lblImageWidth;
	private: System::Windows::Forms::Label ^  lblImageHeight;
	private: System::Windows::Forms::Label ^  lblImageHeightLabel;
	private: System::Windows::Forms::Label ^  lblImageFormat;
	private: System::Windows::Forms::Label ^  lblImageFormatLabel;
	private: System::Windows::Forms::GroupBox ^  grpThumbnailInfo;
	private: System::Windows::Forms::Label ^  lblThumbnailFormat;
	private: System::Windows::Forms::Label ^  lblThumbnailFormatLabel;
	private: System::Windows::Forms::Label ^  lblThumbnailHeight;
	private: System::Windows::Forms::Label ^  lblThumbnailHeightLabel;
	private: System::Windows::Forms::Label ^  lblThumbnailWidth;
	private: System::Windows::Forms::Label ^  lblThumbnailWidthLabel;
	private: System::Windows::Forms::Button ^  btnAnimate;
	private: System::Windows::Forms::Label ^  lblAnimateFPS;
	private: System::Windows::Forms::NumericUpDown ^  numAnimateFPS;
	private: System::Windows::Forms::Timer ^  tmrAnimate;
	private: System::Windows::Forms::GroupBox ^  grpImage;
	private: System::Windows::Forms::GroupBox ^  grpFlags;
	private: System::Windows::Forms::CheckedListBox ^  lstFlags;
	private: System::Windows::Forms::MainMenu ^  mnuMain;
	private: System::Windows::Forms::MenuItem ^  btnOpen;
	private: System::Windows::Forms::MenuItem ^  btnExit;
	private: System::Windows::Forms::OpenFileDialog ^  dlgOpenFile;
	private: System::Windows::Forms::Panel ^  pnlSidebar;
	private: System::Windows::Forms::Panel ^  pnlSidebarRight;
	private: System::Windows::Forms::Label ^  lblFrame;
	private: System::Windows::Forms::NumericUpDown ^  numFrame;
	private: System::Windows::Forms::Label ^  lblFace;
	private: System::Windows::Forms::NumericUpDown ^  numFace;
	private: System::Windows::Forms::Label ^  lblMipmap;
	private: System::Windows::Forms::NumericUpDown ^  numMipmap;
	private: System::ComponentModel::IContainer ^  components;
	private:

		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(CVTFEdit::typeid));
			this->mnuMain = (gcnew System::Windows::Forms::MainMenu(this->components));
			this->btnFileMenu = (gcnew System::Windows::Forms::MenuItem());
			this->btnNew = (gcnew System::Windows::Forms::MenuItem());
			this->btnOpen = (gcnew System::Windows::Forms::MenuItem());
			this->btnFileSpace1 = (gcnew System::Windows::Forms::MenuItem());
			this->btnSave = (gcnew System::Windows::Forms::MenuItem());
			this->btnSaveAs = (gcnew System::Windows::Forms::MenuItem());
			this->btnFileSpace2 = (gcnew System::Windows::Forms::MenuItem());
			this->btnImport = (gcnew System::Windows::Forms::MenuItem());
			this->btnExport = (gcnew System::Windows::Forms::MenuItem());
			this->btnExportAll = (gcnew System::Windows::Forms::MenuItem());
			this->btnFileSpace3 = (gcnew System::Windows::Forms::MenuItem());
			this->btnRecentFiles = (gcnew System::Windows::Forms::MenuItem());
			this->btnFileSpace4 = (gcnew System::Windows::Forms::MenuItem());
			this->btnExit = (gcnew System::Windows::Forms::MenuItem());
			this->btnEditMenu = (gcnew System::Windows::Forms::MenuItem());
			this->btnCopy = (gcnew System::Windows::Forms::MenuItem());
			this->btnPaste = (gcnew System::Windows::Forms::MenuItem());
			this->btnViewMenu = (gcnew System::Windows::Forms::MenuItem());
			this->btnChannelMenu = (gcnew System::Windows::Forms::MenuItem());
			this->btnChannelRGB = (gcnew System::Windows::Forms::MenuItem());
			this->btnChannelR = (gcnew System::Windows::Forms::MenuItem());
			this->btnChannelG = (gcnew System::Windows::Forms::MenuItem());
			this->btnChannelB = (gcnew System::Windows::Forms::MenuItem());
			this->btnChannelA = (gcnew System::Windows::Forms::MenuItem());
			this->btnMask = (gcnew System::Windows::Forms::MenuItem());
			this->btnTile = (gcnew System::Windows::Forms::MenuItem());
			this->btnMipmapFullSize = (gcnew System::Windows::Forms::MenuItem());
			this->btnToolsMenu =(gcnew System::Windows::Forms::MenuItem());
			this->btnCreateVMTFile = (gcnew System::Windows::Forms::MenuItem());
			this->btnConvertFolder = (gcnew System::Windows::Forms::MenuItem());
			this->btnOptionsMenu = (gcnew System::Windows::Forms::MenuItem());
			this->btnAutoCreateVMTFile = (gcnew System::Windows::Forms::MenuItem());
			this->btnHelpMenu = (gcnew System::Windows::Forms::MenuItem());
			this->btnAbout = (gcnew System::Windows::Forms::MenuItem());
			this->dlgOpenFile = (gcnew System::Windows::Forms::OpenFileDialog());
			this->barStatus = (gcnew System::Windows::Forms::StatusBar());
			this->pnlFileName = (gcnew System::Windows::Forms::StatusBarPanel());
			this->pnlInfo1 = (gcnew System::Windows::Forms::StatusBarPanel());
			this->pnlInfo2 = (gcnew System::Windows::Forms::StatusBarPanel());
			this->pnlSidebar = (gcnew System::Windows::Forms::Panel());
			this->tabSidebar = (gcnew System::Windows::Forms::TabControl());
			this->pnlSidebarRight = (gcnew System::Windows::Forms::Panel());
			this->tabSidebarRight = (gcnew System::Windows::Forms::TabControl());
			this->tabImage = (gcnew System::Windows::Forms::TabPage());
			this->grpImage = (gcnew System::Windows::Forms::GroupBox());
			this->mnuHDR = (gcnew System::Windows::Forms::ContextMenu());
			this->btnHDRReset = (gcnew System::Windows::Forms::MenuItem());
			this->lblHDRKey = (gcnew System::Windows::Forms::Label());
			this->trkHDRExposure = (gcnew System::Windows::Forms::TrackBar());
			this->lblSlice = (gcnew System::Windows::Forms::Label());
			this->numSlice = (gcnew System::Windows::Forms::NumericUpDown());
			this->lblMipmap = (gcnew System::Windows::Forms::Label());
			this->numMipmap = (gcnew System::Windows::Forms::NumericUpDown());
			this->numFace = (gcnew System::Windows::Forms::NumericUpDown());
			this->lblFace = (gcnew System::Windows::Forms::Label());
			this->numFrame = (gcnew System::Windows::Forms::NumericUpDown());
			this->lblFrame = (gcnew System::Windows::Forms::Label());
			this->btnAnimate = (gcnew System::Windows::Forms::Button());
			this->grpFlags = (gcnew System::Windows::Forms::GroupBox());
			this->lstFlags = (gcnew System::Windows::Forms::CheckedListBox());
			this->tabInfo = (gcnew System::Windows::Forms::TabPage());
			this->grpImageInfo = (gcnew System::Windows::Forms::GroupBox());
			this->lblImageSlices = (gcnew System::Windows::Forms::Label());
			this->lblImageSlicesLabel = (gcnew System::Windows::Forms::Label());
			this->numImageBumpmapScale = (gcnew System::Windows::Forms::NumericUpDown());
			this->numImageStartFrame = (gcnew System::Windows::Forms::NumericUpDown());
			this->lblImageReflectivity = (gcnew System::Windows::Forms::Label());
			this->lblImageReflectivityLabel = (gcnew System::Windows::Forms::Label());
			this->lblImageBumpmapScaleLabel = (gcnew System::Windows::Forms::Label());
			this->lblImageStartFrameLabel = (gcnew System::Windows::Forms::Label());
			this->lblImageMipmaps = (gcnew System::Windows::Forms::Label());
			this->lblImageMipmapsLabel = (gcnew System::Windows::Forms::Label());
			this->lblImageFaces = (gcnew System::Windows::Forms::Label());
			this->lblImageFacesLabel = (gcnew System::Windows::Forms::Label());
			this->lblImageFrames = (gcnew System::Windows::Forms::Label());
			this->lblImageFramesLabel = (gcnew System::Windows::Forms::Label());
			this->lblImageFormat = (gcnew System::Windows::Forms::Label());
			this->lblImageFormatLabel = (gcnew System::Windows::Forms::Label());
			this->lblImageHeight = (gcnew System::Windows::Forms::Label());
			this->lblImageHeightLabel = (gcnew System::Windows::Forms::Label());
			this->lblImageWidth = (gcnew System::Windows::Forms::Label());
			this->lblImageWidthLabel = (gcnew System::Windows::Forms::Label());
			this->grpThumbnailInfo = (gcnew System::Windows::Forms::GroupBox());
			this->lblThumbnailFormat = (gcnew System::Windows::Forms::Label());
			this->lblThumbnailFormatLabel = (gcnew System::Windows::Forms::Label());
			this->lblThumbnailHeight = (gcnew System::Windows::Forms::Label());
			this->lblThumbnailHeightLabel = (gcnew System::Windows::Forms::Label());
			this->lblThumbnailWidth = (gcnew System::Windows::Forms::Label());
			this->lblThumbnailWidthLabel = (gcnew System::Windows::Forms::Label());
			this->grpFileInfo = (gcnew System::Windows::Forms::GroupBox());
			this->lblFileSize = (gcnew System::Windows::Forms::Label());
			this->lblFileSizeLabel = (gcnew System::Windows::Forms::Label());
			this->lblFileVersion = (gcnew System::Windows::Forms::Label());
			this->lblFileVersionLabel = (gcnew System::Windows::Forms::Label());
			this->lblFileCompression = (gcnew System::Windows::Forms::Label());
			this->lblFileCompressionLabel = (gcnew System::Windows::Forms::Label());
			this->tabResources = (gcnew System::Windows::Forms::TabPage());
			this->grpResources = (gcnew System::Windows::Forms::GroupBox());
			this->treResources = (gcnew System::Windows::Forms::TreeView());
			this->grpResourceInfo = (gcnew System::Windows::Forms::GroupBox());
			this->lblResourceCount = (gcnew System::Windows::Forms::Label());
			this->lblResourceCountLabel = (gcnew System::Windows::Forms::Label());
			this->lblAnimateFPS = (gcnew System::Windows::Forms::Label());
			this->numAnimateFPS = (gcnew System::Windows::Forms::NumericUpDown());
			this->tmrAnimate = (gcnew System::Windows::Forms::Timer(this->components));
			this->dlgSaveVTFFile = (gcnew System::Windows::Forms::SaveFileDialog());
			this->dlgSaveVMTFile = (gcnew System::Windows::Forms::SaveFileDialog());
			this->pnlMain = (gcnew System::Windows::Forms::Panel());
			this->picVTFFileBR = (gcnew System::Windows::Forms::PictureBox());
			this->mnuVTFFile = (gcnew System::Windows::Forms::ContextMenu());
			this->btnVTFFileZoomIn = (gcnew System::Windows::Forms::MenuItem());
			this->btnVTFFileZoomOut = (gcnew System::Windows::Forms::MenuItem());
			this->btnVTFFileSpace1 = (gcnew System::Windows::Forms::MenuItem());
			this->btnVTFFileZoomReset = (gcnew System::Windows::Forms::MenuItem());
			this->btnVTFFileSpace2 = (gcnew System::Windows::Forms::MenuItem());
			this->btnVTFFileCopy = (gcnew System::Windows::Forms::MenuItem());
			this->picVTFFileBL = (gcnew System::Windows::Forms::PictureBox());
			this->picVTFFileTR = (gcnew System::Windows::Forms::PictureBox());
			this->picVTFFileTL = (gcnew System::Windows::Forms::PictureBox());
			this->txtVMTFile = (gcnew System::Windows::Forms::RichTextBox());
			this->mnuVMTFile = (gcnew System::Windows::Forms::ContextMenu());
			this->btnVMTFileUndo = (gcnew System::Windows::Forms::MenuItem());
			this->btnVMTFileSpace1 = (gcnew System::Windows::Forms::MenuItem());
			this->btnVMTFileCut = (gcnew System::Windows::Forms::MenuItem());
			this->btnVMTFileCopy = (gcnew System::Windows::Forms::MenuItem());
			this->btnVMTFilePaste = (gcnew System::Windows::Forms::MenuItem());
			this->btnVMTFileDelete = (gcnew System::Windows::Forms::MenuItem());
			this->btnVMTFileSpace2 = (gcnew System::Windows::Forms::MenuItem());
			this->btnVMTFileSelectAll = (gcnew System::Windows::Forms::MenuItem());
			this->btnVMTFileSpace3 = (gcnew System::Windows::Forms::MenuItem());
			this->btnVMTFileValidate = (gcnew System::Windows::Forms::MenuItem());
			this->btnVMTFileValidateLoose = (gcnew System::Windows::Forms::MenuItem());
			this->btnVMTFileValidateStrict = (gcnew System::Windows::Forms::MenuItem());
			this->dlgImportFile = (gcnew System::Windows::Forms::OpenFileDialog());
			this->dlgExportFile = (gcnew System::Windows::Forms::SaveFileDialog());
			this->barTool = (gcnew System::Windows::Forms::ToolBar());
			this->btnToolImport = (gcnew System::Windows::Forms::ToolBarButton());
			this->btnToolOpen = (gcnew System::Windows::Forms::ToolBarButton());
			this->btnToolSave = (gcnew System::Windows::Forms::ToolBarButton());
			this->btnSeparator1 = (gcnew System::Windows::Forms::ToolBarButton());
			this->btnToolCopy = (gcnew System::Windows::Forms::ToolBarButton());
			this->btnToolPaste = (gcnew System::Windows::Forms::ToolBarButton());
			this->imgTool = (gcnew System::Windows::Forms::ImageList(this->components));
			this->splSidebar = (gcnew System::Windows::Forms::Splitter());
			this->splSidebarRight = (gcnew System::Windows::Forms::Splitter());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pnlFileName))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pnlInfo1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pnlInfo2))->BeginInit();
			this->pnlSidebar->SuspendLayout();
			this->tabSidebar->SuspendLayout();
			this->pnlSidebarRight->SuspendLayout();
			this->tabSidebarRight->SuspendLayout();
			this->tabImage->SuspendLayout();
			this->grpImage->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trkHDRExposure))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numSlice))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numMipmap))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numFace))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numFrame))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numAnimateFPS))->BeginInit();
			this->grpFlags->SuspendLayout();
			this->tabInfo->SuspendLayout();
			this->grpImageInfo->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numImageBumpmapScale))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numImageStartFrame))->BeginInit();
			this->grpThumbnailInfo->SuspendLayout();
			this->grpFileInfo->SuspendLayout();
			this->tabResources->SuspendLayout();
			this->grpResources->SuspendLayout();
			this->grpResourceInfo->SuspendLayout();
			this->pnlMain->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picVTFFileBR))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picVTFFileBL))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picVTFFileTR))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picVTFFileTL))->BeginInit();
			this->SuspendLayout();
			// 
			// mnuMain
			// 
			this->mnuMain->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(6) {
				this->btnFileMenu, this->btnEditMenu,
					this->btnViewMenu, this->btnToolsMenu, this->btnOptionsMenu, this->btnHelpMenu
			});
			// 
			// btnFileMenu
			// 
			this->btnFileMenu->Index = 0;
			this->btnFileMenu->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(13) {
				this->btnNew, this->btnOpen,
					this->btnFileSpace1, this->btnSave, this->btnSaveAs, this->btnFileSpace2, this->btnImport, this->btnExport, this->btnExportAll,
					this->btnFileSpace3, this->btnRecentFiles, this->btnFileSpace4, this->btnExit
			});
			this->btnFileMenu->Text = L"&File";
			// 
			// btnNew
			// 
			this->btnNew->Index = 0;
			this->btnNew->Shortcut = System::Windows::Forms::Shortcut::CtrlN;
			this->btnNew->Text = L"&New";
			this->btnNew->Click += gcnew System::EventHandler(this, &CVTFEdit::btnNew_Click);
			// 
			// btnOpen
			// 
			this->btnOpen->Index = 1;
			this->btnOpen->Shortcut = System::Windows::Forms::Shortcut::CtrlO;
			this->btnOpen->Text = L"&Open";
			this->btnOpen->Click += gcnew System::EventHandler(this, &CVTFEdit::btnOpen_Click);
			// 
			// btnFileSpace1
			// 
			this->btnFileSpace1->Index = 2;
			this->btnFileSpace1->Text = L"-";
			// 
			// btnSave
			// 
			this->btnSave->Enabled = false;
			this->btnSave->Index = 3;
			this->btnSave->Shortcut = System::Windows::Forms::Shortcut::CtrlS;
			this->btnSave->Text = L"&Save";
			this->btnSave->Click += gcnew System::EventHandler(this, &CVTFEdit::btnSave_Click);
			// 
			// btnSaveAs
			// 
			this->btnSaveAs->Enabled = false;
			this->btnSaveAs->Index = 4;
			this->btnSaveAs->Shortcut = System::Windows::Forms::Shortcut::CtrlShiftS;
			this->btnSaveAs->Text = L"Save &As...";
			this->btnSaveAs->Click += gcnew System::EventHandler(this, &CVTFEdit::btnSaveAs_Click);
			// 
			// btnFileSpace2
			// 
			this->btnFileSpace2->Index = 5;
			this->btnFileSpace2->Text = L"-";
			// 
			// btnImport
			// 
			this->btnImport->Index = 6;
			this->btnImport->Shortcut = System::Windows::Forms::Shortcut::CtrlI;
			this->btnImport->Text = L"&Import";
			this->btnImport->Click += gcnew System::EventHandler(this, &CVTFEdit::btnImport_Click);
			// 
			// btnExport
			// 
			this->btnExport->Enabled = false;
			this->btnExport->Index = 7;
			this->btnExport->Shortcut = System::Windows::Forms::Shortcut::CtrlE;
			this->btnExport->Text = L"&Export";
			this->btnExport->Click += gcnew System::EventHandler(this, &CVTFEdit::btnExport_Click);
			// 
			// btnExportAll
			// 
			this->btnExportAll->Enabled = false;
			this->btnExportAll->Index = 8;
			this->btnExportAll->Shortcut = System::Windows::Forms::Shortcut::CtrlShiftE;
			this->btnExportAll->Text = L"&Export All";
			this->btnExportAll->Click += gcnew System::EventHandler(this, &CVTFEdit::btnExportAll_Click);
			// 
			// btnFileSpace3
			// 
			this->btnFileSpace3->Index = 9;
			this->btnFileSpace3->Text = L"-";
			// 
			// btnRecentFiles
			// 
			this->btnRecentFiles->Index = 10;
			this->btnRecentFiles->Text = L"&Recent Files";
			this->btnRecentFiles->Visible = false;
			// 
			// btnFileSpace4
			// 
			this->btnFileSpace4->Index = 11;
			this->btnFileSpace4->Text = L"-";
			this->btnFileSpace4->Visible = false;
			// 
			// btnExit
			// 
			this->btnExit->Index = 12;
			this->btnExit->Shortcut = System::Windows::Forms::Shortcut::AltF4;
			this->btnExit->Text = L"E&xit";
			this->btnExit->Click += gcnew System::EventHandler(this, &CVTFEdit::btnExit_Click);
			// 
			// btnEditMenu
			// 
			this->btnEditMenu->Index = 1;
			this->btnEditMenu->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(2) { this->btnCopy, this->btnPaste });
			this->btnEditMenu->Text = L"&Edit";
			// 
			// btnCopy
			// 
			this->btnCopy->Enabled = false;
			this->btnCopy->Index = 0;
			this->btnCopy->Shortcut = System::Windows::Forms::Shortcut::CtrlC;
			this->btnCopy->Text = L"&Copy";
			this->btnCopy->Click += gcnew System::EventHandler(this, &CVTFEdit::btnCopy_Click);
			// 
			// btnPaste
			// 
			this->btnPaste->Enabled = false;
			this->btnPaste->Index = 1;
			this->btnPaste->Shortcut = System::Windows::Forms::Shortcut::CtrlV;
			this->btnPaste->Text = L"&Paste";
			this->btnPaste->Click += gcnew System::EventHandler(this, &CVTFEdit::btnPaste_Click);
			// 
			// btnViewMenu
			// 
			this->btnViewMenu->Index = 2;
			this->btnViewMenu->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(4) {
				this->btnChannelMenu, this->btnMask,
					this->btnTile, this->btnMipmapFullSize
			});
			this->btnViewMenu->Text = L"&View";
			// 
			// btnChannelMenu
			// 
			this->btnChannelMenu->Index = 0;
			this->btnChannelMenu->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(5) {
				this->btnChannelRGB,
					this->btnChannelR, this->btnChannelG, this->btnChannelB, this->btnChannelA
			});
			this->btnChannelMenu->Text = L"&Channel";
			// 
			// btnChannelRGB
			// 
			this->btnChannelRGB->Checked = true;
			this->btnChannelRGB->Index = 0;
			this->btnChannelRGB->RadioCheck = true;
			this->btnChannelRGB->Shortcut = System::Windows::Forms::Shortcut::CtrlShiftC;
			this->btnChannelRGB->Text = L"RGB";
			this->btnChannelRGB->Click += gcnew System::EventHandler(this, &CVTFEdit::btnChannel_Click);
			// 
			// btnChannelR
			// 
			this->btnChannelR->Index = 1;
			this->btnChannelR->RadioCheck = true;
			this->btnChannelR->Shortcut = System::Windows::Forms::Shortcut::CtrlShiftR;
			this->btnChannelR->Text = L"R";
			this->btnChannelR->Click += gcnew System::EventHandler(this, &CVTFEdit::btnChannel_Click);
			// 
			// btnChannelG
			// 
			this->btnChannelG->Index = 2;
			this->btnChannelG->RadioCheck = true;
			this->btnChannelG->Shortcut = System::Windows::Forms::Shortcut::CtrlShiftG;
			this->btnChannelG->Text = L"G";
			this->btnChannelG->Click += gcnew System::EventHandler(this, &CVTFEdit::btnChannel_Click);
			// 
			// btnChannelB
			// 
			this->btnChannelB->Index = 3;
			this->btnChannelB->RadioCheck = true;
			this->btnChannelB->Shortcut = System::Windows::Forms::Shortcut::CtrlShiftB;
			this->btnChannelB->Text = L"B";
			this->btnChannelB->Click += gcnew System::EventHandler(this, &CVTFEdit::btnChannel_Click);
			// 
			// btnChannelA
			// 
			this->btnChannelA->Index = 4;
			this->btnChannelA->RadioCheck = true;
			this->btnChannelA->Shortcut = System::Windows::Forms::Shortcut::CtrlShiftA;
			this->btnChannelA->Text = L"A";
			this->btnChannelA->Click += gcnew System::EventHandler(this, &CVTFEdit::btnChannel_Click);
			// 
			// btnMask
			// 
			this->btnMask->Index = 1;
			this->btnMask->Shortcut = System::Windows::Forms::Shortcut::CtrlM;
			this->btnMask->Text = L"&Mask";
			this->btnMask->Click += gcnew System::EventHandler(this, &CVTFEdit::btnMask_Click);
			// 
			// btnTile
			// 
			this->btnTile->Index = 2;
			this->btnTile->Shortcut = System::Windows::Forms::Shortcut::CtrlT;
			this->btnTile->Text = L"&Tile";
			this->btnTile->Click += gcnew System::EventHandler(this, &CVTFEdit::btnTile_Click);
			//
			// btnMipmapFullSize
			//
			this->btnMipmapFullSize->Checked = true;
			this->btnMipmapFullSize->Index = 3;
			this->btnMipmapFullSize->Shortcut = System::Windows::Forms::Shortcut::CtrlShiftM;
			this->btnMipmapFullSize->Text = L"&Zoom Mipmaps";
			this->btnMipmapFullSize->Click += gcnew System::EventHandler(this, &CVTFEdit::btnMipmapFullSize_Click);
			//
			// btnToolsMenu
			// 
			this->btnToolsMenu->Index = 3;
			this->btnToolsMenu->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(2) {
				this->btnCreateVMTFile,
					this->btnConvertFolder
			});
			this->btnToolsMenu->Text = L"&Tools";
			// 
			// btnCreateVMTFile
			// 
			this->btnCreateVMTFile->Index = 0;
			this->btnCreateVMTFile->Text = L"Create &VMT File";
			this->btnCreateVMTFile->Click += gcnew System::EventHandler(this, &CVTFEdit::btnCreateVMTFile_Click);
			// 
			// btnConvertFolder
			// 
			this->btnConvertFolder->Index = 1;
			this->btnConvertFolder->Text = L"Convert &Folder";
			this->btnConvertFolder->Click += gcnew System::EventHandler(this, &CVTFEdit::btnConvertFolder_Click);
			// 
			// btnOptionsMenu
			// 
			this->btnOptionsMenu->Index = 4;
			this->btnOptionsMenu->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(1) { this->btnAutoCreateVMTFile });
			this->btnOptionsMenu->Text = L"&Options";
			// 
			// btnAutoCreateVMTFile
			// 
			this->btnAutoCreateVMTFile->Index = 0;
			this->btnAutoCreateVMTFile->Text = L"&Auto Create VMT File";
			this->btnAutoCreateVMTFile->Click += gcnew System::EventHandler(this, &CVTFEdit::btnAutoCreateVMTFile_Click);
			// 
			// btnHelpMenu
			// 
			this->btnHelpMenu->Index = 5;
			this->btnHelpMenu->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(1) { this->btnAbout });
			this->btnHelpMenu->Text = L"&Help";
			// 
			// btnAbout
			// 
			this->btnAbout->Index = 0;
			this->btnAbout->Text = L"&About";
			this->btnAbout->Click += gcnew System::EventHandler(this, &CVTFEdit::btnAbout_Click);
			// 
			// dlgOpenFile
			// 
			this->dlgOpenFile->Filter = L"Supported Files (*.vmt;*.vtf)|*.vmt;*.vtf|VMT Files (*.vmt)|*.vmt|VTF File (*.vtf"
				L")|*.vtf";
			this->dlgOpenFile->Title = L"Open File";
			// 
			// barStatus
			// 
			this->barStatus->Location = System::Drawing::Point(0, 564);
			this->barStatus->Name = L"barStatus";
			this->barStatus->Panels->AddRange(gcnew cli::array< System::Windows::Forms::StatusBarPanel^  >(3) {
				this->pnlFileName, this->pnlInfo1,
					this->pnlInfo2
			});
			this->barStatus->ShowPanels = true;
			this->barStatus->Size = System::Drawing::Size(752, 21);
			this->barStatus->TabIndex = 3;
			// 
			// pnlFileName
			// 
			this->pnlFileName->AutoSize = System::Windows::Forms::StatusBarPanelAutoSize::Spring;
			this->pnlFileName->MinWidth = 100;
			this->pnlFileName->Name = L"pnlFileName";
			this->pnlFileName->Width = 581;
			// 
			// pnlInfo1
			// 
			this->pnlInfo1->AutoSize = System::Windows::Forms::StatusBarPanelAutoSize::Contents;
			this->pnlInfo1->MinWidth = 75;
			this->pnlInfo1->Name = L"pnlInfo1";
			this->pnlInfo1->Width = 75;
			// 
			// pnlInfo2
			// 
			this->pnlInfo2->AutoSize = System::Windows::Forms::StatusBarPanelAutoSize::Contents;
			this->pnlInfo2->MinWidth = 75;
			this->pnlInfo2->Name = L"pnlInfo2";
			this->pnlInfo2->Width = 75;
			// 
			// pnlSidebar
			// 
			this->pnlSidebar->BackColor = System::Drawing::SystemColors::Control;
			this->pnlSidebar->Controls->Add(this->tabSidebar);
			this->pnlSidebar->Dock = System::Windows::Forms::DockStyle::Left;
			this->pnlSidebar->Location = System::Drawing::Point(0, 36);
			this->pnlSidebar->Name = L"pnlSidebar";
			this->pnlSidebar->Size = System::Drawing::Size(258, 528);
			this->pnlSidebar->TabIndex = 0;
			// 
			// tabSidebar
			// 
			this->tabSidebar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabSidebar->Controls->Add(this->tabImage);
			this->tabSidebar->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->tabSidebar->Location = System::Drawing::Point(8, 8);
			this->tabSidebar->Name = L"tabSidebar";
			this->tabSidebar->SelectedIndex = 0;
			this->tabSidebar->Size = System::Drawing::Size(244, 520);
			this->tabSidebar->TabIndex = 2;
			//
			// pnlSidebarRight
			//
			this->pnlSidebarRight->BackColor = System::Drawing::SystemColors::Control;
			this->pnlSidebarRight->Controls->Add(this->tabSidebarRight);
			this->pnlSidebarRight->Dock = System::Windows::Forms::DockStyle::Right;
			this->pnlSidebarRight->Location = System::Drawing::Point(494, 36);
			this->pnlSidebarRight->Name = L"pnlSidebarRight";
			this->pnlSidebarRight->Size = System::Drawing::Size(258, 528);
			this->pnlSidebarRight->TabIndex = 6;
			//
			// tabSidebarRight
			//
			this->tabSidebarRight->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabSidebarRight->Controls->Add(this->tabInfo);
			this->tabSidebarRight->Controls->Add(this->tabResources);
			this->tabSidebarRight->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->tabSidebarRight->Location = System::Drawing::Point(6, 8);
			this->tabSidebarRight->Name = L"tabSidebarRight";
			this->tabSidebarRight->SelectedIndex = 0;
			this->tabSidebarRight->Size = System::Drawing::Size(244, 520);
			this->tabSidebarRight->TabIndex = 0;
			//
			// tabImage
			// 
			this->tabImage->Controls->Add(this->grpImage);
			this->tabImage->Controls->Add(this->grpFlags);
			this->tabImage->Location = System::Drawing::Point(4, 25);
			this->tabImage->Name = L"tabImage";
			this->tabImage->Size = System::Drawing::Size(236, 479);
			this->tabImage->TabIndex = 0;
			this->tabImage->Text = L"Image";
			// 
			// grpImage
			// 
			this->grpImage->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->grpImage->Controls->Add(this->lblHDRKey);
			this->grpImage->Controls->Add(this->trkHDRExposure);
			this->grpImage->Controls->Add(this->lblSlice);
			this->grpImage->Controls->Add(this->numSlice);
			this->grpImage->Controls->Add(this->lblMipmap);
			this->grpImage->Controls->Add(this->numMipmap);
			this->grpImage->Controls->Add(this->numFace);
			this->grpImage->Controls->Add(this->lblFace);
			this->grpImage->Controls->Add(this->numFrame);
			this->grpImage->Controls->Add(this->lblFrame);
			this->grpImage->Controls->Add(this->btnAnimate);
			this->grpImage->Controls->Add(this->lblAnimateFPS);
			this->grpImage->Controls->Add(this->numAnimateFPS);
			this->grpImage->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpImage->Location = System::Drawing::Point(8, 8);
			this->grpImage->Name = L"grpImage";
			this->grpImage->Size = System::Drawing::Size(225, 201);
			this->grpImage->TabIndex = 0;
			this->grpImage->TabStop = false;
			this->grpImage->Text = L"Image:";
			// 
			// mnuHDR
			// 
			this->mnuHDR->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(1) { this->btnHDRReset });
			// 
			// btnHDRReset
			// 
			this->btnHDRReset->Index = 0;
			this->btnHDRReset->Text = L"&Reset";
			this->btnHDRReset->Click += gcnew System::EventHandler(this, &CVTFEdit::btnHDRReset_Click);
			// 
			// lblHDRKey
			// 
			this->lblHDRKey->Location = System::Drawing::Point(8, 116);
			this->lblHDRKey->Name = L"lblHDRKey";
			this->lblHDRKey->Size = System::Drawing::Size(72, 23);
			this->lblHDRKey->TabIndex = 8;
			this->lblHDRKey->Text = L"Exposure:";
			// 
			// trkHDRExposure
			// 
			this->trkHDRExposure->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->trkHDRExposure->AutoSize = false;
			this->trkHDRExposure->ContextMenu = this->mnuHDR;
			this->trkHDRExposure->Location = System::Drawing::Point(82, 116);
			this->trkHDRExposure->Name = L"trkHDRExposure";
			this->trkHDRExposure->Size = System::Drawing::Size(123, 20);
			this->trkHDRExposure->TabIndex = 9;
			this->trkHDRExposure->ValueChanged += gcnew System::EventHandler(this, &CVTFEdit::trkHDRKey_ValueChanged);
			// 
			// lblSlice
			// 
			this->lblSlice->Location = System::Drawing::Point(8, 70);
			this->lblSlice->Name = L"lblSlice";
			this->lblSlice->Size = System::Drawing::Size(56, 23);
			this->lblSlice->TabIndex = 4;
			this->lblSlice->Text = L"Slice:";
			// 
			// numSlice
			// 
			this->numSlice->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->numSlice->Location = System::Drawing::Point(82, 70);
			this->numSlice->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 0, 0, 0, 0 });
			this->numSlice->Name = L"numSlice";
			this->numSlice->Size = System::Drawing::Size(137, 23);
			this->numSlice->TabIndex = 5;
			this->numSlice->ValueChanged += gcnew System::EventHandler(this, &CVTFEdit::numVTFFile_ValueChanged);
			// 
			// lblMipmap
			// 
			this->lblMipmap->Location = System::Drawing::Point(8, 93);
			this->lblMipmap->Name = L"lblMipmap";
			this->lblMipmap->Size = System::Drawing::Size(72, 22);
			this->lblMipmap->TabIndex = 6;
			this->lblMipmap->Text = L"Mipmap:";
			// 
			// numMipmap
			// 
			this->numMipmap->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->numMipmap->Location = System::Drawing::Point(82, 93);
			this->numMipmap->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 0, 0, 0, 0 });
			this->numMipmap->Name = L"numMipmap";
			this->numMipmap->Size = System::Drawing::Size(137, 23);
			this->numMipmap->TabIndex = 7;
			this->numMipmap->ValueChanged += gcnew System::EventHandler(this, &CVTFEdit::numVTFFile_ValueChanged);
			// 
			// numFace
			// 
			this->numFace->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->numFace->Location = System::Drawing::Point(82, 46);
			this->numFace->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 0, 0, 0, 0 });
			this->numFace->Name = L"numFace";
			this->numFace->Size = System::Drawing::Size(137, 23);
			this->numFace->TabIndex = 3;
			this->numFace->ValueChanged += gcnew System::EventHandler(this, &CVTFEdit::numVTFFile_ValueChanged);
			// 
			// lblFace
			// 
			this->lblFace->Location = System::Drawing::Point(8, 46);
			this->lblFace->Name = L"lblFace";
			this->lblFace->Size = System::Drawing::Size(72, 23);
			this->lblFace->TabIndex = 2;
			this->lblFace->Text = L"Face:";
			// 
			// numFrame
			// 
			this->numFrame->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->numFrame->Location = System::Drawing::Point(82, 24);
			this->numFrame->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 0, 0, 0, 0 });
			this->numFrame->Name = L"numFrame";
			this->numFrame->Size = System::Drawing::Size(137, 23);
			this->numFrame->TabIndex = 1;
			this->numFrame->ValueChanged += gcnew System::EventHandler(this, &CVTFEdit::numVTFFile_ValueChanged);
			// 
			// lblFrame
			// 
			this->lblFrame->Location = System::Drawing::Point(8, 24);
			this->lblFrame->Name = L"lblFrame";
			this->lblFrame->Size = System::Drawing::Size(42, 21);
			this->lblFrame->TabIndex = 0;
			this->lblFrame->Text = L"Frame:";
			// 
			// btnAnimate
			// 
			this->btnAnimate->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->btnAnimate->Enabled = false;
			this->btnAnimate->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnAnimate->Location = System::Drawing::Point(11, 142);
			this->btnAnimate->Name = L"btnAnimate";
			this->btnAnimate->Size = System::Drawing::Size(194, 22);
			this->btnAnimate->TabIndex = 14;
			this->btnAnimate->Click += gcnew System::EventHandler(this, &CVTFEdit::btnAnimate_Click);
			//
			// lblAnimateFPS
			//
			this->lblAnimateFPS->Location = System::Drawing::Point(8, 170);
			this->lblAnimateFPS->Name = L"lblAnimateFPS";
			this->lblAnimateFPS->Size = System::Drawing::Size(72, 23);
			this->lblAnimateFPS->TabIndex = 15;
			this->lblAnimateFPS->Text = L"Framerate:";
			//
			// numAnimateFPS
			//
			this->numAnimateFPS->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->numAnimateFPS->Enabled = false;
			this->numAnimateFPS->Location = System::Drawing::Point(82, 170);
			this->numAnimateFPS->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numAnimateFPS->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 100, 0, 0, 0 });
			this->numAnimateFPS->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 24, 0, 0, 0 });
			this->numAnimateFPS->Name = L"numAnimateFPS";
			this->numAnimateFPS->Size = System::Drawing::Size(137, 23);
			this->numAnimateFPS->TabIndex = 16;
			this->numAnimateFPS->ValueChanged += gcnew System::EventHandler(this, &CVTFEdit::numAnimateFPS_ValueChanged);
			//
			// grpFlags
			// 
			this->grpFlags->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->grpFlags->Controls->Add(this->lstFlags);
			this->grpFlags->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpFlags->Location = System::Drawing::Point(8, 206);
			this->grpFlags->Name = L"grpFlags";
			this->grpFlags->Size = System::Drawing::Size(219, 270);
			this->grpFlags->TabIndex = 1;
			this->grpFlags->TabStop = false;
			this->grpFlags->Text = L"Flags:";
			// 
			// lstFlags
			// 
			this->lstFlags->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lstFlags->CheckOnClick = true;
			this->lstFlags->Location = System::Drawing::Point(8, 15);
			this->lstFlags->Name = L"lstFlags";
			this->lstFlags->Size = System::Drawing::Size(191, 274);
			this->lstFlags->TabIndex = 3;
			this->lstFlags->ItemCheck += gcnew System::Windows::Forms::ItemCheckEventHandler(this, &CVTFEdit::lstFlags_ItemCheck);
			// 
			// tabInfo
			// 
			this->tabInfo->Controls->Add(this->grpImageInfo);
			this->tabInfo->Controls->Add(this->grpThumbnailInfo);
			this->tabInfo->Controls->Add(this->grpFileInfo);
			this->tabInfo->Location = System::Drawing::Point(4, 25);
			this->tabInfo->Name = L"tabInfo";
			this->tabInfo->Size = System::Drawing::Size(218, 479);
			this->tabInfo->TabIndex = 1;
			this->tabInfo->Text = L"Info";
			// 
			// grpImageInfo
			// 
			this->grpImageInfo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->grpImageInfo->Controls->Add(this->lblImageSlices);
			this->grpImageInfo->Controls->Add(this->lblImageSlicesLabel);
			this->grpImageInfo->Controls->Add(this->numImageBumpmapScale);
			this->grpImageInfo->Controls->Add(this->numImageStartFrame);
			this->grpImageInfo->Controls->Add(this->lblImageReflectivity);
			this->grpImageInfo->Controls->Add(this->lblImageReflectivityLabel);
			this->grpImageInfo->Controls->Add(this->lblImageBumpmapScaleLabel);
			this->grpImageInfo->Controls->Add(this->lblImageStartFrameLabel);
			this->grpImageInfo->Controls->Add(this->lblImageMipmaps);
			this->grpImageInfo->Controls->Add(this->lblImageMipmapsLabel);
			this->grpImageInfo->Controls->Add(this->lblImageFaces);
			this->grpImageInfo->Controls->Add(this->lblImageFacesLabel);
			this->grpImageInfo->Controls->Add(this->lblImageFrames);
			this->grpImageInfo->Controls->Add(this->lblImageFramesLabel);
			this->grpImageInfo->Controls->Add(this->lblImageFormat);
			this->grpImageInfo->Controls->Add(this->lblImageFormatLabel);
			this->grpImageInfo->Controls->Add(this->lblImageHeight);
			this->grpImageInfo->Controls->Add(this->lblImageHeightLabel);
			this->grpImageInfo->Controls->Add(this->lblImageWidth);
			this->grpImageInfo->Controls->Add(this->lblImageWidthLabel);
			this->grpImageInfo->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpImageInfo->Location = System::Drawing::Point(8, 115);
			this->grpImageInfo->Name = L"grpImageInfo";
			this->grpImageInfo->Size = System::Drawing::Size(204, 265);
			this->grpImageInfo->TabIndex = 1;
			this->grpImageInfo->TabStop = false;
			this->grpImageInfo->Text = L"Image Info:";
			// 
			// lblImageSlices
			// 
			this->lblImageSlices->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblImageSlices->Location = System::Drawing::Point(64, 163);
			this->lblImageSlices->Name = L"lblImageSlices";
			this->lblImageSlices->Size = System::Drawing::Size(134, 22);
			this->lblImageSlices->TabIndex = 13;
			// 
			// lblImageSlicesLabel
			// 
			this->lblImageSlicesLabel->Location = System::Drawing::Point(8, 163);
			this->lblImageSlicesLabel->Name = L"lblImageSlicesLabel";
			this->lblImageSlicesLabel->Size = System::Drawing::Size(56, 22);
			this->lblImageSlicesLabel->TabIndex = 12;
			this->lblImageSlicesLabel->Text = L"Slices:";
			// 
			// numImageBumpmapScale
			// 
			this->numImageBumpmapScale->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->numImageBumpmapScale->Location = System::Drawing::Point(64, 210);
			this->numImageBumpmapScale->Name = L"numImageBumpmapScale";
			this->numImageBumpmapScale->Size = System::Drawing::Size(134, 23);
			this->numImageBumpmapScale->TabIndex = 17;
			// 
			// numImageStartFrame
			// 
			this->numImageStartFrame->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->numImageStartFrame->Location = System::Drawing::Point(64, 116);
			this->numImageStartFrame->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 0, 0, 0, 0 });
			this->numImageStartFrame->Name = L"numImageStartFrame";
			this->numImageStartFrame->Size = System::Drawing::Size(134, 23);
			this->numImageStartFrame->TabIndex = 9;
			// 
			// lblImageReflectivity
			// 
			this->lblImageReflectivity->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblImageReflectivity->Location = System::Drawing::Point(64, 233);
			this->lblImageReflectivity->Name = L"lblImageReflectivity";
			this->lblImageReflectivity->Size = System::Drawing::Size(134, 22);
			this->lblImageReflectivity->TabIndex = 19;
			// 
			// lblImageReflectivityLabel
			// 
			this->lblImageReflectivityLabel->Location = System::Drawing::Point(8, 233);
			this->lblImageReflectivityLabel->Name = L"lblImageReflectivityLabel";
			this->lblImageReflectivityLabel->Size = System::Drawing::Size(56, 22);
			this->lblImageReflectivityLabel->TabIndex = 18;
			this->lblImageReflectivityLabel->Text = L"Reflectivity:";
			// 
			// lblImageBumpmapScaleLabel
			// 
			this->lblImageBumpmapScaleLabel->Location = System::Drawing::Point(8, 210);
			this->lblImageBumpmapScaleLabel->Name = L"lblImageBumpmapScaleLabel";
			this->lblImageBumpmapScaleLabel->Size = System::Drawing::Size(56, 22);
			this->lblImageBumpmapScaleLabel->TabIndex = 16;
			this->lblImageBumpmapScaleLabel->Text = L"Bumpmap:";
			// 
			// lblImageStartFrameLabel
			// 
			this->lblImageStartFrameLabel->Location = System::Drawing::Point(8, 116);
			this->lblImageStartFrameLabel->Name = L"lblImageStartFrameLabel";
			this->lblImageStartFrameLabel->Size = System::Drawing::Size(56, 23);
			this->lblImageStartFrameLabel->TabIndex = 8;
			this->lblImageStartFrameLabel->Text = L"Start:";
			// 
			// lblImageMipmaps
			// 
			this->lblImageMipmaps->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblImageMipmaps->Location = System::Drawing::Point(64, 187);
			this->lblImageMipmaps->Name = L"lblImageMipmaps";
			this->lblImageMipmaps->Size = System::Drawing::Size(134, 22);
			this->lblImageMipmaps->TabIndex = 15;
			// 
			// lblImageMipmapsLabel
			// 
			this->lblImageMipmapsLabel->Location = System::Drawing::Point(8, 187);
			this->lblImageMipmapsLabel->Name = L"lblImageMipmapsLabel";
			this->lblImageMipmapsLabel->Size = System::Drawing::Size(56, 22);
			this->lblImageMipmapsLabel->TabIndex = 14;
			this->lblImageMipmapsLabel->Text = L"Mipmaps:";
			// 
			// lblImageFaces
			// 
			this->lblImageFaces->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblImageFaces->Location = System::Drawing::Point(64, 140);
			this->lblImageFaces->Name = L"lblImageFaces";
			this->lblImageFaces->Size = System::Drawing::Size(134, 23);
			this->lblImageFaces->TabIndex = 11;
			// 
			// lblImageFacesLabel
			// 
			this->lblImageFacesLabel->Location = System::Drawing::Point(8, 140);
			this->lblImageFacesLabel->Name = L"lblImageFacesLabel";
			this->lblImageFacesLabel->Size = System::Drawing::Size(56, 23);
			this->lblImageFacesLabel->TabIndex = 10;
			this->lblImageFacesLabel->Text = L"Faces:";
			// 
			// lblImageFrames
			// 
			this->lblImageFrames->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblImageFrames->Location = System::Drawing::Point(64, 93);
			this->lblImageFrames->Name = L"lblImageFrames";
			this->lblImageFrames->Size = System::Drawing::Size(134, 22);
			this->lblImageFrames->TabIndex = 7;
			// 
			// lblImageFramesLabel
			// 
			this->lblImageFramesLabel->Location = System::Drawing::Point(8, 93);
			this->lblImageFramesLabel->Name = L"lblImageFramesLabel";
			this->lblImageFramesLabel->Size = System::Drawing::Size(56, 22);
			this->lblImageFramesLabel->TabIndex = 6;
			this->lblImageFramesLabel->Text = L"Frames:";
			// 
			// lblImageFormat
			// 
			this->lblImageFormat->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblImageFormat->Location = System::Drawing::Point(64, 70);
			this->lblImageFormat->Name = L"lblImageFormat";
			this->lblImageFormat->Size = System::Drawing::Size(134, 23);
			this->lblImageFormat->TabIndex = 5;
			// 
			// lblImageFormatLabel
			// 
			this->lblImageFormatLabel->Location = System::Drawing::Point(8, 70);
			this->lblImageFormatLabel->Name = L"lblImageFormatLabel";
			this->lblImageFormatLabel->Size = System::Drawing::Size(56, 23);
			this->lblImageFormatLabel->TabIndex = 4;
			this->lblImageFormatLabel->Text = L"Format:";
			// 
			// lblImageHeight
			// 
			this->lblImageHeight->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblImageHeight->Location = System::Drawing::Point(64, 46);
			this->lblImageHeight->Name = L"lblImageHeight";
			this->lblImageHeight->Size = System::Drawing::Size(134, 23);
			this->lblImageHeight->TabIndex = 3;
			// 
			// lblImageHeightLabel
			// 
			this->lblImageHeightLabel->Location = System::Drawing::Point(8, 46);
			this->lblImageHeightLabel->Name = L"lblImageHeightLabel";
			this->lblImageHeightLabel->Size = System::Drawing::Size(56, 23);
			this->lblImageHeightLabel->TabIndex = 2;
			this->lblImageHeightLabel->Text = L"Height:";
			// 
			// lblImageWidth
			// 
			this->lblImageWidth->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblImageWidth->Location = System::Drawing::Point(64, 24);
			this->lblImageWidth->Name = L"lblImageWidth";
			this->lblImageWidth->Size = System::Drawing::Size(134, 21);
			this->lblImageWidth->TabIndex = 1;
			// 
			// lblImageWidthLabel
			// 
			this->lblImageWidthLabel->Location = System::Drawing::Point(8, 24);
			this->lblImageWidthLabel->Name = L"lblImageWidthLabel";
			this->lblImageWidthLabel->Size = System::Drawing::Size(56, 21);
			this->lblImageWidthLabel->TabIndex = 0;
			this->lblImageWidthLabel->Text = L"Width:";
			// 
			// grpThumbnailInfo
			// 
			this->grpThumbnailInfo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->grpThumbnailInfo->Controls->Add(this->lblThumbnailFormat);
			this->grpThumbnailInfo->Controls->Add(this->lblThumbnailFormatLabel);
			this->grpThumbnailInfo->Controls->Add(this->lblThumbnailHeight);
			this->grpThumbnailInfo->Controls->Add(this->lblThumbnailHeightLabel);
			this->grpThumbnailInfo->Controls->Add(this->lblThumbnailWidth);
			this->grpThumbnailInfo->Controls->Add(this->lblThumbnailWidthLabel);
			this->grpThumbnailInfo->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpThumbnailInfo->Location = System::Drawing::Point(8, 387);
			this->grpThumbnailInfo->Name = L"grpThumbnailInfo";
			this->grpThumbnailInfo->Size = System::Drawing::Size(204, 102);
			this->grpThumbnailInfo->TabIndex = 2;
			this->grpThumbnailInfo->TabStop = false;
			this->grpThumbnailInfo->Text = L"Thumbnail Info:";
			// 
			// lblThumbnailFormat
			// 
			this->lblThumbnailFormat->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblThumbnailFormat->Location = System::Drawing::Point(64, 70);
			this->lblThumbnailFormat->Name = L"lblThumbnailFormat";
			this->lblThumbnailFormat->Size = System::Drawing::Size(134, 23);
			this->lblThumbnailFormat->TabIndex = 5;
			// 
			// lblThumbnailFormatLabel
			// 
			this->lblThumbnailFormatLabel->Location = System::Drawing::Point(8, 70);
			this->lblThumbnailFormatLabel->Name = L"lblThumbnailFormatLabel";
			this->lblThumbnailFormatLabel->Size = System::Drawing::Size(56, 23);
			this->lblThumbnailFormatLabel->TabIndex = 4;
			this->lblThumbnailFormatLabel->Text = L"Format:";
			// 
			// lblThumbnailHeight
			// 
			this->lblThumbnailHeight->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblThumbnailHeight->Location = System::Drawing::Point(64, 46);
			this->lblThumbnailHeight->Name = L"lblThumbnailHeight";
			this->lblThumbnailHeight->Size = System::Drawing::Size(134, 23);
			this->lblThumbnailHeight->TabIndex = 3;
			// 
			// lblThumbnailHeightLabel
			// 
			this->lblThumbnailHeightLabel->Location = System::Drawing::Point(8, 46);
			this->lblThumbnailHeightLabel->Name = L"lblThumbnailHeightLabel";
			this->lblThumbnailHeightLabel->Size = System::Drawing::Size(56, 23);
			this->lblThumbnailHeightLabel->TabIndex = 2;
			this->lblThumbnailHeightLabel->Text = L"Height:";
			// 
			// lblThumbnailWidth
			// 
			this->lblThumbnailWidth->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblThumbnailWidth->Location = System::Drawing::Point(64, 24);
			this->lblThumbnailWidth->Name = L"lblThumbnailWidth";
			this->lblThumbnailWidth->Size = System::Drawing::Size(134, 21);
			this->lblThumbnailWidth->TabIndex = 1;
			// 
			// lblThumbnailWidthLabel
			// 
			this->lblThumbnailWidthLabel->Location = System::Drawing::Point(8, 24);
			this->lblThumbnailWidthLabel->Name = L"lblThumbnailWidthLabel";
			this->lblThumbnailWidthLabel->Size = System::Drawing::Size(56, 21);
			this->lblThumbnailWidthLabel->TabIndex = 0;
			this->lblThumbnailWidthLabel->Text = L"Width:";
			// 
			// grpFileInfo
			// 
			this->grpFileInfo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->grpFileInfo->Controls->Add(this->lblFileCompression);
			this->grpFileInfo->Controls->Add(this->lblFileCompressionLabel);
			this->grpFileInfo->Controls->Add(this->lblFileSize);
			this->grpFileInfo->Controls->Add(this->lblFileSizeLabel);
			this->grpFileInfo->Controls->Add(this->lblFileVersion);
			this->grpFileInfo->Controls->Add(this->lblFileVersionLabel);
			this->grpFileInfo->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpFileInfo->Location = System::Drawing::Point(8, 8);
			this->grpFileInfo->Name = L"grpFileInfo";
			this->grpFileInfo->Size = System::Drawing::Size(204, 100);
			this->grpFileInfo->TabIndex = 0;
			this->grpFileInfo->TabStop = false;
			this->grpFileInfo->Text = L"File Info:";
			// 
			// lblFileSize
			// 
			this->lblFileSize->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblFileSize->Location = System::Drawing::Point(64, 46);
			this->lblFileSize->Name = L"lblFileSize";
			this->lblFileSize->Size = System::Drawing::Size(134, 23);
			this->lblFileSize->TabIndex = 3;
			// 
			// lblFileSizeLabel
			// 
			this->lblFileSizeLabel->Location = System::Drawing::Point(8, 46);
			this->lblFileSizeLabel->Name = L"lblFileSizeLabel";
			this->lblFileSizeLabel->Size = System::Drawing::Size(56, 23);
			this->lblFileSizeLabel->TabIndex = 2;
			this->lblFileSizeLabel->Text = L"Size:";
			// 
			// lblFileCompression
			// 
			this->lblFileCompression->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblFileCompression->Location = System::Drawing::Point(64, 68);
			this->lblFileCompression->Name = L"lblFileCompression";
			this->lblFileCompression->Size = System::Drawing::Size(134, 23);
			this->lblFileCompression->TabIndex = 5;
			// 
			// lblFileCompressionLabel
			// 
			this->lblFileCompressionLabel->Location = System::Drawing::Point(8, 68);
			this->lblFileCompressionLabel->Name = L"lblFileCompressionLabel";
			this->lblFileCompressionLabel->Size = System::Drawing::Size(56, 23);
			this->lblFileCompressionLabel->TabIndex = 4;
			this->lblFileCompressionLabel->Text = L"Compression:";
			// 
			// lblFileVersion
			// 
			this->lblFileVersion->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblFileVersion->Location = System::Drawing::Point(64, 24);
			this->lblFileVersion->Name = L"lblFileVersion";
			this->lblFileVersion->Size = System::Drawing::Size(134, 21);
			this->lblFileVersion->TabIndex = 1;
			// 
			// lblFileVersionLabel
			// 
			this->lblFileVersionLabel->Location = System::Drawing::Point(8, 24);
			this->lblFileVersionLabel->Name = L"lblFileVersionLabel";
			this->lblFileVersionLabel->Size = System::Drawing::Size(56, 21);
			this->lblFileVersionLabel->TabIndex = 0;
			this->lblFileVersionLabel->Text = L"Version:";
			// 
			// tabResources
			// 
			this->tabResources->Controls->Add(this->grpResources);
			this->tabResources->Controls->Add(this->grpResourceInfo);
			this->tabResources->Location = System::Drawing::Point(4, 25);
			this->tabResources->Name = L"tabResources";
			this->tabResources->Size = System::Drawing::Size(218, 479);
			this->tabResources->TabIndex = 3;
			this->tabResources->Text = L"Resources";
			// 
			// grpResources
			// 
			this->grpResources->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->grpResources->Controls->Add(this->treResources);
			this->grpResources->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpResources->Location = System::Drawing::Point(8, 70);
			this->grpResources->Name = L"grpResources";
			this->grpResources->Size = System::Drawing::Size(204, 401);
			this->grpResources->TabIndex = 2;
			this->grpResources->TabStop = false;
			this->grpResources->Text = L"Resources:";
			// 
			// treResources
			// 
			this->treResources->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->treResources->Location = System::Drawing::Point(8, 15);
			this->treResources->Name = L"treResources";
			this->treResources->Size = System::Drawing::Size(190, 378);
			this->treResources->TabIndex = 0;
			// 
			// grpResourceInfo
			// 
			this->grpResourceInfo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->grpResourceInfo->Controls->Add(this->lblResourceCount);
			this->grpResourceInfo->Controls->Add(this->lblResourceCountLabel);
			this->grpResourceInfo->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->grpResourceInfo->Location = System::Drawing::Point(8, 8);
			this->grpResourceInfo->Name = L"grpResourceInfo";
			this->grpResourceInfo->Size = System::Drawing::Size(204, 54);
			this->grpResourceInfo->TabIndex = 1;
			this->grpResourceInfo->TabStop = false;
			this->grpResourceInfo->Text = L"Resource Info:";
			// 
			// lblResourceCount
			// 
			this->lblResourceCount->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->lblResourceCount->Location = System::Drawing::Point(64, 24);
			this->lblResourceCount->Name = L"lblResourceCount";
			this->lblResourceCount->Size = System::Drawing::Size(134, 21);
			this->lblResourceCount->TabIndex = 1;
			// 
			// lblResourceCountLabel
			// 
			this->lblResourceCountLabel->Location = System::Drawing::Point(8, 24);
			this->lblResourceCountLabel->Name = L"lblResourceCountLabel";
			this->lblResourceCountLabel->Size = System::Drawing::Size(56, 21);
			this->lblResourceCountLabel->TabIndex = 0;
			this->lblResourceCountLabel->Text = L"Resources:";
			// 
			// tmrAnimate
			// 
			this->tmrAnimate->Interval = 41;
			this->tmrAnimate->Tick += gcnew System::EventHandler(this, &CVTFEdit::tmrAnimate_Tick);
			// 
			// dlgSaveVTFFile
			// 
			this->dlgSaveVTFFile->Filter = L"VTF Files (*.vtf)|*.vtf";
			this->dlgSaveVTFFile->Title = L"Save VTF File";
			// 
			// dlgSaveVMTFile
			// 
			this->dlgSaveVMTFile->Filter = L"VMT Files (*.vmt)|*.vmt";
			this->dlgSaveVMTFile->Title = L"Save VMT File";
			// 
			// pnlMain
			// 
			this->pnlMain->AllowDrop = true;
			this->pnlMain->AutoScroll = true;
			this->pnlMain->ContextMenu = this->mnuVTFFile;
			this->pnlMain->Controls->Add(this->picVTFFileBR);
			this->pnlMain->Controls->Add(this->picVTFFileBL);
			this->pnlMain->Controls->Add(this->picVTFFileTR);
			this->pnlMain->Controls->Add(this->picVTFFileTL);
			this->pnlMain->Controls->Add(this->txtVMTFile);
			this->pnlMain->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pnlMain->Location = System::Drawing::Point(262, 36);
			this->pnlMain->Name = L"pnlMain";
			this->pnlMain->Size = System::Drawing::Size(490, 528);
			this->pnlMain->TabIndex = 2;
			this->pnlMain->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &CVTFEdit::Control_DragDrop);
			this->pnlMain->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &CVTFEdit::Control_DragEnter);
			this->pnlMain->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseDown);
			this->pnlMain->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::pnlMain_MouseMove);
			this->pnlMain->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseUp);
			// 
			// picVTFFileBR
			// 
			this->picVTFFileBR->ContextMenu = this->mnuVTFFile;
			this->picVTFFileBR->Location = System::Drawing::Point(24, 26);
			this->picVTFFileBR->Name = L"picVTFFileBR";
			this->picVTFFileBR->Size = System::Drawing::Size(25, 26);
			this->picVTFFileBR->TabIndex = 4;
			this->picVTFFileBR->TabStop = false;
			this->picVTFFileBR->Visible = false;
			this->picVTFFileBR->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &CVTFEdit::Control_DragDrop);
			this->picVTFFileBR->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &CVTFEdit::Control_DragEnter);
			this->picVTFFileBR->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseDown);
			this->picVTFFileBR->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseMove);
			this->picVTFFileBR->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseUp);
			// 
			// mnuVTFFile
			// 
			this->mnuVTFFile->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(6) {
				this->btnVTFFileZoomIn,
					this->btnVTFFileZoomOut, this->btnVTFFileSpace1, this->btnVTFFileZoomReset, this->btnVTFFileSpace2, this->btnVTFFileCopy
			});
			// 
			// btnVTFFileZoomIn
			// 
			this->btnVTFFileZoomIn->Index = 0;
			this->btnVTFFileZoomIn->Text = L"Zoom &In";
			this->btnVTFFileZoomIn->Click += gcnew System::EventHandler(this, &CVTFEdit::btnVTFFileZoomIn_Click);
			// 
			// btnVTFFileZoomOut
			// 
			this->btnVTFFileZoomOut->Index = 1;
			this->btnVTFFileZoomOut->Text = L"Zoom &Out";
			this->btnVTFFileZoomOut->Click += gcnew System::EventHandler(this, &CVTFEdit::btnVTFFileZoomOut_Click);
			// 
			// btnVTFFileSpace1
			// 
			this->btnVTFFileSpace1->Index = 2;
			this->btnVTFFileSpace1->Text = L"-";
			// 
			// btnVTFFileZoomReset
			// 
			this->btnVTFFileZoomReset->Index = 3;
			this->btnVTFFileZoomReset->Text = L"&Reset Zoom/Pan";
			this->btnVTFFileZoomReset->Click += gcnew System::EventHandler(this, &CVTFEdit::btnVTFFileZoomReset_Click);
			// 
			// btnVTFFileSpace2
			// 
			this->btnVTFFileSpace2->Index = 4;
			this->btnVTFFileSpace2->Text = L"-";
			// 
			// btnVTFFileCopy
			// 
			this->btnVTFFileCopy->Index = 5;
			this->btnVTFFileCopy->Text = L"&Copy";
			this->btnVTFFileCopy->Click += gcnew System::EventHandler(this, &CVTFEdit::btnVTFFileCopy_Click);
			// 
			// picVTFFileBL
			// 
			this->picVTFFileBL->ContextMenu = this->mnuVTFFile;
			this->picVTFFileBL->Location = System::Drawing::Point(0, 26);
			this->picVTFFileBL->Name = L"picVTFFileBL";
			this->picVTFFileBL->Size = System::Drawing::Size(24, 26);
			this->picVTFFileBL->TabIndex = 3;
			this->picVTFFileBL->TabStop = false;
			this->picVTFFileBL->Visible = false;
			this->picVTFFileBL->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &CVTFEdit::Control_DragDrop);
			this->picVTFFileBL->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &CVTFEdit::Control_DragEnter);
			this->picVTFFileBL->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseDown);
			this->picVTFFileBL->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseMove);
			this->picVTFFileBL->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseUp);
			// 
			// picVTFFileTR
			// 
			this->picVTFFileTR->ContextMenu = this->mnuVTFFile;
			this->picVTFFileTR->Location = System::Drawing::Point(24, 0);
			this->picVTFFileTR->Name = L"picVTFFileTR";
			this->picVTFFileTR->Size = System::Drawing::Size(25, 26);
			this->picVTFFileTR->TabIndex = 2;
			this->picVTFFileTR->TabStop = false;
			this->picVTFFileTR->Visible = false;
			this->picVTFFileTR->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &CVTFEdit::Control_DragDrop);
			this->picVTFFileTR->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &CVTFEdit::Control_DragEnter);
			this->picVTFFileTR->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseDown);
			this->picVTFFileTR->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseMove);
			this->picVTFFileTR->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseUp);
			// 
			// picVTFFileTL
			// 
			this->picVTFFileTL->ContextMenu = this->mnuVTFFile;
			this->picVTFFileTL->Location = System::Drawing::Point(0, 0);
			this->picVTFFileTL->Name = L"picVTFFileTL";
			this->picVTFFileTL->Size = System::Drawing::Size(24, 26);
			this->picVTFFileTL->TabIndex = 0;
			this->picVTFFileTL->TabStop = false;
			this->picVTFFileTL->Visible = false;
			this->picVTFFileTL->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &CVTFEdit::Control_DragDrop);
			this->picVTFFileTL->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &CVTFEdit::Control_DragEnter);
			this->picVTFFileTL->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseDown);
			this->picVTFFileTL->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseMove);
			this->picVTFFileTL->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &CVTFEdit::picVTFFile_MouseUp);
			// 
			// txtVMTFile
			// 
			this->txtVMTFile->AcceptsTab = true;
			this->txtVMTFile->AllowDrop = true;
			this->txtVMTFile->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(29)), static_cast<System::Int32>(static_cast<System::Byte>(31)),
				static_cast<System::Int32>(static_cast<System::Byte>(33)));
			this->txtVMTFile->ContextMenu = this->mnuVMTFile;
			this->txtVMTFile->DetectUrls = false;
			this->txtVMTFile->Dock = System::Windows::Forms::DockStyle::Fill;
			this->txtVMTFile->Font = (gcnew System::Drawing::Font(L"Consolas", 10));
			this->txtVMTFile->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(197)), static_cast<System::Int32>(static_cast<System::Byte>(200)),
				static_cast<System::Int32>(static_cast<System::Byte>(198)));
			this->txtVMTFile->Location = System::Drawing::Point(0, 0);
			this->txtVMTFile->Name = L"txtVMTFile";
			this->txtVMTFile->Size = System::Drawing::Size(490, 528);
			this->txtVMTFile->TabIndex = 5;
			this->txtVMTFile->Text = L"";
			this->txtVMTFile->Visible = false;
			this->txtVMTFile->WordWrap = false;
			this->txtVMTFile->SelectionChanged += gcnew System::EventHandler(this, &CVTFEdit::txtVMTFile_SelectionChanged);
			this->txtVMTFile->TextChanged += gcnew System::EventHandler(this, &CVTFEdit::txtVMTFile_TextChanged);
			// 
			// mnuVMTFile
			// 
			this->mnuVMTFile->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(10) {
				this->btnVMTFileUndo, this->btnVMTFileSpace1,
					this->btnVMTFileCut, this->btnVMTFileCopy, this->btnVMTFilePaste, this->btnVMTFileDelete, this->btnVMTFileSpace2, this->btnVMTFileSelectAll,
					this->btnVMTFileSpace3, this->btnVMTFileValidate
			});
			// 
			// btnVMTFileUndo
			// 
			this->btnVMTFileUndo->Enabled = false;
			this->btnVMTFileUndo->Index = 0;
			this->btnVMTFileUndo->Text = L"&Undo";
			this->btnVMTFileUndo->Click += gcnew System::EventHandler(this, &CVTFEdit::btnVMTFileUndo_Click);
			// 
			// btnVMTFileSpace1
			// 
			this->btnVMTFileSpace1->Index = 1;
			this->btnVMTFileSpace1->Text = L"-";
			// 
			// btnVMTFileCut
			// 
			this->btnVMTFileCut->Enabled = false;
			this->btnVMTFileCut->Index = 2;
			this->btnVMTFileCut->Text = L"&Cut";
			this->btnVMTFileCut->Click += gcnew System::EventHandler(this, &CVTFEdit::btnVMTFileCut_Click);
			// 
			// btnVMTFileCopy
			// 
			this->btnVMTFileCopy->Enabled = false;
			this->btnVMTFileCopy->Index = 3;
			this->btnVMTFileCopy->Text = L"&Copy";
			this->btnVMTFileCopy->Click += gcnew System::EventHandler(this, &CVTFEdit::btnVMTFileCopy_Click);
			// 
			// btnVMTFilePaste
			// 
			this->btnVMTFilePaste->Enabled = false;
			this->btnVMTFilePaste->Index = 4;
			this->btnVMTFilePaste->Text = L"&Paste";
			this->btnVMTFilePaste->Click += gcnew System::EventHandler(this, &CVTFEdit::btnVMTFilePaste_Click);
			// 
			// btnVMTFileDelete
			// 
			this->btnVMTFileDelete->Enabled = false;
			this->btnVMTFileDelete->Index = 5;
			this->btnVMTFileDelete->Text = L"&Delete";
			this->btnVMTFileDelete->Click += gcnew System::EventHandler(this, &CVTFEdit::btnVMTFileDelete_Click);
			// 
			// btnVMTFileSpace2
			// 
			this->btnVMTFileSpace2->Index = 6;
			this->btnVMTFileSpace2->Text = L"-";
			// 
			// btnVMTFileSelectAll
			// 
			this->btnVMTFileSelectAll->Enabled = false;
			this->btnVMTFileSelectAll->Index = 7;
			this->btnVMTFileSelectAll->Text = L"Select &All";
			this->btnVMTFileSelectAll->Click += gcnew System::EventHandler(this, &CVTFEdit::btnVMTFileSelectAll_Click);
			// 
			// btnVMTFileSpace3
			// 
			this->btnVMTFileSpace3->Index = 8;
			this->btnVMTFileSpace3->Text = L"-";
			// 
			// btnVMTFileValidate
			// 
			this->btnVMTFileValidate->Index = 9;
			this->btnVMTFileValidate->MenuItems->AddRange(gcnew cli::array< System::Windows::Forms::MenuItem^  >(2) {
				this->btnVMTFileValidateLoose,
					this->btnVMTFileValidateStrict
			});
			this->btnVMTFileValidate->Text = L"&Validate";
			// 
			// btnVMTFileValidateLoose
			// 
			this->btnVMTFileValidateLoose->Index = 0;
			this->btnVMTFileValidateLoose->Text = L"&Loose";
			this->btnVMTFileValidateLoose->Click += gcnew System::EventHandler(this, &CVTFEdit::btnVMTFileValidateLoose_Click);
			// 
			// btnVMTFileValidateStrict
			// 
			this->btnVMTFileValidateStrict->Index = 1;
			this->btnVMTFileValidateStrict->Text = L"&Strict";
			this->btnVMTFileValidateStrict->Click += gcnew System::EventHandler(this, &CVTFEdit::btnVMTFileValidateStrict_Click);
			// 
			// dlgImportFile
			// 
			this->dlgImportFile->Filter = resources->GetString(L"dlgImportFile.Filter");
			this->dlgImportFile->Multiselect = true;
			this->dlgImportFile->Title = L"Import File";
			// 
			// dlgExportFile
			// 
			this->dlgExportFile->Filter = L"BMP Files (*.bmp)|*.bmp|JPEG Files (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG Files (*.png)|"
				L"*.png|TGA Files (*.tga)|*.tga";
			this->dlgExportFile->FilterIndex = 4;
			this->dlgExportFile->Title = L"Export File";
			// 
			// barTool
			// 
			this->barTool->Buttons->AddRange(gcnew cli::array< System::Windows::Forms::ToolBarButton^  >(6) {
				this->btnToolImport, this->btnToolOpen,
					this->btnToolSave, this->btnSeparator1, this->btnToolCopy, this->btnToolPaste
			});
			this->barTool->ButtonSize = System::Drawing::Size(23, 22);
			this->barTool->DropDownArrows = true;
			this->barTool->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 32));
			this->barTool->ImageList = this->imgTool;
			this->barTool->Location = System::Drawing::Point(0, 0);
			this->barTool->Name = L"barTool";
			this->barTool->ShowToolTips = true;
			this->barTool->Size = System::Drawing::Size(752, 36);
			this->barTool->TabIndex = 4;
			this->barTool->Wrappable = false;
			this->barTool->ButtonClick += gcnew System::Windows::Forms::ToolBarButtonClickEventHandler(this, &CVTFEdit::barTool_ButtonClick);
			// 
			// btnToolImport
			// 
			this->btnToolImport->ImageIndex = 0;
			this->btnToolImport->Name = L"btnToolImport";
			this->btnToolImport->ToolTipText = L"Import";
			// 
			// btnToolOpen
			// 
			this->btnToolOpen->ImageIndex = 1;
			this->btnToolOpen->Name = L"btnToolOpen";
			this->btnToolOpen->ToolTipText = L"Open";
			// 
			// btnToolSave
			// 
			this->btnToolSave->Enabled = false;
			this->btnToolSave->ImageIndex = 2;
			this->btnToolSave->Name = L"btnToolSave";
			this->btnToolSave->ToolTipText = L"Save";
			// 
			// btnSeparator1
			// 
			this->btnSeparator1->Name = L"btnSeparator1";
			this->btnSeparator1->Style = System::Windows::Forms::ToolBarButtonStyle::Separator;
			// 
			// btnToolCopy
			// 
			this->btnToolCopy->Enabled = false;
			this->btnToolCopy->ImageIndex = 3;
			this->btnToolCopy->Name = L"btnToolCopy";
			this->btnToolCopy->ToolTipText = L"Copy";
			// 
			// btnToolPaste
			// 
			this->btnToolPaste->Enabled = false;
			this->btnToolPaste->ImageIndex = 4;
			this->btnToolPaste->Name = L"btnToolPaste";
			this->btnToolPaste->ToolTipText = L"Paste";
			// 
			// imgTool
			// 
			this->imgTool->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^>(resources->GetObject(L"imgTool.ImageStream")));
			this->imgTool->TransparentColor = System::Drawing::Color::Fuchsia;
			this->imgTool->Images->SetKeyName(0, L"");
			this->imgTool->Images->SetKeyName(1, L"");
			this->imgTool->Images->SetKeyName(2, L"");
			this->imgTool->Images->SetKeyName(3, L"");
			this->imgTool->Images->SetKeyName(4, L"");
			// 
			// splSidebar
			// 
			this->splSidebar->BackColor = System::Drawing::SystemColors::Control;
			this->splSidebar->Location = System::Drawing::Point(258, 36);
			this->splSidebar->MinExtra = 96;
			this->splSidebar->MinSize = 96;
			this->splSidebar->Name = L"splSidebar";
			this->splSidebar->Size = System::Drawing::Size(4, 528);
			this->splSidebar->TabIndex = 5;
			this->splSidebar->TabStop = false;
			this->splSidebar->SplitterMoved += gcnew System::Windows::Forms::SplitterEventHandler(this, &CVTFEdit::splSidebar_SplitterMoved);
			//
			// splSidebarRight
			//
			this->splSidebarRight->BackColor = System::Drawing::SystemColors::Control;
			this->splSidebarRight->Dock = System::Windows::Forms::DockStyle::Right;
			this->splSidebarRight->Location = System::Drawing::Point(490, 36);
			this->splSidebarRight->MinExtra = 96;
			this->splSidebarRight->MinSize = 96;
			this->splSidebarRight->Name = L"splSidebarRight";
			this->splSidebarRight->Size = System::Drawing::Size(4, 528);
			this->splSidebarRight->TabIndex = 7;
			this->splSidebarRight->TabStop = false;
			this->splSidebarRight->SplitterMoved += gcnew System::Windows::Forms::SplitterEventHandler(this, &CVTFEdit::splSidebarRight_SplitterMoved);
			//
			// CVTFEdit
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(6, 16);
			this->BackColor = System::Drawing::SystemColors::ControlDark;
			this->ClientSize = System::Drawing::Size(752, 597);
			this->Controls->Add(this->pnlMain);
			this->Controls->Add(this->splSidebar);
			this->Controls->Add(this->pnlSidebar);
			this->Controls->Add(this->splSidebarRight);
			this->Controls->Add(this->pnlSidebarRight);
			this->Controls->Add(this->barTool);
			this->Controls->Add(this->barStatus);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8));
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Menu = this->mnuMain;
			this->Name = L"CVTFEdit";
			this->Text = L"VTFEdit++";
			this->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &CVTFEdit::CVTFEdit_Closing);
			this->Load += gcnew System::EventHandler(this, &CVTFEdit::CVTFEdit_Load);
			this->Move += gcnew System::EventHandler(this, &CVTFEdit::CVTFEdit_Move);
			this->Resize += gcnew System::EventHandler(this, &CVTFEdit::CVTFEdit_Resize);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pnlFileName))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pnlInfo1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pnlInfo2))->EndInit();
			this->pnlSidebar->ResumeLayout(false);
			this->tabSidebar->ResumeLayout(false);
			this->pnlSidebarRight->ResumeLayout(false);
			this->tabSidebarRight->ResumeLayout(false);
			this->tabImage->ResumeLayout(false);
			this->grpImage->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trkHDRExposure))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numSlice))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numMipmap))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numFace))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numFrame))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numAnimateFPS))->EndInit();
			this->grpFlags->ResumeLayout(false);
			this->tabInfo->ResumeLayout(false);
			this->grpImageInfo->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numImageBumpmapScale))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numImageStartFrame))->EndInit();
			this->grpThumbnailInfo->ResumeLayout(false);
			this->grpFileInfo->ResumeLayout(false);
			this->tabResources->ResumeLayout(false);
			this->grpResources->ResumeLayout(false);
			this->grpResourceInfo->ResumeLayout(false);
			this->pnlMain->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picVTFFileBR))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picVTFFileBL))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picVTFFileTR))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picVTFFileTL))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
		//
		// CVMTTreeNode
		// VMT node tree view class.
		//
		/*private: __gc class CVMTTreeNode : public System::Windows::Forms::TreeNode
		{
		public:
			CVMTTreeNode(VTFLib::Nodes::CVMTNode ^Node) : System::Windows::Forms::TreeNode(gcnew System::String(Node->GetName()))
			{
				if(Node->GetType() == NODE_TYPE_GROUP)
				{
					VTFLib::Nodes::CVMTGroupNode ^Group = static_cast<VTFLib::Nodes::CVMTGroupNode ^>(Node);
					for(vlUInt i = 0; i < Group->GetNodeCount(); i++)
					{
						this->Nodes->Add(new CVMTTreeNode(Group->GetNode(i)));
					}
				}
				else if(Node->GetType() == NODE_TYPE_STRING)
				{
					VTFLib::Nodes::CVMTStringNode ^System::String = static_cast<VTFLib::Nodes::CVMTStringNode ^>(Node);
					this->Nodes->Add(new System::Windows::Forms::TreeNode(gcnew System::String(System::String->GetValue())));
				}
				else if(Node->GetType() == NODE_TYPE_INTEGER)
				{
					VTFLib::Nodes::CVMTIntegerNode ^Integer = static_cast<VTFLib::Nodes::CVMTIntegerNode ^>(Node);
					this->Nodes->Add(new System::Windows::Forms::TreeNode(Integer->GetValue().ToString()));
				}
				else if(Node->GetType() == NODE_TYPE_SINGLE)
				{
					VTFLib::Nodes::CVMTSingleNode ^Single = static_cast<VTFLib::Nodes::CVMTSingleNode ^>(Node);
					this->Nodes->Add(new System::Windows::Forms::TreeNode(Single->GetValue().ToString("0.00000000")));
				}
			}
		};*/

		//
		// Form events.
		//

		public: property System::String ^FileName
		{
			System::String^ get()
			{
				return this->sFileName;
			}
			void set(System::String^ sFileName)
			{
				this->sFileName = sFileName;
				this->pnlFileName->Text = this->sFileName;

				if (sFileName)
				{
					if (sFileName->LastIndexOf('\\') != -1)
					{
						this->Text = System::String::Concat(sFileName->Substring(sFileName->LastIndexOf('\\') + 1), " - ", Application::ProductName);
					}
					else
					{
						this->Text = System::String::Concat(sFileName, " - ", Application::ProductName);
					}
				}
				else
				{
					if (this->VTFFile)
					{
						this->Text = "untitled.vtf - ", Application::ProductName;
					}
					else if (this->VMTFile)
					{
						this->Text = "untitled.vmt - ", Application::ProductName;
					}
					else
					{
						this->Text = Application::ProductName;
					}
				}
			}
		}

		private: System::String^ GetAppDataFolder()
		{
			System::String ^pDirectory = Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData);
			pDirectory = IO::Path::Combine(pDirectory, "Frog Co");
			pDirectory = IO::Path::Combine(pDirectory, Application::ProductName);

			if(!IO::Directory::Exists(pDirectory))
			{
				try
				{
					IO::Directory::CreateDirectory(pDirectory);
				}
				catch(Exception ^)
				{
				}
			}

			return pDirectory;
		}

		private: System::Void CVTFEdit_Load(System::Object ^  sender, System::EventArgs ^  e)
		{
			System::Windows::Forms::Application::AddMessageFilter(this);

			this->picVTFFileTL->AllowDrop = true;
			this->picVTFFileTR->AllowDrop = true;
			this->picVTFFileBL->AllowDrop = true;
			this->picVTFFileBR->AllowDrop = true;

			this->txtVMTFile->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &CVTFEdit::Control_DragEnter);
			this->txtVMTFile->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &CVTFEdit::Control_DragDrop);

			// "Hide" the tab pages.
			if(this->tabSidebarRight->TabPages->Contains(this->tabResources))
			{
				this->tabSidebarRight->TabPages->Remove(this->tabResources);
			}
			if(this->tabSidebarRight->TabPages->Contains(this->tabInfo))
			{
				this->tabSidebarRight->TabPages->Remove(this->tabInfo);
			}
			if(this->tabSidebar->TabPages->Contains(this->tabImage))
			{
				this->tabSidebar->TabPages->Remove(this->tabImage);
			}
			this->UpdateSidebarsVisible();

			this->numImageBumpmapScale->DecimalPlaces = 2;
			this->numImageBumpmapScale->Increment = System::Decimal(0.01);
			this->numImageBumpmapScale->Minimum = System::Decimal(-100);
			this->numImageBumpmapScale->Maximum = System::Decimal(100);
			this->numImageBumpmapScale->Value = System::Decimal(0);
			this->btnHDRReset_Click(this, System::EventArgs::Empty);

			// Restore options.
			this->BackupForm();
			System::String ^pNewConfigFile = IO::Path::Combine(GetAppDataFolder(), System::String::Concat(Application::ProductName, ".ini"));
			System::String ^pOldConfigFile = IO::Path::Combine(Application::StartupPath, System::String::Concat(Application::ProductName, ".ini"));
			if(IO::File::Exists(pNewConfigFile))
			{
				this->ReadConfigFile(pNewConfigFile);
			}
			else if(IO::File::Exists(pOldConfigFile))
			{
				this->ReadConfigFile(pOldConfigFile);
			}
			this->RestoreForm();


			// If we were pased a file, load it.
			array<System::String^>^ CommandLineArgs = System::Environment::GetCommandLineArgs();

			if(CommandLineArgs->Length >= 2)
			{
				System::String ^sFilePath = CommandLineArgs[1];

				if(System::IO::File::Exists(sFilePath))
				{
					if(sFilePath->ToLower()->EndsWith(".vtf") || sFilePath->ToLower()->EndsWith(".vmt"))
					{
						this->Open(sFilePath, false);
					}
					else
					{
						array<System::String^>^ sFileNames = gcnew array<System::String^>(1);
						sFileNames[0] = sFilePath;
						this->Import(sFileNames);
					}
				}
			}

			if(System::Threading::Thread::CurrentThread->ApartmentState == System::Threading::ApartmentState::STA)
			{
				// Set intitial clipboard state.
				this->btnToolPaste->Enabled = System::Windows::Forms::Clipboard::GetDataObject()->GetDataPresent(System::Windows::Forms::DataFormats::Bitmap);
				this->btnPaste->Enabled = this->btnToolPaste->Enabled;

				// Add ourselves to clipboard notification chain.
				if(this->hWndNewViewer == 0)
				{
					this->hWndNewViewer = SetClipboardViewer((HWND)this->Handle.ToPointer());
				}
			}
			else
			{	// Set intitial clipboard state.
				this->btnToolPaste->Enabled = true;
				this->btnPaste->Enabled = this->btnToolPaste->Enabled;
			}

		}

		private: System::Void CVTFEdit_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e)
		{
			//Causes crah when more than once instance of VTFEdit is open.  WM_DESTROY is all that is needed?
			//ChangeClipboardChain((HWND)this->Handle.ToPointer(), this->hWndNewViewer);

			System::Windows::Forms::Application::RemoveMessageFilter(this);

			System::String ^pNewConfigFile = IO::Path::Combine(GetAppDataFolder(), System::String::Concat(Application::ProductName, ".ini"));
			System::String ^pOldConfigFile = IO::Path::Combine(Application::StartupPath, System::String::Concat(Application::ProductName, ".ini"));
			if(this->WriteConfigFile(pNewConfigFile))
			{
				if(IO::File::Exists(pOldConfigFile))
				{
					try
					{
						IO::File::Delete(pOldConfigFile);
					}
					catch(Exception ^)
					{
					}
				}
			}
		}

		private: System::Void CVTFEdit_Move(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->BackupForm();
		}

		private: System::Void CVTFEdit_Resize(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->BackupForm();
		}

		private: System::Void splSidebar_SplitterMoved(System::Object ^  sender, System::Windows::Forms::SplitterEventArgs ^  e)
		{
			this->BackupForm();
		}

		private: System::Void splSidebarRight_SplitterMoved(System::Object ^  sender, System::Windows::Forms::SplitterEventArgs ^  e)
		{
			this->BackupForm();
		}

		private: void UpdateSidebarVisible(System::Windows::Forms::TabControl ^Tabs, System::Windows::Forms::Panel ^Panel, System::Windows::Forms::Splitter ^Splitter, int iSplitPosition)
		{
			bool bVisible = Tabs->TabPages->Count != 0;

			if(bVisible && !Panel->Visible)
			{
				Panel->Visible = true;
				Splitter->Visible = true;

				try
				{
					Splitter->SplitPosition = iSplitPosition;
				}
				catch(Exception ^)
				{

				}
			}
			else if(!bVisible)
			{
				Panel->Visible = false;
				Splitter->Visible = false;
			}
		}

		private: void UpdateSidebarsVisible()
		{
			this->UpdateSidebarVisible(this->tabSidebar, this->pnlSidebar, this->splSidebar, this->iFormSaveSidebarSplitPosition);
			this->UpdateSidebarVisible(this->tabSidebarRight, this->pnlSidebarRight, this->splSidebarRight, this->iFormSaveSidebarRightSplitPosition);
		}

		private: void BackupForm()
		{
			if(!this->bFormRestoring && this->WindowState != FormWindowState::Minimized)
			{
				if(this->WindowState != FormWindowState::Maximized)
				{
					this->FormSaveLocation = Location;
					this->FormSaveSize = Size;
					if(this->pnlSidebar->Visible)
					{
						this->iFormSaveSidebarSplitPosition = this->splSidebar->SplitPosition;
					}
					if(this->pnlSidebarRight->Visible)
					{
						this->iFormSaveSidebarRightSplitPosition = this->splSidebarRight->SplitPosition;
					}
				}
				this->FormSaveWindowState = this->WindowState;
			}
		}

		private: void RestoreForm()
		{
			this->bFormRestoring = true;
			try
			{
				this->Location = this->FormSaveLocation;
				this->Size = this->FormSaveSize;
				this->WindowState = this->FormSaveWindowState;
				if(this->pnlSidebar->Visible)
				{
					this->splSidebar->SplitPosition = this->iFormSaveSidebarSplitPosition;
				}
				if(this->pnlSidebarRight->Visible)
				{
					this->splSidebarRight->SplitPosition = this->iFormSaveSidebarRightSplitPosition;
				}
			}
			catch(Exception ^)
			{

			}
			this->bFormRestoring = false;
		}

		//
		// WndProc()
		// Override default WndProc to process clipboard messages.
		//
		protected: System::Void WndProc(Message %e) override
		{
			if(System::Threading::Thread::CurrentThread->ApartmentState == System::Threading::ApartmentState::STA)
			{
				switch(e.Msg)
				{
				case WM_CHANGECBCHAIN:
					// Store the changed handle of the next item in the clipboard chain.
					this->hWndNewViewer = (HWND)e.LParam.ToPointer();

					if(this->hWndNewViewer != 0)
					{
						::SendMessage(this->hWndNewViewer, e.Msg, (WPARAM)e.WParam.ToPointer(), (LPARAM)e.LParam.ToPointer());
					}

					// We have processed this message.
					e.Result = System::IntPtr::Zero;
					break;
				case WM_DRAWCLIPBOARD:
				{
					try
					{
						this->btnPaste->Enabled = System::Windows::Forms::Clipboard::GetDataObject()->GetDataPresent(System::Windows::Forms::DataFormats::Bitmap);
					}
					catch(Exception ^)
					{
						this->btnPaste->Enabled = false;
					}
					this->btnToolPaste->Enabled = this->btnPaste->Enabled;

					this->btnVMTFilePaste->Enabled = false;
					try
					{
						array<System::String^>^ Formats = System::Windows::Forms::Clipboard::GetDataObject()->GetFormats();
						for(int i = 0; i < Formats->Length; i++)
						{
							if(System::Windows::Forms::DataFormats::GetFormat(Formats[i])->Name->Equals("Text"))
							{
								this->btnVMTFilePaste->Enabled = true;
								break;
							}
						}
					}
					catch(Exception ^)
					{
						this->btnVMTFilePaste->Enabled = false;
					}

					// Pass the message on.
					if(this->hWndNewViewer != 0)
					{
						::SendMessage(this->hWndNewViewer, e.Msg, (WPARAM)e.WParam.ToPointer(), (LPARAM)e.LParam.ToPointer());
					}

					// We have processed this message.
					e.Result = System::IntPtr::Zero;
					break;
				}
				case WM_DESTROY:
					ChangeClipboardChain((HWND)this->Handle.ToPointer(), this->hWndNewViewer);

					__super::WndProc(e);
					break;
				default:
					__super::WndProc(e);
					break;
				}
			}
			else
			{
				__super::WndProc(e);
			}
		}

		//
		// VTF and VMT routines.
		//

		private: void UpdateVTFFile()
		{
			if(this->VTFFile == 0)
				return;

			static bool bUpdating = false;
			if(bUpdating)
				return;

			bUpdating = true;

			this->picVTFFileTL->Visible = true;
			this->picVTFFileTR->Visible = this->btnTile->Checked;
			this->picVTFFileBL->Visible = this->btnTile->Checked;
			this->picVTFFileBR->Visible = this->btnTile->Checked;

			this->picVTFFileTL->Image = nullptr;
			this->picVTFFileTR->Image = nullptr;
			this->picVTFFileBL->Image = nullptr;
			this->picVTFFileBR->Image = nullptr;

			delete []this->ucImageData;
			this->ucImageData = 0;

			vlUInt uiFrame = (vlUInt)Convert::ToUInt32(this->numFrame->Value);
			vlUInt uiFace = (vlUInt)Convert::ToUInt32(this->numFace->Value);
			vlUInt uiSlice = (vlUInt)Convert::ToUInt32(this->numSlice->Value);
			vlUInt uiMipmap = (vlUInt)Convert::ToUInt32(this->numMipmap->Value);
			vlSingle sHDRExposure = (vlSingle)Convert::ToSingle(this->trkHDRExposure->Value) / 100.0f;

			vlUInt uiWidth = 0;
			vlUInt uiHeight = 0;
			vlUInt uiDepth = 0;

			vlUInt uiScaledWidth = 0;
			vlUInt uiScaledHeight = 0;

			vlByte *lpBuffer = nullptr;
			vlUInt uiBufferSize = 0;
			
			this->VTFFile->ComputeMipmapDimensions(this->VTFFile->GetWidth(), this->VTFFile->GetHeight(), this->VTFFile->GetDepth(), uiMipmap, uiWidth, uiHeight, uiDepth);

			if(uiSlice >= uiDepth)
				uiSlice = uiDepth - 1;

			this->numSlice->Value = uiSlice;
			this->numSlice->Maximum = uiDepth;

			float fMipmapScale = 1.0f;

			if(this->btnMipmapFullSize->Checked)
			{
				fMipmapScale = (float)(1 << uiMipmap);
			}

			float fScale = this->fImageScale * fMipmapScale;

			// Don't let the rescaled image get larger than 4096x4096.  .NET crashes...
			while(true)
			{
				uiScaledWidth = (vlUInt)((float)uiWidth * fScale);
				uiScaledHeight = (vlUInt)((float)uiHeight * fScale);

				if(uiScaledWidth <= 4096 && uiScaledHeight <= 4096)
				{
					break;
				}

				this->fImageScale *= 0.5f;
				fScale = this->fImageScale * fMipmapScale;
			}

			// Don't let it get smaller than 1 either.
			if(uiScaledWidth < 1)
				uiScaledWidth = 1;

			if(uiScaledHeight < 1)
				uiScaledHeight = 1;

			// Allocate buffer to decode image data to.
			uiBufferSize = this->VTFFile->ComputeImageSize(uiWidth, uiHeight, 1, IMAGE_FORMAT_RGBA8888);
			
			lpBuffer = new vlByte[uiBufferSize];

			// Decode image data.
			vlSetFloat(VTFLIB_FP16_HDR_EXPOSURE, sHDRExposure);
			this->VTFFile->ConvertToRGBA8888(this->VTFFile->GetData(uiFrame, uiFace, uiSlice, uiMipmap), lpBuffer, uiWidth, uiHeight, this->VTFFile->GetFormat());

			this->fEffectiveImageScale = fScale;

			float fInverseImageScale = 1.0f / fScale;

			vlUInt uiScaledStride = (uiScaledWidth + 3) / 4 * 4;

			// Allocate buffer for bitmap image data.
			this->ucImageData = new unsigned char[uiScaledStride * uiScaledHeight * 3];

			vlUInt uiR, uiG, uiB;

			// Pick R, G, B and A indicies.
			if(this->btnChannelRGB->Checked)
			{
				uiR = 0;
				uiG = 1;
				uiB = 2;
			}
			else if(this->btnChannelR->Checked)
			{
				uiR = 0;
				uiG = 0;
				uiB = 0;
			}
			else if(this->btnChannelG->Checked)
			{
				uiR = 1;
				uiG = 1;
				uiB = 1;
			}
			else if(this->btnChannelB->Checked)
			{
				uiR = 2;
				uiG = 2;
				uiB = 2;
			}
			else if(this->btnChannelA->Checked)
			{
				uiR = 3;
				uiG = 3;
				uiB = 3;
			}

			if(this->btnMask->Checked)// && (this->VTFFile->GetFlag(TEXTUREFLAGS_ONEBITALPHA) || this->VTFFile->GetFlag(TEXTUREFLAGS_EIGHTBITALPHA)))
			{
				// Generate checkered mask and alpha blend texture with it.
				for(vlUInt i = 0; i < uiScaledWidth; i++)
				{
					for(vlUInt j = 0; j < uiScaledHeight; j++)
					{
						vlUInt uiSrcIndex = ((vlUInt)((float)i * fInverseImageScale) + (vlUInt)((float)j * fInverseImageScale) * uiWidth) * 4;
						vlUInt uiDstIndex = (i + j * uiScaledStride) * 3;

						float fAlpha = (float)lpBuffer[uiSrcIndex + 3] / (float)255.0f;
						float fOneMinusAlpha = 1.0f - fAlpha;
						float fBlend = (i / 8 % 2 == j / 8 % 2) ? 255.0f : 191.25f;	// Blend color.

						this->ucImageData[uiDstIndex + 2] = (unsigned char)(fAlpha * (float)lpBuffer[uiSrcIndex + uiR] + fOneMinusAlpha * fBlend);
						this->ucImageData[uiDstIndex + 1] = (unsigned char)(fAlpha * (float)lpBuffer[uiSrcIndex + uiG] + fOneMinusAlpha * fBlend);
						this->ucImageData[uiDstIndex + 0] = (unsigned char)(fAlpha * (float)lpBuffer[uiSrcIndex + uiB] + fOneMinusAlpha * fBlend);
					}
				}
			}
			else
			{
				// Don't generate checkered mask.
				for(vlUInt i = 0; i < uiScaledWidth; i++)
				{
					for(vlUInt j = 0; j < uiScaledHeight; j++)
					{
						vlUInt uiSrcIndex = ((vlUInt)((float)i * fInverseImageScale) + (vlUInt)((float)j * fInverseImageScale) * uiWidth) * 4;
						vlUInt uiDstIndex = (i + j * uiScaledStride) * 3;

						this->ucImageData[uiDstIndex + 2] = lpBuffer[uiSrcIndex + uiR];
						this->ucImageData[uiDstIndex + 1] = lpBuffer[uiSrcIndex + uiG];
						this->ucImageData[uiDstIndex + 0] = lpBuffer[uiSrcIndex + uiB];
					}
				}
			}

			delete []lpBuffer;

			this->picVTFFileTL->Size = System::Drawing::Size(uiScaledWidth, uiScaledHeight);
			this->picVTFFileTL->Image = gcnew System::Drawing::Bitmap(uiScaledWidth, uiScaledHeight, uiScaledStride * 3, System::Drawing::Imaging::PixelFormat::Format24bppRgb, (System::IntPtr)this->ucImageData);

			this->picVTFFileTL->Visible = true;

			if(this->btnTile->Checked)
			{
				this->picVTFFileTR->Location = System::Drawing::Point(this->picVTFFileTL->Location.X + uiScaledWidth, this->picVTFFileTL->Location.Y);
				this->picVTFFileTR->Size = System::Drawing::Size(uiScaledWidth, uiScaledHeight);
				this->picVTFFileTR->Image = gcnew System::Drawing::Bitmap(uiScaledWidth, uiScaledHeight, uiScaledStride * 3, System::Drawing::Imaging::PixelFormat::Format24bppRgb, (System::IntPtr)this->ucImageData);

				this->picVTFFileBL->Location = System::Drawing::Point(this->picVTFFileTL->Location.X, this->picVTFFileTL->Location.Y + uiScaledHeight);
				this->picVTFFileBL->Size = System::Drawing::Size(uiScaledWidth, uiScaledHeight);
				this->picVTFFileBL->Image = gcnew System::Drawing::Bitmap(uiScaledWidth, uiScaledHeight, uiScaledStride * 3, System::Drawing::Imaging::PixelFormat::Format24bppRgb, (System::IntPtr)this->ucImageData);

				this->picVTFFileBR->Location = System::Drawing::Point(this->picVTFFileTL->Location.X + uiScaledWidth, this->picVTFFileTL->Location.Y + uiScaledHeight);
				this->picVTFFileBR->Size = System::Drawing::Size(uiScaledWidth, uiScaledHeight);
				this->picVTFFileBR->Image = gcnew System::Drawing::Bitmap(uiScaledWidth, uiScaledHeight, uiScaledStride * 3, System::Drawing::Imaging::PixelFormat::Format24bppRgb, (System::IntPtr)this->ucImageData);

				this->picVTFFileTR->Visible = true;
				this->picVTFFileBL->Visible = true;
				this->picVTFFileBR->Visible = true;
			}

			this->pnlInfo1->Text = System::String::Concat((this->fImageScale * 100.0f).ToString(), "%");

			bUpdating = false;
		}

		//private: void UpdateVTFFileScale()
		//{
		//	if(this->VTFFile == 0)
		//		return;

		//	vlUInt uiMipmap = (vlUInt)Convert::ToUInt32(this->numMipmap->Value);

		//	vlUInt uiWidth = 0;
		//	vlUInt uiHeight = 0;

		//	this->VTFFile->ComputeMipmapDimensions(this->VTFFile->GetWidth(), this->VTFFile->GetHeight(), uiMipmap, uiWidth, uiHeight);

		//	while(true)
		//	{
		//		if((int)((float)uiWidth * this->fImageScale) > 4096 || (int)((float)uiHeight * this->fImageScale) > 4096)
		//		{
		//			this->fImageScale *= 0.5f;
		//		}
		//		else
		//		{
		//			break;
		//		}
		//	}

		//	this->picVTFFile->Size = System::Drawing::Size((int)((float)uiWidth * this->fImageScale), (int)((float)uiHeight * this->fImageScale));
		//}

		private: void SetVTFFile(VTFLib::CVTFFile *VTFFile)
		{
			this->numFrame->Value = 0;
			this->numFace->Value = 0;
			this->numSlice->Value = 0;
			this->numMipmap->Value = 0;

			this->numFrame->Maximum = VTFFile->GetFrameCount() - 1;
			this->numFace->Maximum = VTFFile->GetFaceCount() - 1;
			this->numSlice->Maximum = VTFFile->GetDepth() - 1;
			this->numMipmap->Maximum = VTFFile->GetMipmapCount() - 1;

			if(VTFFile->GetStartFrame() < VTFFile->GetFrameCount())
			{
				this->numFrame->Value = VTFFile->GetStartFrame();
			}

			if(VTFFile->GetFrameCount() > 1)
			{
				this->btnAnimate->Enabled = true;
				this->numAnimateFPS->Enabled = true;
			}

			if(VTFFile->GetFormat() == IMAGE_FORMAT_RGBA16161616F || VTFFile->GetFormat() == IMAGE_FORMAT_BC6H || VTFFile->GetFormat() == IMAGE_FORMAT_BC6H_UNSIGNED)
			{
				this->trkHDRExposure->Enabled = true;
			}

			vlUInt uiFlags = VTFFile->GetFlags();

			this->lstFlags->BeginUpdate();
			this->lstFlags->Items->Clear();

			#pragma warning(disable: 4800)
			this->lstFlags->Items->Add("Point Sample", uiFlags & TEXTUREFLAGS_POINTSAMPLE);
			this->lstFlags->Items->Add("Trilinear", uiFlags & TEXTUREFLAGS_TRILINEAR);
			this->lstFlags->Items->Add("Clamp S", uiFlags & TEXTUREFLAGS_CLAMPS);
			this->lstFlags->Items->Add("Clamp T", uiFlags & TEXTUREFLAGS_CLAMPT);
			this->lstFlags->Items->Add("Anisotropic", uiFlags & TEXTUREFLAGS_ANISOTROPIC);
			this->lstFlags->Items->Add("Hint DXT5", uiFlags & TEXTUREFLAGS_HINT_DXT5);
			this->lstFlags->Items->Add("SRGB", uiFlags & TEXTUREFLAGS_SRGB);
			this->lstFlags->Items->Add("Normal Map", uiFlags & TEXTUREFLAGS_NORMAL);
			this->lstFlags->Items->Add("No Mipmap", uiFlags & TEXTUREFLAGS_NOMIP);
			this->lstFlags->Items->Add("No Level Of Detail", uiFlags & TEXTUREFLAGS_NOLOD);
			this->lstFlags->Items->Add("No Minimum Mipmap", uiFlags & TEXTUREFLAGS_MINMIP);
			this->lstFlags->Items->Add("Procedural", uiFlags & TEXTUREFLAGS_PROCEDURAL);
			this->lstFlags->Items->Add("One Bit Alpha (Format Specific)", uiFlags & TEXTUREFLAGS_ONEBITALPHA);
			this->lstFlags->Items->Add("Eight Bit Alpha (Format Specific)", uiFlags & TEXTUREFLAGS_EIGHTBITALPHA);
			this->lstFlags->Items->Add("Enviroment Map (Format Specific)", uiFlags & TEXTUREFLAGS_ENVMAP);
			this->lstFlags->Items->Add("Render Target", uiFlags & TEXTUREFLAGS_RENDERTARGET);
			this->lstFlags->Items->Add("Depth Render Target", uiFlags & TEXTUREFLAGS_DEPTHRENDERTARGET);
			this->lstFlags->Items->Add("No Debug Override", uiFlags & TEXTUREFLAGS_NODEBUGOVERRIDE);
			this->lstFlags->Items->Add("Single Copy", uiFlags & TEXTUREFLAGS_SINGLECOPY);
			this->lstFlags->Items->Add("Unused", uiFlags & TEXTUREFLAGS_UNUSED0);
			this->lstFlags->Items->Add("Unused", uiFlags & TEXTUREFLAGS_UNUSED1);
			this->lstFlags->Items->Add("Unused", uiFlags & TEXTUREFLAGS_UNUSED2);
			this->lstFlags->Items->Add("Unused", uiFlags & TEXTUREFLAGS_UNUSED3);
			this->lstFlags->Items->Add("No Depth Buffer", uiFlags & TEXTUREFLAGS_NODEPTHBUFFER);
			this->lstFlags->Items->Add("Unused", uiFlags & TEXTUREFLAGS_UNUSED4);
			this->lstFlags->Items->Add("Clamp U", uiFlags & TEXTUREFLAGS_CLAMPU);
			this->lstFlags->Items->Add("Vertex Texture", uiFlags & TEXTUREFLAGS_VERTEXTEXTURE);
			this->lstFlags->Items->Add("SSBump", uiFlags & TEXTUREFLAGS_SSBUMP);
			this->lstFlags->Items->Add("Unused", uiFlags & TEXTUREFLAGS_UNUSED5);
			this->lstFlags->Items->Add("Clamp All", uiFlags & TEXTUREFLAGS_BORDER);
			#pragma warning(default: 4800)

			this->lstFlags->EndUpdate();

			this->lblFileVersion->Text = System::String::Concat(VTFFile->GetMajorVersion().ToString(), ".", VTFFile->GetMinorVersion().ToString());
			this->lblFileSize->Text = System::String::Concat(((float)VTFFile->GetSize() / 1024.0f).ToString("#,##0.000"), " KB");

			vlShort sAuxCompressionLevel = VTFFile->GetAuxCompressionLevel();
			if(sAuxCompressionLevel == VTF_AUX_COMPRESSION_LEVEL_NONE)
			{
				this->lblFileCompression->Text = "None";
			}
			else
			{
				System::String ^sMethod = VTFFile->GetAuxCompressionMethod() == AUX_COMPRESSION_METHOD_ZSTD ? "Zstandard" : "Deflate";
				this->lblFileCompression->Text = sAuxCompressionLevel == VTF_AUX_COMPRESSION_LEVEL_DEFAULT
					? sMethod
					: System::String::Concat(sMethod, " (", sAuxCompressionLevel.ToString(), ")");
			}

			this->lblImageWidth->Text = VTFFile->GetWidth().ToString();
			this->lblImageHeight->Text = VTFFile->GetHeight().ToString();
			this->lblImageFormat->Text = this->GetImageFormatString(VTFFile->GetFormat());
			this->lblImageFrames->Text = VTFFile->GetFrameCount().ToString();
			//this->lblImageStartFrame->Text = VTFFile->GetStartFrame().ToString();
			this->numImageStartFrame->Maximum = VTFFile->GetFrameCount() - 1;
			this->numImageStartFrame->Value = VTFFile->GetStartFrame() == 0xffff ? 0 : VTFFile->GetStartFrame();
			this->lblImageFaces->Text = VTFFile->GetFaceCount().ToString();
			this->lblImageSlices->Text = VTFFile->GetDepth().ToString();
			this->lblImageMipmaps->Text = VTFFile->GetMipmapCount().ToString();
			//this->lblImageBumpmapScale->Text = VTFFile->GetBumpmapScale().ToString("0.00");
			this->numImageBumpmapScale->Value = System::Decimal(VTFFile->GetBumpmapScale());

			vlSingle sX, sY, sZ;
			VTFFile->GetReflectivity(sX, sY, sZ);

			this->lblImageReflectivity->Text = System::String::Concat(sX.ToString("0.000"), ", ", sY.ToString("0.000"), ", ", sZ.ToString("0.000"));

			this->lblThumbnailWidth->Text = VTFFile->GetThumbnailWidth().ToString();
			this->lblThumbnailHeight->Text = VTFFile->GetThumbnailHeight().ToString();
			this->lblThumbnailFormat->Text = this->GetImageFormatString(VTFFile->GetThumbnailFormat());

			this->lblResourceCount->Text = VTFFile->GetResourceCount().ToString();

			this->treResources->BeginUpdate();
			this->treResources->Nodes->Clear();

			for(vlUInt i = 0; i < VTFFile->GetResourceCount(); i++)
			{
				vlUInt uiResource = VTFFile->GetResourceType(i);

				System::String ^sName = "Unknown";
				switch(uiResource)
				{
				case VTF_LEGACY_RSRC_LOW_RES_IMAGE:
					sName = "Thumbnail Image";
					break;
				case VTF_LEGACY_RSRC_IMAGE:
					sName = "Image";
					break;
				case VTF_RSRC_SHEET:
					sName = "Sheet";
					break;
				case VTF_RSRC_CRC:
					sName = "Cyclic Redundancy Check";
					break;
				case VTF_RSRC_TEXTURE_LOD_SETTINGS:
					sName = "LOD Settings";
					break;
				case VTF_RSRC_TEXTURE_SETTINGS_EX:
					sName = "Extended Texture Settings";
					break;
				case VTF_RSRC_PARALLAX_CUBEMAP:
					sName = "Parallax Cubemap";
					break;
				case VTF_RSRC_KEY_VALUE_DATA:
					sName = "Key/Value Data";
					break;
				default:
					sName = "Unknown";
					break;
				}

				System::Windows::Forms::TreeNode ^pNode = gcnew System::Windows::Forms::TreeNode(sName);

				vlUInt uiSize = 0;
				vlVoid *lpData = VTFFile->GetResourceData(uiResource, uiSize);

				switch(uiResource)
				{
				case VTF_RSRC_CRC:
					pNode->Nodes->Add(System::String::Concat("Checksum: 0x", (*(vlUInt *)lpData).ToString("X8")));
					break;
				case VTF_RSRC_TEXTURE_LOD_SETTINGS:
					if(lpData && uiSize == sizeof(SVTFTextureLODControlResource))
					{
						SVTFTextureLODControlResource *pLODControl = (SVTFTextureLODControlResource *)lpData;
						pNode->Nodes->Add(System::String::Concat("Clamp U: ", pLODControl->ResolutionClampU.ToString()));
						pNode->Nodes->Add(System::String::Concat("Clamp V: ", pLODControl->ResolutionClampV.ToString()));
						break;
					}
				case VTF_RSRC_KEY_VALUE_DATA:
					if(lpData && uiSize)
					{
						VTFLib::CVMTFile *pVMTFile = new VTFLib::CVMTFile();

						if(pVMTFile->Load(lpData, uiSize))
						{
							pNode->Text = gcnew System::String(pVMTFile->GetRoot()->GetName());
							this->SetInformation(pNode, pVMTFile->GetRoot());
						}
					}
				default:
					if(lpData && uiSize == sizeof(vlUInt))
					{
						pNode->Nodes->Add(System::String::Concat("Data: 0x", (*(vlUInt *)lpData).ToString("X8")));
					}
					else
					{
						pNode->Nodes->Add(System::String::Concat("Size: ", uiSize.ToString("#,##0"), " B"));
					}
					break;
				}

				this->treResources->Nodes->Add(pNode);
				pNode->Expand();
			}

			this->treResources->EndUpdate();

			this->VTFFile = VTFFile;

			this->fImageScale = 1.0f;
			this->UpdateVTFFile();

			// "Show" the tab pages.
			if(!this->tabSidebar->TabPages->Contains(this->tabImage))
			{
				this->tabSidebar->TabPages->Add(this->tabImage);
			}
			if(!this->tabSidebarRight->TabPages->Contains(this->tabInfo))
			{
				this->tabSidebarRight->TabPages->Add(this->tabInfo);
			}
			if(!this->tabSidebarRight->TabPages->Contains(this->tabResources))
			{
				this->tabSidebarRight->TabPages->Add(this->tabResources);
			}
			this->UpdateSidebarsVisible();

			this->tabSidebar->SelectedTab = this->tabImage;
			this->tabSidebarRight->SelectedTab = this->tabInfo;
		}

		private: void SetInformation(System::Windows::Forms::TreeNode ^pNode, VTFLib::Nodes::CVMTGroupNode *pVMTNode)
		{
			for(vlUInt i = 0; i < pVMTNode->GetNodeCount(); i++)
			{
				VTFLib::Nodes::CVMTNode *pVMTChild = pVMTNode->GetNode(i);
				switch(pVMTChild->GetType())
				{
					case NODE_TYPE_GROUP:
					{
						System::Windows::Forms::TreeNode ^pChild = gcnew System::Windows::Forms::TreeNode(gcnew System::String(pVMTChild->GetName()));
						this->SetInformation(pChild, static_cast<VTFLib::Nodes::CVMTGroupNode *>(pVMTChild));
						pNode->Nodes->Add(pChild);
						break;
					}
					case NODE_TYPE_STRING:
					{
						System::Windows::Forms::TreeNode ^pChild = gcnew System::Windows::Forms::TreeNode(gcnew System::String(pVMTChild->GetName()));
						pChild->Nodes->Add(gcnew System::Windows::Forms::TreeNode(gcnew System::String(static_cast<VTFLib::Nodes::CVMTStringNode *>(pVMTChild)->GetValue())));
						pNode->Nodes->Add(pChild);
						break;
					}
					case NODE_TYPE_INTEGER:
					{
						System::Windows::Forms::TreeNode ^pChild = gcnew System::Windows::Forms::TreeNode(gcnew System::String(pVMTChild->GetName()));
						pChild->Nodes->Add(gcnew System::Windows::Forms::TreeNode(static_cast<VTFLib::Nodes::CVMTIntegerNode *>(pVMTChild)->GetValue().ToString()));
						pNode->Nodes->Add(pChild);
						break;
					}
					case NODE_TYPE_SINGLE:
					{
						System::Windows::Forms::TreeNode ^pChild = gcnew System::Windows::Forms::TreeNode(gcnew System::String(pVMTChild->GetName()));
						pChild->Nodes->Add(gcnew System::Windows::Forms::TreeNode(static_cast<VTFLib::Nodes::CVMTSingleNode *>(pVMTChild)->GetValue().ToString()));
						pNode->Nodes->Add(pChild);
						break;
					}
				}
			}
		}

		private: bool GetVTFFile()
		{
			this->VTFFile->SetFlags(0);
			for(vlUInt i = 0, j = 0x00000001; i < (vlUInt)this->lstFlags->Items->Count; i++, j <<= 1)
			{
				this->VTFFile->SetFlag((VTFImageFlag)j, this->lstFlags->GetItemChecked(i));
			}

			this->VTFFile->SetStartFrame(Convert::ToUInt32(this->numImageStartFrame->Value));

			VTFFile->SetBumpmapScale(Convert::ToSingle(this->numImageBumpmapScale->Value));

			return true;
		}

		private: void SetVMTFile(VTFLib::CVMTFile *VMTFile)
		{
			this->VMTFile = VMTFile;

			/*vlUInt Size;
			char cBuffer[8192];

			VMTFile->Save(cBuffer, sizeof(cBuffer), Size);
			cBuffer[Size] = '\0';

			this->txtVMTFile->Text = gcnew System::String(cBuffer);
			this->txtVMTFile->Visible = true;*/

			this->EnableVMTContextMenuItems();
			this->SyntaxHilighter->Enabled = true;
			this->ValidateVMTFile();
			this->SyntaxHilighter->Process();

			this->txtVMTFile->Visible = true;
		}

		private: bool ValidateVMTFile()
		{
			if(this->VMTFile == 0)
				return true;

			char *cText = (char *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(this->txtVMTFile->Text).ToPointer();
			vlBool bResult = this->VMTFile->Load(cText, this->txtVMTFile->Text->Length);
			System::Runtime::InteropServices::Marshal::FreeHGlobal((IntPtr)cText);

			if(bResult)
			{
				this->SyntaxHilighter->ErrorLine = 0;
				this->pnlInfo1->Text = nullptr;
			}
			else
			{
				this->SyntaxHilighter->ErrorLine = (int)this->VMTFile->GetParseErrorLine();
				this->pnlInfo1->Text = gcnew System::String(vlGetLastError());
			}

			return bResult != 0;
		}

		private: bool GetVMTFile()
		{
			if(!this->ValidateVMTFile())
			{
				return MessageBox::Show(System::String::Concat("This VMT has a syntax error:\n\n", gcnew System::String(vlGetLastError()), "\n\nSave it anyway?"), Application::ProductName, MessageBoxButtons::YesNo, MessageBoxIcon::Warning) == System::Windows::Forms::DialogResult::Yes;
			}

			return true;
		}

		private: System::Void barTool_ButtonClick(System::Object ^  sender, System::Windows::Forms::ToolBarButtonClickEventArgs ^  e)
		{
			if(e->Button == this->btnToolImport)
			{
				this->btnImport_Click(this->btnImport, System::EventArgs::Empty);
			}
			else if(e->Button == this->btnToolOpen)
			{
				this->btnOpen_Click(this->btnOpen, System::EventArgs::Empty);
			}
			else if(e->Button == this->btnToolSave)
			{
				this->btnSave_Click(this->btnSave, System::EventArgs::Empty);
			}
			else if(e->Button == this->btnToolCopy)
			{
				this->btnCopy_Click(this->btnCopy, System::EventArgs::Empty);
			}
			else if(e->Button == this->btnToolPaste)
			{
				this->btnPaste_Click(this->btnPaste, System::EventArgs::Empty);
			}
		}

		private: void New()
		{
			this->Close();

			VTFLib::CVMTFile *VMTFile = new VTFLib::CVMTFile();

			this->txtVMTFile->Text = "\"LightmappedGeneric\"\n{\n}";
			this->txtVMTFile->SelectionStart = 1;
			this->txtVMTFile->SelectionLength = 18;

			this->SetVMTFile(VMTFile);

			this->FileName = nullptr;

			this->btnSave->Enabled = true;
			this->btnToolSave->Enabled = true;
			this->btnSaveAs->Enabled = true;

			this->txtVMTFile->Focus();
		}

		private: void Open(System::String ^sFileName, bool bTemp)
		{
			this->Close();

			char cPath[512];
			CUtility::StringToCharPointer(sFileName, cPath, 512);

			if(sFileName->ToLower()->EndsWith(".vtf"))
			{
				VTFLib::CVTFFile *VTFFile = new VTFLib::CVTFFile();

				if(VTFFile->Load(cPath))
				{
					this->SetVTFFile(VTFFile);

					if(!bTemp)
					{
						this->FileName = sFileName;

						this->AddRecentFile(this->FileName);
					}
					else
					{
						this->FileName = nullptr;
					}

					this->btnSave->Enabled = true;
					this->btnToolSave->Enabled = true;
					this->btnSaveAs->Enabled = true;

					this->btnExport->Enabled = true;
					this->btnExportAll->Enabled = true;

					this->btnToolCopy->Enabled = true;
					this->btnCopy->Enabled = true;
				}
				else
				{
					delete VTFFile;

					MessageBox::Show(System::String::Concat("Error loading VTF texture:\n\n", gcnew System::String(vlGetLastError())), Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
			}
			else if(sFileName->ToLower()->EndsWith(".vmt"))
			{
				VTFLib::CVMTFile *VMTFile = new VTFLib::CVMTFile();

				VMTFile->Load(cPath);

				try
				{
					this->txtVMTFile->LoadFile(sFileName, System::Windows::Forms::RichTextBoxStreamType::PlainText);
				}
				catch(Exception ^e)
				{
					delete VMTFile;

					MessageBox::Show(System::String::Concat("Error loading VMT texture:\n\n", e->Message), Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);
					return;
				}

				this->SetVMTFile(VMTFile);

				if(!bTemp)
				{
					this->FileName = sFileName;

					this->AddRecentFile(this->FileName);
				}
				else
				{
					this->FileName = nullptr;
				}

				this->btnSave->Enabled = true;
				this->btnToolSave->Enabled = true;
				this->btnSaveAs->Enabled = true;

				this->txtVMTFile->Focus();
			}
		}

		private: void Save(System::String ^sFileName)
		{
			if(sFileName == nullptr)
				return;

			if(this->VTFFile != 0)
			{
				char cPath[512];
				CUtility::StringToCharPointer(sFileName, cPath, 512);

				if (!this->GetVTFFile())
				{
					return;
				}

				if(this->VTFFile->Save(cPath))
				{
					this->FileName = sFileName;

					this->AddRecentFile(this->FileName);

					if(this->btnAutoCreateVMTFile->Checked)
					{
						CVMTFileUtility::CreateDefaultMaterial(this->FileName, "LightmappedGeneric");
					}
				}
				else
				{
					MessageBox::Show(System::String::Concat("Error saving VTF texture:\n\n", gcnew System::String(vlGetLastError())), Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
			}
			else if(this->VMTFile != 0)
			{
				char cPath[512];
				CUtility::StringToCharPointer(sFileName, cPath, 512);

				if(!this->GetVMTFile())
				{
					return;
				}

				/*if(this->txtVMTFile->SaveFile(sFileName)this->VMTFile->Save(cPath))
				{
					this->FileName = sFileName;
				}
				else
				{
					MessageBox::Show(System::String::Concat("Error saving VMT texture:\n\n", gcnew System::String(vlGetLastError())), Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);
				}*/

				try
				{
					this->txtVMTFile->SaveFile(sFileName, System::Windows::Forms::RichTextBoxStreamType::PlainText);

					this->FileName = sFileName;
				}
				catch(Exception ^e)
				{
					MessageBox::Show(System::String::Concat("Error saving VMT texture:\n\n", e->Message), Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
			}
		}

		private: void SaveAs()
		{
			if(this->VTFFile != 0)
			{
				if(this->dlgSaveVTFFile->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				{
					this->Save(this->dlgSaveVTFFile->FileName);
				}
			}
			else if(this->VMTFile != 0)
			{
				if(this->dlgSaveVMTFile->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				{
					this->Save(this->dlgSaveVMTFile->FileName);
				}
			}
		}

		private: void Import(array<System::String ^>^ sFileNames)
		{
			if(this->Options->ShowDialog() != System::Windows::Forms::DialogResult::OK)
			{
				return;
			}

			this->Close();

			char cPath[512];

			VTFLib::CVTFFile *VTFFile = new VTFLib::CVTFFile();

			// Initialize image data.
			vlUInt uiWidth = 0, uiHeight = 0;
			bool bHasAlpha = false;

			std::vector<vlByte *> vImageData;

			// Load each frame/face.
			for(int i = 0; i < sFileNames->Length && VTFFile != 0; i++)
			{
				CUtility::StringToCharPointer(static_cast<System::String ^>(sFileNames[i]), cPath, 512);

				// Load the image and convert it to RGBA.
				if(ilLoadImage(cPath))
				{
					ILuint uiImage = (ILuint)ilGetInteger(IL_CUR_IMAGE);
					vlUInt uiImages = (vlUInt)ilGetInteger(IL_NUM_IMAGES) + 1;

					// Copy every animation frame the file contains.
					for(vlUInt j = 0; j < uiImages; j++)
					{
						ilBindImage(uiImage);
						ilActiveImage((ILuint)j);

						if(ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
						{
							// Get the size of the image and make sure it matches the other images.
							if(vImageData.empty())
							{
								uiWidth = (vlUInt)ilGetInteger(IL_IMAGE_WIDTH);
								uiHeight = (vlUInt)ilGetInteger(IL_IMAGE_HEIGHT);
							}
							else
							{
								if(uiWidth != (vlUInt)ilGetInteger(IL_IMAGE_WIDTH) || uiHeight != (vlUInt)ilGetInteger(IL_IMAGE_HEIGHT))
								{
									delete VTFFile;
									VTFFile = 0;

									MessageBox::Show("Error loading image:\n\nAll frames and faces must be the same size.", Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);

									break;
								}
							}

							// Copy the image data.
							vlByte *lpFrameData = new vlByte[uiWidth * uiHeight * 4];
							memcpy(lpFrameData, ilGetData(), uiWidth * uiHeight * 4);
							vImageData.push_back(lpFrameData);

							bHasAlpha |= !this->Options->StripAlpha && CVTFFileUtility::HasAlphaData(lpFrameData, uiWidth, uiHeight);
						}
						else
						{
							delete VTFFile;
							VTFFile = 0;

							MessageBox::Show("Error converting image.", Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);

							break;
						}
					}

					// Leave the base image bound for the next file.
					ilBindImage(uiImage);
				}
				else
				{
					delete VTFFile;
					VTFFile = 0;

					MessageBox::Show("Error loading image.", Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);

					break;
				}
			}

			// Check that we loaded all images.
			if(VTFFile != 0)
			{
				vlUInt uiImages = (vlUInt)vImageData.size();
				vlByte **lpImageData = uiImages != 0 ? &vImageData[0] : 0;

				vlUInt uiFrames = this->Options->TextureType == 0 ? uiImages : 1;
				vlUInt uiFaces = this->Options->TextureType == 1 ? uiImages : 1;
				vlUInt uiSlices = this->Options->TextureType == 2 ? uiImages : 1;

				SVTFCreateOptions VTFCreateOptions = CVTFFileUtility::GetCreateOptions(this->Options);

				VTFCreateOptions.ImageFormat = bHasAlpha ? Options->AlphaFormat : Options->NormalFormat;

				// Create the .vtf file.
				if(VTFFile->Create(uiWidth, uiHeight, uiFrames, uiFaces, uiSlices, lpImageData, VTFCreateOptions) != vlFalse && CVTFFileUtility::CreateResources(Options, VTFFile))
				{
					this->SetVTFFile(VTFFile);

					this->FileName = nullptr;

					this->btnSave->Enabled = true;
					this->btnToolSave->Enabled = true;
					this->btnSaveAs->Enabled = true;

					this->btnExport->Enabled = true;
					this->btnExportAll->Enabled = true;

					this->btnToolCopy->Enabled = true;
					this->btnCopy->Enabled = true;
				}
				else
				{
					delete VTFFile;

					MessageBox::Show(System::String::Concat("Error creating VTF texture:\n\n", gcnew System::String(vlGetLastError())), Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
			}

			// Delete all image data.
			for(vlUInt i = 0; i < (vlUInt)vImageData.size(); i++)
			{
				delete []vImageData[i];
			}
		}

		private: void Export(System::String ^sFileName)
		{
			if(this->VTFFile == 0)
				return;

			char cPath[512];
			CUtility::StringToCharPointer(sFileName, cPath, 512);

			//ILuint uiImage;

			//ilInit();

			//ilEnable(IL_ORIGIN_SET);
			//ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

			//ilGenImages(1, &uiImage);
			//ilBindImage(uiImage);

			vlUInt uiWidth, uiHeight, uiDepth;
			this->VTFFile->ComputeMipmapDimensions(this->VTFFile->GetWidth(), this->VTFFile->GetHeight(), this->VTFFile->GetDepth(), (vlUInt)this->numMipmap->Value, uiWidth,  uiHeight, uiDepth);

			vlByte *lpImageData = new vlByte[this->VTFFile->ComputeImageSize(uiWidth, uiHeight, 1, IMAGE_FORMAT_RGBA8888)];

			VTFFile->ConvertToRGBA8888(this->VTFFile->GetData((vlUInt)this->numFrame->Value, (vlUInt)this->numFace->Value, (vlUInt)this->numSlice->Value, (vlUInt)this->numMipmap->Value), lpImageData, uiWidth, uiHeight, this->VTFFile->GetFormat());

			// DevIL likes image data upside down...
			VTFFile->FlipImage(lpImageData, uiWidth, uiHeight);

			if(!(ilTexImage(uiWidth, uiHeight, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, lpImageData) && ilSaveImage(cPath)))
			{
				MessageBox::Show("Error saving image.", Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);
			}

			delete []lpImageData;

			//ilDeleteImages(1, &uiImage);
			//ilShutDown();
		}

		private: void ExportAll(System::String ^sFileName)
		{
			if(this->VTFFile == 0)
				return;

			char cPath[512];
			char cExt[512] = "";
			char *cInsert;

			CUtility::StringToCharPointer(sFileName, cPath, 512);
			cInsert = strrchr(cPath, '.');
			if(cInsert)
			{
				strcpy(cExt, cInsert);
			}
			else
			{
				cInsert = cPath + strlen(cPath);
			}

			//ILuint uiImage;

			//ilInit();

			//ilEnable(IL_ORIGIN_SET);
			//ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

			//ilGenImages(1, &uiImage);
			//ilBindImage(uiImage);

			vlUInt uiWidth, uiHeight, uiDepth;
			this->VTFFile->ComputeMipmapDimensions(this->VTFFile->GetWidth(), this->VTFFile->GetHeight(), this->VTFFile->GetDepth(), (vlUInt)this->numMipmap->Value, uiWidth,  uiHeight, uiDepth);

			vlByte *lpImageData = new vlByte[this->VTFFile->ComputeImageSize(uiWidth, uiHeight, 1, IMAGE_FORMAT_RGBA8888)];

			for(vlUInt i = 0; i < this->VTFFile->GetFrameCount(); i++)
			{
				for(vlUInt j = 0; j < this->VTFFile->GetFaceCount(); j++)
				{
					for(vlUInt k = 0; k < this->VTFFile->GetDepth(); k++)
					{
						VTFFile->ConvertToRGBA8888(this->VTFFile->GetData(i, j, k, (vlUInt)this->numMipmap->Value), lpImageData, uiWidth, uiHeight, this->VTFFile->GetFormat());

						// DevIL likes image data upside down...
						VTFFile->FlipImage(lpImageData, uiWidth, uiHeight);

						sprintf(cInsert, "_%.2u_%.2u_%.2u%s", i, j, k, cExt);
						if(!(ilTexImage(uiWidth, uiHeight, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, lpImageData) && ilSaveImage(cPath)))
						{
							MessageBox::Show("Error saving image.", Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);
						}
					}
				}
			}

			delete []lpImageData;

			//ilDeleteImages(1, &uiImage);
			//ilShutDown();
		}

		private: void Close()
		{
			this->btnToolSave->Enabled = false;
			this->btnSave->Enabled = false;
			this->btnSaveAs->Enabled = false;

			this->btnExport->Enabled = false;
			this->btnExportAll->Enabled = false;

			this->btnToolCopy->Enabled = false;
			this->btnCopy->Enabled = false;

			this->trkHDRExposure->Enabled = false;

			this->btnAnimate->Text = "&Play";
			this->btnAnimate->Enabled = false;
			this->numAnimateFPS->Enabled = false;
			this->tmrAnimate->Enabled = false;

			// "Hide" the tab pages.
			if(this->tabSidebarRight->TabPages->Contains(this->tabResources))
			{
				this->tabSidebarRight->TabPages->Remove(this->tabResources);
			}
			if(this->tabSidebarRight->TabPages->Contains(this->tabInfo))
			{
				this->tabSidebarRight->TabPages->Remove(this->tabInfo);
			}
			if(this->tabSidebar->TabPages->Contains(this->tabImage))
			{
				this->tabSidebar->TabPages->Remove(this->tabImage);
			}
			this->UpdateSidebarsVisible();

			this->picVTFFileTL->Visible = false;
			this->picVTFFileTR->Visible = false;
			this->picVTFFileBL->Visible = false;
			this->picVTFFileBR->Visible = false;

			this->picVTFFileTL->Image = nullptr;
			this->picVTFFileTR->Image = nullptr;
			this->picVTFFileBL->Image = nullptr;
			this->picVTFFileBR->Image = nullptr;

			//this->treVMTFile->Visible = false;
			//this->treVMTFile->Nodes->Clear();
			this->SyntaxHilighter->Enabled = false;
			this->SyntaxHilighter->Purge();

			this->txtVMTFile->Visible = false;
			this->txtVMTFile->Clear();
			this->txtVMTFile->ClearUndo();

			delete this->VMTFile;
			this->VMTFile = 0;

			delete this->VTFFile;
			this->VTFFile = 0;

			delete []this->ucImageData;
			this->ucImageData = 0;

			this->FileName = nullptr;
			this->pnlInfo1->Text = nullptr;
			this->pnlInfo2->Text = nullptr;
		}

		//
		// Main menu.
		//

		private: System::Void btnNew_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->New();
		}

		private: System::Void btnOpen_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(this->dlgOpenFile->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				this->Open(this->dlgOpenFile->FileName, false);
			}
		}

		private: System::Void btnSave_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(this->FileName != nullptr)
				this->Save(this->FileName);
			else
				this->SaveAs();
		}

		private: System::Void btnSaveAs_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->SaveAs();
		}

		private: System::Void btnImport_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(this->dlgImportFile->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				this->Import(this->dlgImportFile->FileNames);
			}
		}

		private: System::String ^RemoveExtension(System::String ^sFileName)
		{
			if(sFileName == nullptr)
			{
				return "";
			}

			System::String ^sTempFileName = sFileName;
			if(sTempFileName->LastIndexOf("\\") != -1)
			{
				sTempFileName = sTempFileName->Substring(sTempFileName->LastIndexOf("\\") + 1);
			}
			if(sTempFileName->LastIndexOf(".") != -1)
			{
				sTempFileName = sTempFileName->Substring(0, sTempFileName->LastIndexOf("."));
			}
			return sTempFileName;
		}

		private: System::Void btnExport_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->dlgExportFile->FileName = this->RemoveExtension(this->FileName);

			if(this->dlgExportFile->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				this->Export(this->dlgExportFile->FileName);
			}
		}

		private: System::Void btnExportAll_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->dlgExportFile->FileName = this->RemoveExtension(this->FileName);

			if(this->dlgExportFile->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				this->ExportAll(this->dlgExportFile->FileName);
			}
		}

		private: System::Void btnCreateVMTFile_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(this->VTFFile != 0 && this->FileName != nullptr && (this->FileName->Length >= 4 && System::String::Compare(this->FileName, this->FileName->Length - 4, ".vtf", 0, 4, true) == 0))
			{
				this->VMTCreate->Create(this->FileName, *this->VTFFile);
			}
			this->VMTCreate->ShowDialog();
		}

		private: System::Void btnConvertFolder_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->BatchConvert->ShowDialog();
		}

		private: System::Void btnRecentFile_Click(System::Object ^sender, System::EventArgs ^e)
		{
			this->Open(static_cast<System::String ^>(this->RecentFiles[static_cast<MenuItem ^>(sender)->Index]), false);
		}

		private: System::Void btnExit_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			__super::Close();
		}

		private: System::Void btnCopy_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(System::Threading::Thread::CurrentThread->ApartmentState == System::Threading::ApartmentState::STA)
			{
				if(this->picVTFFileTL->Image != nullptr)
				{
					System::Windows::Forms::Clipboard::SetDataObject(this->picVTFFileTL->Image, true);
				}
			}
			else
			{
				MessageBox::Show("Operation not supported.\n\nVTFEdit has determined that the current thread apartment state does not\nsupport this operation. This is a .NET design flaw.", Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}

		private: System::Void btnPaste_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(System::Threading::Thread::CurrentThread->ApartmentState == System::Threading::ApartmentState::STA)
			{
				System::Windows::Forms::IDataObject ^Data = System::Windows::Forms::Clipboard::GetDataObject();

				if(!Data->GetDataPresent(System::Windows::Forms::DataFormats::Bitmap))
				{
					return;
				}

				System::Drawing::Bitmap ^Bitmap = static_cast<System::Drawing::Bitmap ^>(Data->GetData(System::Windows::Forms::DataFormats::Bitmap));

				if(this->Options->ShowDialog() != System::Windows::Forms::DialogResult::OK)
				{
					return;
				}

				this->Close();

				VTFLib::CVTFFile *VTFFile = new VTFLib::CVTFFile();

				// Initialize image data.
				vlUInt uiWidth = (vlUInt)Bitmap->Width, uiHeight = (vlUInt)Bitmap->Height;

				vlByte *lpImageData = new vlByte[uiWidth * uiHeight * 4];
				vlByte *lpPointer = lpImageData;

				for(int j = 0; j < Bitmap->Height; j++)
				{
					for(int i = 0; i < Bitmap->Width; i++)
					{
						System::Drawing::Color Pixel = Bitmap->GetPixel(i, j);

						*(lpPointer++) = (vlByte)Pixel.R;
						*(lpPointer++) = (vlByte)Pixel.G;
						*(lpPointer++) = (vlByte)Pixel.B;
						*(lpPointer++) = (vlByte)Pixel.A;
					}
				}

				SVTFCreateOptions VTFCreateOptions = CVTFFileUtility::GetCreateOptions(this->Options);

				// Create the .vtf file.
				if(VTFFile->Create(uiWidth, uiHeight, lpImageData, VTFCreateOptions) != vlFalse && CVTFFileUtility::CreateResources(Options, VTFFile))
				{
					this->SetVTFFile(VTFFile);

					this->btnToolSave->Enabled = true;
					this->btnSaveAs->Enabled = true;

					this->btnExport->Enabled = true;
					this->btnExportAll->Enabled = true;

					this->btnToolCopy->Enabled = true;
					this->btnCopy->Enabled = true;
				}
				else
				{
					delete VTFFile;

					MessageBox::Show(System::String::Concat("Error creating VTF texture:\n\n", gcnew System::String(vlGetLastError())), Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);
				}

				delete []lpImageData;
			}
			else
			{
				MessageBox::Show("Operation not supported.\n\nVTFEdit has determined that the current thread apartment state does not\nsupport this operation. This is a .NET design flaw.", Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}

		private: System::Void btnChannel_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->btnChannelRGB->Checked = false;
			this->btnChannelR->Checked = false;
			this->btnChannelG->Checked = false;
			this->btnChannelB->Checked = false;
			this->btnChannelA->Checked = false;

			static_cast<System::Windows::Forms::MenuItem ^>(sender)->Checked = true;

			this->UpdateVTFFile();
		}

		private: System::Void btnMask_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->btnMask->Checked = !this->btnMask->Checked;

			this->UpdateVTFFile();
		}

		private: System::Void btnTile_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->btnTile->Checked = !this->btnTile->Checked;

			this->UpdateVTFFile();
		}

		private: System::Void btnMipmapFullSize_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->btnMipmapFullSize->Checked = !this->btnMipmapFullSize->Checked;

			this->UpdateVTFFile();
		}

		private: System::Void btnAutoCreateVMTFile_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->btnAutoCreateVMTFile->Checked = !this->btnAutoCreateVMTFile->Checked;
		}

		private: System::Void btnAbout_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->About->ShowDialog();
		}

		private: System::Void numVTFFile_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(!this->bHDRReseting)
			{
				this->UpdateVTFFile();
			}
		}

		private: System::Void trkHDRKey_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(!this->bHDRReseting)
			{
				this->UpdateVTFFile();
			}
		}

		private: System::Void trkHDRShift_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(!this->bHDRReseting)
			{
				this->UpdateVTFFile();
			}
		}

		private: System::Void trkHDRGamma_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->UpdateVTFFile();
		}

		//
		// Animation.
		//

		private: System::Void btnAnimate_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(this->tmrAnimate->Enabled)
			{
				this->tmrAnimate->Enabled = false;
				this->btnAnimate->Text = "&Play";
			}
			else
			{
				this->tmrAnimate->Enabled = true;
				this->btnAnimate->Text = "&Stop";
			}
		}

		private: System::Void numAnimateFPS_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(this->tmrAnimate == nullptr)
				return;

			int iFPS = (int)this->numAnimateFPS->Value;

			this->tmrAnimate->Interval = 1000 / iFPS;
		}

		private: System::Void tmrAnimate_Tick(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(System::Windows::Forms::Form::ActiveForm != this)
				return;

			int iValue = (int)this->numFrame->Value + 1;

			if(iValue > this->numFrame->Maximum)
				iValue = (int)this->numFrame->Minimum;

			this->numFrame->Value = iValue;
		}

		private: System::Void lstFlags_ItemCheck(System::Object ^  sender, System::Windows::Forms::ItemCheckEventArgs ^  e)
		{
			if(this->VTFFile == 0)
				return;

			if(e->Index == 12 || e->Index == 13 || e->Index == 14 || static_cast<System::String ^>(this->lstFlags->Items[e->Index]) == "Unused")
			{
				e->NewValue = e->CurrentValue;
			}
		}

		//
		// HDR context.
		//

		private: System::Void btnHDRReset_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->bHDRReseting = true;

			this->trkHDRExposure->LargeChange = 1;
			this->trkHDRExposure->Minimum = 0;
			this->trkHDRExposure->Maximum = 8000;
			this->trkHDRExposure->Value = 2000;
			this->trkHDRExposure->TickFrequency = 160;

			this->bHDRReseting = false;

			this->UpdateVTFFile();
		}

		//
		// VTF context.
		//

		private: System::Void picVTFFile_MouseMove(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e)
		{
			if(this->UpdateImagePan())
				return;

			int iX = (int)((float)e->X / this->fEffectiveImageScale) + 1;
			int iY = (int)((float)e->Y / this->fEffectiveImageScale) + 1;

			this->pnlInfo2->Text = System::String::Concat(iX.ToString(), ", ", iY.ToString());
		}

		private: System::Void pnlMain_MouseMove(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e)
		{
			this->UpdateImagePan();
		}

		private: System::Void picVTFFile_MouseDown(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e)
		{
			if(this->VTFFile == 0 || e->Button != System::Windows::Forms::MouseButtons::Left)
				return;

			if((System::Windows::Forms::Control::ModifierKeys & System::Windows::Forms::Keys::Alt) != System::Windows::Forms::Keys::Alt)
				return;

			this->bImagePanning = true;
			this->ImagePanStartMouse = System::Windows::Forms::Control::MousePosition;
			this->ImagePanStartScroll = System::Drawing::Point(-this->pnlMain->AutoScrollPosition.X, -this->pnlMain->AutoScrollPosition.Y);
			this->pnlMain->Cursor = System::Windows::Forms::Cursors::Hand;
		}

		private: System::Void picVTFFile_MouseUp(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e)
		{
			if(!this->bImagePanning)
				return;

			this->bImagePanning = false;
			this->pnlMain->Cursor = System::Windows::Forms::Cursors::Default;
		}

		private: bool UpdateImagePan()
		{
			if(!this->bImagePanning)
				return false;

			if((System::Windows::Forms::Control::MouseButtons & System::Windows::Forms::MouseButtons::Left) != System::Windows::Forms::MouseButtons::Left)
			{
				this->bImagePanning = false;
				this->pnlMain->Cursor = System::Windows::Forms::Cursors::Default;
				return false;
			}

			System::Drawing::Point Mouse = System::Windows::Forms::Control::MousePosition;

			this->pnlMain->AutoScrollPosition = System::Drawing::Point(
				this->ImagePanStartScroll.X - (Mouse.X - this->ImagePanStartMouse.X),
				this->ImagePanStartScroll.Y - (Mouse.Y - this->ImagePanStartMouse.Y));

			return true;
		}

		private: void ZoomVTFFileAt(float fFactor, System::Drawing::Point Anchor)
		{
			if(this->VTFFile == 0 || !this->picVTFFileTL->Visible)
				return;

			if(fFactor > 1.0f && (this->picVTFFileTL->Width >= 4096 || this->picVTFFileTL->Height >= 4096))
				return;

			if(fFactor < 1.0f && this->picVTFFileTL->Width <= 1 && this->picVTFFileTL->Height <= 1)
				return;

			float fOldImageScale = this->fImageScale;

			float fContentX = (float)(Anchor.X - this->pnlMain->AutoScrollPosition.X);
			float fContentY = (float)(Anchor.Y - this->pnlMain->AutoScrollPosition.Y);

			this->fImageScale *= fFactor;

			// this may clamp the scale
			this->UpdateVTFFile();

			float fRatio = this->fImageScale / fOldImageScale;

			this->pnlMain->AutoScrollPosition = System::Drawing::Point(
				(int)(fContentX * fRatio) - Anchor.X,
				(int)(fContentY * fRatio) - Anchor.Y);
		}

		private: void ZoomVTFFile(float fFactor)
		{
			this->ZoomVTFFileAt(fFactor, System::Drawing::Point(this->pnlMain->ClientSize.Width / 2, this->pnlMain->ClientSize.Height / 2));
		}

		private: System::Void btnVTFFileZoomIn_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->ZoomVTFFile(2.0f);
		}

		private: System::Void btnVTFFileZoomOut_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->ZoomVTFFile(0.5f);
		}

		private: System::Void btnVTFFileZoomReset_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(this->VTFFile == 0)
				return;

			this->fImageScale = 1.0f;
			//this->UpdateVTFFileScale();
			this->UpdateVTFFile();

			this->pnlMain->AutoScrollPosition = System::Drawing::Point(0, 0);
		}

		public: virtual bool PreFilterMessage(System::Windows::Forms::Message %WinMessage)
		{
			// intercept the mouse wheel only
			if(WinMessage.Msg != WM_MOUSEWHEEL)
				return false;

			if (this->VTFFile == 0 || !this->picVTFFileTL->Visible)
				return false;

			System::Drawing::Point ScreenPoint = System::Windows::Forms::Control::MousePosition;

			if(!this->pnlMain->RectangleToScreen(this->pnlMain->ClientRectangle).Contains(ScreenPoint))
				return false;

			int iDelta = (short)((WinMessage.WParam.ToInt64() >> 16) & 0xffff);

			if(iDelta != 0)
			{
				this->ZoomVTFFileAt(iDelta > 0 ? 2.0f : 0.5f, this->pnlMain->PointToClient(ScreenPoint));
			}

			return true;
		}

		private: System::Void btnVTFFileCopy_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(this->VTFFile == 0)
				return;

			this->btnCopy_Click(this->btnCopy, System::EventArgs::Empty);
		}

		//
		// VMT context.
		//

		private: System::Void EnableVMTContextMenuItems()
		{
			this->btnVMTFileUndo->Enabled = this->txtVMTFile->CanUndo;
			this->btnVMTFileCut->Enabled = this->txtVMTFile->SelectedText->Length > 0;
			this->btnVMTFileCopy->Enabled = this->txtVMTFile->SelectedText->Length > 0;
			if(System::Threading::Thread::CurrentThread->ApartmentState == System::Threading::ApartmentState::STA)
			{
				this->btnVMTFilePaste->Enabled = false;
				array<System::String ^>^Formats = System::Windows::Forms::Clipboard::GetDataObject()->GetFormats();
				for(int i = 0; i < Formats->Length; i++)
				{
					if(System::Windows::Forms::DataFormats::GetFormat(Formats[i])->Name->Equals("Text"))
					{
						this->btnVMTFilePaste->Enabled = true;
						break;
					}
				}
			}
			else
			{
				this->btnVMTFilePaste->Enabled = true;
			}
			this->btnVMTFileDelete->Enabled = this->txtVMTFile->SelectedText->Length > 0;
			this->btnVMTFileSelectAll->Enabled = this->txtVMTFile->Text->Length > 0;

			int line, column;

			line = this->txtVMTFile->GetLineFromCharIndex(this->txtVMTFile->SelectionStart);
			column = (int)::SendMessage((HWND)this->txtVMTFile->Handle.ToPointer(), EM_LINEINDEX, line, 0);
			column = this->txtVMTFile->SelectionStart - column;

			this->pnlInfo2->Text = System::String::Concat("Ln ", (line + 1).ToString(), ", Col ", (column + 1).ToString());
		}

		private: System::Void btnVMTFileUndo_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->txtVMTFile->Undo();
			this->EnableVMTContextMenuItems();
		}

		private: System::Void btnVMTFileCut_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->txtVMTFile->Cut();
			this->EnableVMTContextMenuItems();
		}

		private: System::Void btnVMTFileCopy_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->txtVMTFile->Copy();
			this->EnableVMTContextMenuItems();
		}

		private: System::Void btnVMTFilePaste_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->txtVMTFile->Paste(System::Windows::Forms::DataFormats::GetFormat(System::Windows::Forms::DataFormats::Text));
			this->EnableVMTContextMenuItems();
		}

		private: System::Void btnVMTFileDelete_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->txtVMTFile->SelectedText = "";
			this->EnableVMTContextMenuItems();
		}

		private: System::Void btnVMTFileSelectAll_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->txtVMTFile->SelectAll();
			this->EnableVMTContextMenuItems();
		}

		private: System::Void btnVMTFileValidateLoose_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(this->ValidateVMTFile())
			{
				MessageBox::Show("VMT validation successful.", Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Information);
			}
			else
			{
				MessageBox::Show(System::String::Concat("Error validating VMT:\n\n", gcnew System::String(vlGetLastError())), Application::ProductName, MessageBoxButtons::OK, MessageBoxIcon::Error);
			}

			this->SyntaxHilighter->Process();
		}

		private: System::Void btnVMTFileValidateStrict_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			vlSetInteger(VTFLIB_VMT_PARSE_MODE, PARSE_MODE_STRICT);
			this->btnVMTFileValidateLoose_Click(sender, e);
			vlSetInteger(VTFLIB_VMT_PARSE_MODE, PARSE_MODE_LOOSE);
		}

		private: System::Void txtVMTFile_SelectionChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->EnableVMTContextMenuItems();
		}

		private: System::Void txtVMTFile_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->EnableVMTContextMenuItems();

			this->ValidateVMTFile();

			this->txtVMTFile->SelectionChanged -= gcnew System::EventHandler(this, &CVTFEdit::txtVMTFile_SelectionChanged);
			this->SyntaxHilighter->Process();
			this->txtVMTFile->SelectionChanged += gcnew System::EventHandler(this, &CVTFEdit::txtVMTFile_SelectionChanged);
		}

		//
		// VMT context.
		//

		/*private: System::Void treVMTFile_MouseDown(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e)
		{
			if(e->Button == MouseButtons::Left || e->Button == MouseButtons::Right)
			{
				this->treVMTFile->SelectedNode = this->treVMTFile->GetNodeAt(e->X, e->Y);
			}
		}

		private: System::Void mnuVMTFile_Popup(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->btnVMTFileCopy->Visible = false;

			if(this->treVMTFile->SelectedNode == 0)
				return;

			this->btnVMTFileCopy->Visible = true;
		}

		private: System::Void btnVMTFileCopy_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			if(this->treVMTFile->SelectedNode == 0)
				return;

			System::Windows::Forms::Clipboard::SetDataObject(this->treVMTFile->SelectedNode->Text, true);
		}*/

		//
		// Helper functions.
		//

		private: System::String ^GetImageFormatString(VTFImageFormat ImageFormat)
		{
			SVTFImageFormatInfo ImageFormatInfo;
			if(vlImageGetImageFormatInfoEx(ImageFormat, &ImageFormatInfo))
			{
				return gcnew System::String(ImageFormatInfo.lpName);
			}
			return "";
		}

		//
		// Recent menu.
		//

		private: void AddRecentFile(System::String ^sFileName)
		{
			for(int i = 0; i < this->RecentFiles->Count; i++)
			{
				if(System::String::Compare(static_cast<System::String ^>(this->RecentFiles[i]), sFileName, true) == 0)
				{
					if(i == 0)
					{
						return;
					}
					else
					{
						this->RecentFiles->RemoveAt(i);
						this->RecentFiles->Insert(0, sFileName);

						this->UpdateRecentFiles();

						return;
					}
				}
			}

			this->RecentFiles->Insert(0, sFileName);

			while(this->RecentFiles->Count > (int)this->uiMaximumRecentFiles)
			{
				this->RecentFiles->RemoveAt(this->RecentFiles->Count - 1);
			}

			this->UpdateRecentFiles();
		}

		private: void UpdateRecentFiles()
		{
			this->btnRecentFiles->MenuItems->Clear();

			for(int i = 0; i < this->RecentFiles->Count; i++)
			{
				MenuItem ^RecentFile = gcnew MenuItem(static_cast<System::String ^>(this->RecentFiles[i]));
				RecentFile->Click += gcnew EventHandler(this, &CVTFEdit::btnRecentFile_Click);
				this->btnRecentFiles->MenuItems->Add(RecentFile);
			}

			this->btnRecentFiles->Visible = RecentFiles->Count != 0;
			this->btnFileSpace4->Visible = RecentFiles->Count != 0;
		}

		//
		// Configuration
		//

		private: bool WriteConfigFile(System::String ^sConfigFile)
		{
			System::IO::StreamWriter ^ConfigFile;

			try
			{
				ConfigFile = gcnew System::IO::StreamWriter(sConfigFile, false, System::Text::Encoding::ASCII);
			}
			catch(Exception ^)
			{
				return false;
			}

			//try
			//{
				ConfigFile->WriteLine("[VTFEdit]");
				ConfigFile->WriteLine("");

				ConfigFile->WriteLine(System::String::Concat("VTFEdit.AnimationFrameInterval = ", this->tmrAnimate->Interval.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFEdit.Mask = ", this->btnMask->Checked.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFEdit.Tile = ", this->btnTile->Checked.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFEdit.MipmapFullSize = ", this->btnMipmapFullSize->Checked.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFEdit.AutoCreateVMTFile = ", this->btnAutoCreateVMTFile->Checked.ToString()));

				ConfigFile->WriteLine("");
				ConfigFile->WriteLine("[Forms]");
				ConfigFile->WriteLine("");

				ConfigFile->WriteLine(System::String::Concat("Forms.VTFEdit.Location.X = ", this->FormSaveLocation.X.ToString()));
				ConfigFile->WriteLine(System::String::Concat("Forms.VTFEdit.Location.Y = ", this->FormSaveLocation.Y.ToString()));
				ConfigFile->WriteLine(System::String::Concat("Forms.VTFEdit.Size.Width = ", this->FormSaveSize.Width.ToString()));
				ConfigFile->WriteLine(System::String::Concat("Forms.VTFEdit.Size.Height = ", this->FormSaveSize.Height.ToString()));
				if(this->FormSaveWindowState == FormWindowState::Maximized)
				{
					ConfigFile->WriteLine("Forms.VTFEdit.WindowState = Maximized");
				}
				else
				{
					ConfigFile->WriteLine("Forms.VTFEdit.WindowState = Normal");
				}
				ConfigFile->WriteLine(System::String::Concat("Forms.VTFEdit.Sidebar.SplitPosition = ", this->iFormSaveSidebarSplitPosition.ToString()));
				ConfigFile->WriteLine(System::String::Concat("Forms.VTFEdit.SidebarRight.SplitPosition = ", this->iFormSaveSidebarRightSplitPosition.ToString()));

				ConfigFile->WriteLine(System::String::Concat("Forms.BatchConvert.InputFolder = ", this->BatchConvert->InputFolder));
				ConfigFile->WriteLine(System::String::Concat("Forms.BatchConvert.OutputFolder = ", this->BatchConvert->OutputFolder));
				ConfigFile->WriteLine(System::String::Concat("Forms.BatchConvert.ToVTF = ", this->BatchConvert->ToVTF.ToString()));
				ConfigFile->WriteLine(System::String::Concat("Forms.BatchConvert.ToVTFFilter = ", this->BatchConvert->ToVTFFilter));
				ConfigFile->WriteLine(System::String::Concat("Forms.BatchConvert.FromVTFFormat = ", this->BatchConvert->FromVTFFormat));
				ConfigFile->WriteLine(System::String::Concat("Forms.BatchConvert.FromVTFFilter = ", this->BatchConvert->FromVTFFilter));
				ConfigFile->WriteLine(System::String::Concat("Forms.BatchConvert.Recurse = ", this->BatchConvert->Recurse.ToString()));
				ConfigFile->WriteLine(System::String::Concat("Forms.BatchConvert.CreateVMTFiles = ", this->BatchConvert->CreateVMTFiles.ToString()));


				ConfigFile->WriteLine("");
				ConfigFile->WriteLine("[VTF Options]");
				ConfigFile->WriteLine("");

				ConfigFile->WriteLine(System::String::Concat("VTFOptions.NormalFormat = ", Convert::ToInt32(this->Options->NormalFormat).ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.AlphaFormat = ", Convert::ToInt32(this->Options->AlphaFormat).ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.TextureType = ", this->Options->TextureType.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.StripAlpha = ", this->Options->StripAlpha.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.sRGB = ", this->Options->sRGB.ToString()));

				ConfigFile->WriteLine(System::String::Concat("VTFOptions.Resize = ", this->Options->ResizeImage.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.ResizeMethod = ", Convert::ToInt32(this->Options->ResizeMethod).ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.ResizeFilter = ", Convert::ToInt32(this->Options->ResizeFilter).ToString()));

				ConfigFile->WriteLine(System::String::Concat("VTFOptions.ResizeClamp = ", this->Options->ResizeClamp.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.ResizeClampWidth = ", this->Options->ResizeClampWidth.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.ResizeClampHeight = ", this->Options->ResizeClampHeight.ToString()));

				ConfigFile->WriteLine(System::String::Concat("VTFOptions.GenerateMipmaps = ", this->Options->GenerateMipmaps.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.MipmapFilter = ", Convert::ToInt32(this->Options->MipmapFilter).ToString()));

				ConfigFile->WriteLine(System::String::Concat("VTFOptions.Version = ", this->Options->Version));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.AuxCompressionLevel = ", this->Options->AuxCompressionLevel.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.AuxCompressionMethod = ", this->Options->AuxCompressionMethod.ToString()));

				ConfigFile->WriteLine(System::String::Concat("VTFOptions.ComputeReflectivity = ", this->Options->ComputeReflectivity.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.GenerateThumbnail = ", this->Options->GenerateThumbnail.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.GenerateSphereMap = ", this->Options->GenerateSphereMap.ToString()));

				ConfigFile->WriteLine(System::String::Concat("VTFOptions.CorrectGamma = ", this->Options->CorrectGamma.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.GammaCorrection = ", this->Options->GammaCorrection.ToString()));

				ConfigFile->WriteLine(System::String::Concat("VTFOptions.LuminanceWeightR = ", this->Options->LuminanceWeightR.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.LuminanceWeightG = ", this->Options->LuminanceWeightG.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.LuminanceWeightB = ", this->Options->LuminanceWeightB.ToString()));

				ConfigFile->WriteLine(System::String::Concat("VTFOptions.CreateLODControlResource = ", this->Options->CreateLODControlResource.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.LODControlClampU = ", this->Options->LODControlClampU.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.LODControlClampV = ", this->Options->LODControlClampV.ToString()));

				ConfigFile->WriteLine(System::String::Concat("VTFOptions.CreateInformationResource = ", this->Options->CreateInformationResource.ToString()));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.InformationAuthor = ", this->Options->InformationAuthor));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.InformationContact = ", this->Options->InformationContact));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.InformationVersion = ", this->Options->InformationVersion));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.InformationModification = ", this->Options->InformationModification));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.InformationDescription = ", this->Options->InformationDescription));
				ConfigFile->WriteLine(System::String::Concat("VTFOptions.InformationComments = ", this->Options->InformationComments));

				ConfigFile->WriteLine("");
				ConfigFile->WriteLine("[Recent Files]");
				ConfigFile->WriteLine("");

				ConfigFile->WriteLine(System::String::Concat("RecentFiles.Maximum = ", this->uiMaximumRecentFiles.ToString()));

				for(int i = this->RecentFiles->Count - 1; i >= 0; i--)
				{
					ConfigFile->WriteLine(System::String::Concat("RecentFiles.File = ", static_cast<System::String ^>(this->RecentFiles[i])));
				}
			//}
			//catch(Exception ^)
			//{
				//ConfigFile->Close();
				//return false;
			//}

			ConfigFile->Close();

			return true;
		}

		private: bool ReadConfigFile(System::String ^sConfigFile)
		{
			System::IO::StreamReader ^ConfigFile;

			try
			{
				ConfigFile = gcnew System::IO::StreamReader(sConfigFile, System::Text::Encoding::ASCII);
			}
			catch(Exception ^)
			{
				return false;
			}

			try
			{
				System::String ^sLine, ^sArg, ^sVal;

				while((sLine = ConfigFile->ReadLine()) != nullptr)
				{
					int iIndex = sLine->IndexOf("=");

					if(iIndex == -1)
						continue;

					sArg = sLine->Substring(0, iIndex)->Trim();
					sVal = sLine->Substring(iIndex + 1)->Trim();

					try
					{
						if(System::String::Compare(sArg, "VTFEdit.AnimationFrameInterval", true) == 0)
						{
							this->tmrAnimate->Interval = Convert::ToUInt32(sVal);

							if(this->tmrAnimate->Interval < 10)
							{
								this->tmrAnimate->Interval = 10;
							}
							else if(this->tmrAnimate->Interval > 1000)
							{
								this->tmrAnimate->Interval = 1000;
							}

							this->numAnimateFPS->Value = System::Decimal(1000 / this->tmrAnimate->Interval);
						}
						else if(System::String::Compare(sArg, "VTFEdit.Mask", true) == 0)
						{
							this->btnMask->Checked = Convert::ToBoolean(sVal);
						}
						else if(System::String::Compare(sArg, "VTFEdit.Tile", true) == 0)
						{
							this->btnTile->Checked = Convert::ToBoolean(sVal);
						}
						else if(System::String::Compare(sArg, "VTFEdit.MipmapFullSize", true) == 0)
						{
							this->btnMipmapFullSize->Checked = Convert::ToBoolean(sVal);
						}
						else if(System::String::Compare(sArg, "VTFEdit.AutoCreateVMTFile", true) == 0)
						{
							this->btnAutoCreateVMTFile->Checked = Convert::ToBoolean(sVal);
						}
						else if(System::String::Compare(sArg, "Forms.VTFEdit.Location.X", true) == 0)
						{
							this->FormSaveLocation.X = Convert::ToInt32(sVal);
						}
						else if(System::String::Compare(sArg, "Forms.VTFEdit.Location.Y", true) == 0)
						{
							this->FormSaveLocation.Y = Convert::ToInt32(sVal);
						}
						else if(System::String::Compare(sArg, "Forms.VTFEdit.Size.Width", true) == 0)
						{
							this->FormSaveSize.Width = Convert::ToInt32(sVal);
						}
						else if(System::String::Compare(sArg, "Forms.VTFEdit.Size.Height", true) == 0)
						{
							this->FormSaveSize.Height = Convert::ToInt32(sVal);
						}
						else if(System::String::Compare(sArg, "Forms.VTFEdit.WindowState", true) == 0)
						{
							if(System::String::Compare(sVal, "Normal", true) == 0)
							{
								this->FormSaveWindowState = FormWindowState::Normal;
							}
							else if(System::String::Compare(sVal, "Maximized", true) == 0)
							{
								this->FormSaveWindowState = FormWindowState::Maximized;
							}
						}
						else if(System::String::Compare(sArg, "Forms.VTFEdit.Sidebar.SplitPosition", true) == 0)
						{
							this->iFormSaveSidebarSplitPosition = Convert::ToInt32(sVal);
						}
						else if(System::String::Compare(sArg, "Forms.VTFEdit.SidebarRight.SplitPosition", true) == 0)
						{
							this->iFormSaveSidebarRightSplitPosition = Convert::ToInt32(sVal);
						}

						else if(System::String::Compare(sArg, "Forms.BatchConvert.InputFolder", true) == 0)
						{
							this->BatchConvert->InputFolder = sVal;
						}
						else if(System::String::Compare(sArg, "Forms.BatchConvert.OutputFolder", true) == 0)
						{
							this->BatchConvert->OutputFolder = sVal;
						}
						else if(System::String::Compare(sArg, "Forms.BatchConvert.ToVTF", true) == 0)
						{
							this->BatchConvert->ToVTF = Convert::ToBoolean(sVal);
						}
						else if(System::String::Compare(sArg, "Forms.BatchConvert.ToVTFFilter", true) == 0)
						{
							this->BatchConvert->ToVTFFilter = sVal;
						}
						else if(System::String::Compare(sArg, "Forms.BatchConvert.FromVTFFormat", true) == 0)
						{
							this->BatchConvert->FromVTFFormat = sVal;
						}
						else if(System::String::Compare(sArg, "Forms.BatchConvert.FromVTFFilter", true) == 0)
						{
							this->BatchConvert->FromVTFFilter = sVal;
						}
						else if(System::String::Compare(sArg, "Forms.BatchConvert.Recurse", true) == 0)
						{
							this->BatchConvert->Recurse = Convert::ToBoolean(sVal);
						}
						else if(System::String::Compare(sArg, "Forms.BatchConvert.CreateVMTFiles", true) == 0)
						{
							this->BatchConvert->CreateVMTFiles = Convert::ToBoolean(sVal);
						}

						else if(System::String::Compare(sArg, "VTFOptions.NormalFormat", true) == 0)
						{
							this->Options->NormalFormat = (VTFImageFormat)Convert::ToInt32(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.AlphaFormat", true) == 0)
						{
							this->Options->AlphaFormat = (VTFImageFormat)Convert::ToInt32(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.TextureType", true) == 0)
						{
							this->Options->TextureType = Convert::ToInt32(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.StripAlpha", true) == 0)
						{
							this->Options->StripAlpha = Convert::ToByte(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.sRGB", true) == 0)
						{
							this->Options->sRGB = Convert::ToByte(sVal);
						}

						else if(System::String::Compare(sArg, "VTFOptions.Resize", true) == 0)
						{
							this->Options->ResizeImage = Convert::ToByte(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.ResizeMethod", true) == 0)
						{
							this->Options->ResizeMethod = (VTFResizeMethod)Convert::ToInt32(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.ResizeFilter", true) == 0)
						{
							this->Options->ResizeFilter = (VTFMipmapFilter)Convert::ToInt32(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.ResizeClamp", true) == 0)
						{
							this->Options->ResizeClamp = Convert::ToByte(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.ResizeClampWidth", true) == 0)
						{
							this->Options->ResizeClampWidth = Convert::ToInt32(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.ResizeClampHeight", true) == 0)
						{
							this->Options->ResizeClampHeight = Convert::ToInt32(sVal);
						}

						else if(System::String::Compare(sArg, "VTFOptions.GenerateMipmaps", true) == 0)
						{
							this->Options->GenerateMipmaps = Convert::ToByte(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.MipmapFilter", true) == 0)
						{
							this->Options->MipmapFilter = (VTFMipmapFilter)Convert::ToInt32(sVal);
						}

						else if(System::String::Compare(sArg, "VTFOptions.Version", true) == 0)
						{
							this->Options->Version = sVal;
						}
						else if(System::String::Compare(sArg, "VTFOptions.AuxCompressionLevel", true) == 0)
						{
							this->Options->AuxCompressionLevel = Convert::ToInt16(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.AuxCompressionMethod", true) == 0)
						{
							this->Options->AuxCompressionMethod = Convert::ToInt16(sVal);
						}

						else if(System::String::Compare(sArg, "VTFOptions.ComputeReflectivity", true) == 0)
						{
							this->Options->ComputeReflectivity = Convert::ToByte(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.GenerateThumbnail", true) == 0)
						{
							this->Options->GenerateThumbnail = Convert::ToByte(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.GenerateSphereMap", true) == 0)
						{
							this->Options->GenerateSphereMap = Convert::ToByte(sVal);
						}

						else if(System::String::Compare(sArg, "VTFOptions.CorrectGamma", true) == 0)
						{
							this->Options->CorrectGamma = Convert::ToByte(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.GammaCorrection", true) == 0)
						{
							this->Options->GammaCorrection = Convert::ToSingle(sVal);
						}

						else if(System::String::Compare(sArg, "VTFOptions.LuminanceWeightR", true) == 0)
						{
							this->Options->LuminanceWeightR = Convert::ToSingle(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.LuminanceWeightG", true) == 0)
						{
							this->Options->LuminanceWeightG = Convert::ToSingle(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.LuminanceWeightB", true) == 0)
						{
							this->Options->LuminanceWeightB = Convert::ToSingle(sVal);
						}

						else if(System::String::Compare(sArg, "VTFOptions.CreateLODControlResource", true) == 0)
						{
							this->Options->CreateLODControlResource = Convert::ToByte(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.LODControlClampU", true) == 0)
						{
							this->Options->LODControlClampU = Convert::ToByte(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.LODControlClampV", true) == 0)
						{
							this->Options->LODControlClampV = Convert::ToByte(sVal);
						}

						else if(System::String::Compare(sArg, "VTFOptions.CreateInformationResource", true) == 0)
						{
							this->Options->CreateInformationResource = Convert::ToByte(sVal);
						}
						else if(System::String::Compare(sArg, "VTFOptions.InformationAuthor", true) == 0)
						{
							this->Options->InformationAuthor = sVal;
						}
						else if(System::String::Compare(sArg, "VTFOptions.InformationContact", true) == 0)
						{
							this->Options->InformationContact = sVal;
						}
						else if(System::String::Compare(sArg, "VTFOptions.InformationVersion", true) == 0)
						{
							this->Options->InformationVersion = sVal;
						}
						else if(System::String::Compare(sArg, "VTFOptions.InformationModification", true) == 0)
						{
							this->Options->InformationModification = sVal;
						}
						else if(System::String::Compare(sArg, "VTFOptions.InformationDescription", true) == 0)
						{
							this->Options->InformationDescription = sVal;
						}
						else if(System::String::Compare(sArg, "VTFOptions.InformationComments", true) == 0)
						{
							this->Options->InformationComments = sVal;
						}

						else if(System::String::Compare(sArg, "RecentFiles.Maximum", true) == 0)
						{
							this->uiMaximumRecentFiles = Convert::ToUInt32(sVal);

							if(this->uiMaximumRecentFiles > 16)
							{
								this->uiMaximumRecentFiles = 16;
							}
						}
						else if(System::String::Compare(sArg, "RecentFiles.File", true) == 0 && System::IO::File::Exists(sVal))
						{
							this->AddRecentFile(sVal);
						}
					}
					catch(Exception ^)
					{
						continue;
					}
				}
			}
			catch(Exception ^)
			{
				ConfigFile->Close();
				return false;
			}

			ConfigFile->Close();

			return true;
		}

		private: System::Void Control_DragDrop(System::Object ^  sender, System::Windows::Forms::DragEventArgs ^  e)
		{
			array< System::String^>^ lpFiles = static_cast<array< System::String^>^>(e->Data->GetData(System::Windows::Forms::DataFormats::FileDrop));
			if(lpFiles->Length > 0)
			{
				if(lpFiles[0]->ToLower()->EndsWith(".vtf") || lpFiles[0]->ToLower()->EndsWith(".vmt"))
				{
					this->Open(lpFiles[0], false);
				}
				else
				{
					// drop order is not defined by the shell, so sort the frames/faces by name
					array< System::String ^>^ sFileNames = static_cast<array< System::String ^>^>(lpFiles->Clone());
					System::Array::Sort(sFileNames, System::StringComparer::OrdinalIgnoreCase);

					this->Import(sFileNames);
				}
			}
		}

		private: System::Void Control_DragEnter(System::Object ^  sender, System::Windows::Forms::DragEventArgs ^  e)
		{
			if(e->Data->GetDataPresent(System::Windows::Forms::DataFormats::FileDrop, false))
			{
				array< System::String^>^ lpFiles = static_cast<array< System::String^>^>(e->Data->GetData(System::Windows::Forms::DataFormats::FileDrop));
				if(lpFiles->Length > 0)
				{
					// VTF/VMT files are opened and anything else is imported
					// accept any file (but not directories)
					if(!System::IO::Directory::Exists(lpFiles[0]))
					{
						e->Effect = System::Windows::Forms::DragDropEffects::All;
					}
				}
			}
		}
	};
}