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

#include "MainWindow.h"

#include "AboutDialog.h"
#include "BatchConvertDialog.h"
#include "FileDialogHistory.h"
#include "HotspotDialog.h"
#include "ImageView.h"
#include "SheetDialog.h"
#include "VmtCreateDialog.h"
#include "VmtEditorOptionsDialog.h"
#include "VmtFileUtility.h"
#include "VtfFileUtility.h"
#include "VtfOptionsDialog.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QDragEnterEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QListWidget>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QStatusBar>
#include <QStyleHints>
#include <QTabBar>
#include <QTabWidget>
#include <QTextDocument>
#include <QTextEdit>
#include <QTextStream>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QWindow>

#include <algorithm>
#include <vector>

namespace VTFEdit
{
	namespace
	{
		const char *const FlagNames[] =
		{
			"Point Sample", 
			"Trilinear",
			"Clamp S",
			"Clamp T",
			"Anisotropic",
			"Unused [5]", 
			"sRGB",
			"Normal Map", 
			"No Mipmaps", 
			"No LOD",
			"No Minimum Mipmap",
			"Procedural",
			"One Bit Alpha",
			"Eight Bit Alpha",
			"Enviroment Map", 
			"Render Target", 
			"Depth Render Target",
			"No Debug Override",
			"Single Copy",
			"Unused [19]", 
			"Unused [20]", 
			"Unused [21]",
			"Unused [22]",
			"No Depth Buffer", 
			"Unused [24]", 
			"Clamp U", 
			"Vertex Texture",
			"SSBump", 
			"Unused [28]",
			"Border",
			"Unused [30]",
			"Unused [31]"
		};
		const int FlagCount = static_cast<int>(sizeof(FlagNames) / sizeof(FlagNames[0]));

		QString imageFormatString(VTFImageFormat ImageFormat)
		{
			SVTFImageFormatInfo ImageFormatInfo;
			if(vlImageGetImageFormatInfoEx(ImageFormat, &ImageFormatInfo))
			{
				return QString::fromLatin1(ImageFormatInfo.lpName);
			}
			return QString();
		}

		QString hex32(vlUInt uiValue)
		{
			return QStringLiteral("%1").arg(uiValue, 8, 16, QLatin1Char('0')).toUpper();
		}

		QString lastErrorString()
		{
			return QString::fromLatin1(vlGetLastError());
		}

		bool IsKeyValuesFileName(const QString &sFileName)
		{
			return sFileName.endsWith(QLatin1String(".vmt"), Qt::CaseInsensitive)
				|| sFileName.endsWith(QLatin1String(".rect"), Qt::CaseInsensitive);
		}

		bool IsImportableFileName(const QString &sFileName)
		{
			// keeo in sync with onImport
			static const char *const szExtensions[] =
			{
				".bmp", ".dds", ".exr", ".gif", ".hdr", ".jpg", ".jpeg", ".pfm", ".png", ".tga"
			};

			for(const char *const szExtension : szExtensions)
			{
				if(sFileName.endsWith(QLatin1String(szExtension), Qt::CaseInsensitive))
				{
					return true;
				}
			}

			return false;
		}

		bool IsSupportedFileName(const QString &sFileName)
		{
			return sFileName.endsWith(QLatin1String(".vtf"), Qt::CaseInsensitive)
				|| IsKeyValuesFileName(sFileName)
				|| IsImportableFileName(sFileName);
		}

		QLabel *addInfoRow(QFormLayout *pForm, const QString &sLabel)
		{
			QLabel *pValue = new QLabel(pForm->parentWidget());
			pValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
			pForm->addRow(sLabel, pValue);
			return pValue;
		}
	}

	MainWindow::MainWindow()
		: m_iCurrentDocument(-1)
		, m_iUntitledCounter(0)
		, m_bSwitchingDocument(false)
		, m_pVMTFile(nullptr)
		, m_pVTFFile(nullptr)
		, m_fImageScale(1.0f)
		, m_fEffectiveImageScale(1.0f)
		, m_pDecodedVTFFile(nullptr)
		, m_uiDecodedWidth(0)
		, m_uiDecodedHeight(0)
		, m_uiDecodedFrame(0)
		, m_uiDecodedFace(0)
		, m_uiDecodedSlice(0)
		, m_uiDecodedMipmap(0)
		, m_sDecodedExposure(0.0f)
		, m_iCompositeChannel(-1)
		, m_bCompositeMask(false)
		, m_bCompositeValid(false)
		, m_bImagePanning(false)
		, m_bUpdatingVtfFile(false)
		, m_bUpdatingFlags(false)
		, m_bUpdatingFileInfo(false)
		, m_bHdrResetting(false)
		, m_iMaximumRecentFiles(8)
		, m_pOptionsDialog(nullptr)
		, m_pVmtCreateDialog(nullptr)
		, m_pVmtEditorOptionsDialog(nullptr)
		, m_pBatchConvertDialog(nullptr)
		, m_pAboutDialog(nullptr)
		, m_iVmtErrorLine(0)
		, m_iSidebarSplit(258)
		, m_iSidebarRightSplit(258)
	{
		setWindowTitle(QApplication::applicationName());
		setAcceptDrops(true);
		resize(1024, 600);

		createActions();
		createMenus();
		createToolBar();
		createCentralWidget();
		createStatusBar();

		m_pAnimateTimer = new QTimer(this);
		m_pAnimateTimer->setInterval(1000 / 24);
		connect(m_pAnimateTimer, &QTimer::timeout, this, &MainWindow::onAnimateTick);

		connect(QApplication::clipboard(), &QClipboard::dataChanged,
			this, &MainWindow::onClipboardChanged);

		onHdrReset();
		clearWidgets();
		updateActions();

		readConfigFile(configFilePath());
		updateRecentFiles();
		onClipboardChanged();
	}

	MainWindow::~MainWindow()
	{
		for(Document *pDocument : m_Documents)
		{
			delete pDocument->pVMTFile;
			delete pDocument->pVTFFile;
			delete pDocument;
		}
	}

	//
	// Construction.
	//

	void MainWindow::createActions()
	{
		m_pNewAction = new QAction(tr("&New"), this);
		m_pNewAction->setShortcut(QKeySequence::New);
		connect(m_pNewAction, &QAction::triggered, this, &MainWindow::onNew);

		m_pOpenAction = new QAction(tr("&Open"), this);
		m_pOpenAction->setShortcut(QKeySequence::Open);
		connect(m_pOpenAction, &QAction::triggered, this, &MainWindow::onOpen);

		m_pSaveAction = new QAction(tr("&Save"), this);
		m_pSaveAction->setShortcut(QKeySequence::Save);
		connect(m_pSaveAction, &QAction::triggered, this, &MainWindow::onSave);

		m_pSaveAsAction = new QAction(tr("Save &As..."), this);
		m_pSaveAsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
		connect(m_pSaveAsAction, &QAction::triggered, this, &MainWindow::onSaveAs);

		m_pSaveAllAction = new QAction(tr("Save A&ll"), this);
		m_pSaveAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_S));
		connect(m_pSaveAllAction, &QAction::triggered, this, &MainWindow::onSaveAll);

		m_pCloseAction = new QAction(tr("&Close"), this);
		m_pCloseAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
		connect(m_pCloseAction, &QAction::triggered, this, &MainWindow::onClose);

		m_pCloseAllAction = new QAction(tr("Close A&ll"), this);
		m_pCloseAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_W));
		connect(m_pCloseAllAction, &QAction::triggered, this, &MainWindow::onCloseAll);

		m_pReopenRecentAction = new QAction(tr("&Reopen Recent File"), this);
		m_pReopenRecentAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T));
		connect(m_pReopenRecentAction, &QAction::triggered, this, &MainWindow::onReopenRecent);

		m_pNextTabAction = new QAction(tr("&Next Tab"), this);
		m_pNextTabAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Tab));
		connect(m_pNextTabAction, &QAction::triggered, this, &MainWindow::onNextTab);
		addAction(m_pNextTabAction);

		m_pPreviousTabAction = new QAction(tr("&Previous Tab"), this);
		m_pPreviousTabAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab));
		connect(m_pPreviousTabAction, &QAction::triggered, this, &MainWindow::onPreviousTab);
		addAction(m_pPreviousTabAction);

		// Ctrl+1 through Ctrl+8 select a tab by position
		// Ctrl+9 selects the last tab
		for(int i = 0; i < 9; i++)
		{
			m_pTabIndexActions[i] = new QAction(this);
			m_pTabIndexActions[i]->setShortcut(QKeySequence(Qt::CTRL | (Qt::Key_1 + i)));
			const int iIndex = i == 8 ? -1 : i;
			connect(m_pTabIndexActions[i], &QAction::triggered, this,
				[this, iIndex]() { switchToTab(iIndex); });
			addAction(m_pTabIndexActions[i]);
		}

		m_pImportAction = new QAction(tr("&Import"), this);
		m_pImportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
		connect(m_pImportAction, &QAction::triggered, this, &MainWindow::onImport);

		m_pExportAction = new QAction(tr("&Export"), this);
		m_pExportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
		connect(m_pExportAction, &QAction::triggered, this, &MainWindow::onExport);

		m_pExportAllAction = new QAction(tr("&Export All"), this);
		m_pExportAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_E));
		connect(m_pExportAllAction, &QAction::triggered, this, &MainWindow::onExportAll);

		m_pExitAction = new QAction(tr("E&xit"), this);
		m_pExitAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_F4));
		connect(m_pExitAction, &QAction::triggered, this, &QWidget::close);

		m_pCopyAction = new QAction(tr("&Copy"), this);
		m_pCopyAction->setShortcut(QKeySequence::Copy);
		connect(m_pCopyAction, &QAction::triggered, this, &MainWindow::onCopy);

		m_pPasteAction = new QAction(tr("&Paste"), this);
		m_pPasteAction->setShortcut(QKeySequence::Paste);
		connect(m_pPasteAction, &QAction::triggered, this, &MainWindow::onPaste);

		m_pChannelGroup = new QActionGroup(this);
		struct { QAction **ppAction; const char *pText; const char *pIcon; const char *pToolTip; Qt::Key Key; } Channels[] =
		{
			{ &m_pChannelRgbAction, "RGB", ":/icons/rgb.png", QT_TR_NOOP("RGB Channels"), Qt::Key_C },
			{ &m_pChannelRAction, "R", ":/icons/red.png", QT_TR_NOOP("Red Channel"), Qt::Key_R },
			{ &m_pChannelGAction, "G", ":/icons/green.png", QT_TR_NOOP("Green Channel"), Qt::Key_G },
			{ &m_pChannelBAction, "B", ":/icons/blue.png", QT_TR_NOOP("Blue Channel"), Qt::Key_B },
			{ &m_pChannelAAction, "A", ":/icons/alpha.png", QT_TR_NOOP("Alpha Channel"), Qt::Key_A },
		};
		for(auto &Channel : Channels)
		{
			QAction *pAction = new QAction(QString::fromLatin1(Channel.pText), this);
			pAction->setCheckable(true);
			pAction->setIcon(QIcon(QLatin1String(Channel.pIcon)));
			pAction->setToolTip(tr(Channel.pToolTip));
			pAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Channel.Key));
			m_pChannelGroup->addAction(pAction);
			connect(pAction, &QAction::triggered, this, &MainWindow::onChannelChanged);
			*Channel.ppAction = pAction;
		}
		m_pChannelRgbAction->setChecked(true);

		m_pMaskAction = new QAction(tr("&Mask"), this);
		m_pMaskAction->setCheckable(true);
		m_pMaskAction->setIcon(QIcon(QStringLiteral(":/icons/alphamask.png")));
		m_pMaskAction->setToolTip(tr("Alpha Mask"));
		m_pMaskAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_M));
		connect(m_pMaskAction, &QAction::triggered, this, &MainWindow::onViewOptionChanged);

		m_pTileAction = new QAction(tr("&Tile"), this);
		m_pTileAction->setCheckable(true);
		m_pTileAction->setIcon(QIcon(QStringLiteral(":/icons/tile.png")));
		m_pTileAction->setToolTip(tr("Tile Image"));
		m_pTileAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
		connect(m_pTileAction, &QAction::triggered, this, &MainWindow::onViewOptionChanged);

		m_pMipmapFullSizeAction = new QAction(tr("&Zoom Mipmaps"), this);
		m_pMipmapFullSizeAction->setCheckable(true);
		m_pMipmapFullSizeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_M));
		connect(m_pMipmapFullSizeAction, &QAction::triggered, this, &MainWindow::onViewOptionChanged);

		m_pEditSheetAction = new QAction(tr("&Edit..."), this);
		connect(m_pEditSheetAction, &QAction::triggered, this, &MainWindow::onEditSheet);

		m_pRemoveSheetAction = new QAction(tr("&Remove"), this);
		connect(m_pRemoveSheetAction, &QAction::triggered, this, &MainWindow::onRemoveSheet);

		m_pEditHotspotAction = new QAction(tr("&Edit..."), this);
		connect(m_pEditHotspotAction, &QAction::triggered, this, &MainWindow::onEditHotspot);

		m_pRemoveHotspotAction = new QAction(tr("&Remove"), this);
		connect(m_pRemoveHotspotAction, &QAction::triggered, this, &MainWindow::onRemoveHotspot);

		m_pCreateVmtFileAction = new QAction(tr("Create &VMT File"), this);
		connect(m_pCreateVmtFileAction, &QAction::triggered, this, &MainWindow::onCreateVmtFile);

		m_pConvertFolderAction = new QAction(tr("Convert &Folder"), this);
		connect(m_pConvertFolderAction, &QAction::triggered, this, &MainWindow::onConvertFolder);

		m_pAutoCreateVmtFileAction = new QAction(tr("&Auto Create VMT File"), this);
		m_pAutoCreateVmtFileAction->setCheckable(true);

		m_pSingleInstanceAction = new QAction(tr("&Single Instance"), this);
		m_pSingleInstanceAction->setCheckable(true);
		m_pSingleInstanceAction->setChecked(true);
		m_pSingleInstanceAction->setToolTip(tr("Open files as tabs in this window instead of a new window"));
		connect(m_pSingleInstanceAction, &QAction::toggled, this, &MainWindow::singleInstanceChanged);

		m_pVmtEditorOptionsAction = new QAction(tr("VMT &Editor Options..."), this);
		connect(m_pVmtEditorOptionsAction, &QAction::triggered, this, &MainWindow::onVmtEditorOptions);

		m_pAboutAction = new QAction(tr("&About"), this);
		connect(m_pAboutAction, &QAction::triggered, this, &MainWindow::onAbout);
	}

	void MainWindow::createMenus()
	{
		QMenu *pFileMenu = menuBar()->addMenu(tr("&File"));
		pFileMenu->addAction(m_pNewAction);
		pFileMenu->addAction(m_pOpenAction);
		pFileMenu->addSeparator();
		pFileMenu->addAction(m_pSaveAction);
		pFileMenu->addAction(m_pSaveAsAction);
		pFileMenu->addAction(m_pSaveAllAction);
		pFileMenu->addSeparator();
		pFileMenu->addAction(m_pCloseAction);
		pFileMenu->addAction(m_pCloseAllAction);
		pFileMenu->addSeparator();
		pFileMenu->addAction(m_pImportAction);
		pFileMenu->addAction(m_pExportAction);
		pFileMenu->addAction(m_pExportAllAction);
		pFileMenu->addSeparator();
		m_pRecentFilesMenu = pFileMenu->addMenu(tr("&Recent Files"));
		pFileMenu->addAction(m_pReopenRecentAction);
		pFileMenu->addSeparator();
		pFileMenu->addAction(m_pExitAction);

		QMenu *pEditMenu = menuBar()->addMenu(tr("&Edit"));
		pEditMenu->addAction(m_pCopyAction);
		pEditMenu->addAction(m_pPasteAction);

		QMenu *pViewMenu = menuBar()->addMenu(tr("&View"));
		QMenu *pChannelMenu = pViewMenu->addMenu(tr("&Channel"));
		pChannelMenu->addActions(m_pChannelGroup->actions());
		pViewMenu->addAction(m_pMaskAction);
		pViewMenu->addAction(m_pTileAction);
		pViewMenu->addAction(m_pMipmapFullSizeAction);
		pViewMenu->addSeparator();
		pViewMenu->addAction(m_pNextTabAction);
		pViewMenu->addAction(m_pPreviousTabAction);

		QMenu *pToolsMenu = menuBar()->addMenu(tr("&Tools"));
		pToolsMenu->addAction(m_pCreateVmtFileAction);
		pToolsMenu->addAction(m_pConvertFolderAction);

		m_pTextureMenu = menuBar()->addMenu(tr("&Texture"));

		QMenu* pSheetMenu = m_pTextureMenu->addMenu(tr("Sprite &Sheet"));
		pSheetMenu->setToolTipsVisible(true);
		pSheetMenu->addAction(m_pEditSheetAction);
		pSheetMenu->addAction(m_pRemoveSheetAction);

		QMenu* pHotspotMenu = m_pTextureMenu->addMenu(tr("&Hotspot Rectangles"));
		pHotspotMenu->setToolTipsVisible(true);
		pHotspotMenu->addAction(m_pEditHotspotAction);
		pHotspotMenu->addAction(m_pRemoveHotspotAction);

		QMenu *pOptionsMenu = menuBar()->addMenu(tr("&Options"));
		pOptionsMenu->addAction(m_pAutoCreateVmtFileAction);
		pOptionsMenu->addAction(m_pSingleInstanceAction);
		pOptionsMenu->addSeparator();
		pOptionsMenu->addAction(m_pVmtEditorOptionsAction);

		QMenu *pHelpMenu = menuBar()->addMenu(tr("&Help"));
		pHelpMenu->addAction(m_pAboutAction);
	}

	void MainWindow::createToolBar()
	{
		QToolBar *pToolBar = addToolBar(tr("Main"));
		pToolBar->setObjectName(QStringLiteral("MainToolBar"));
		pToolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
		pToolBar->addAction(m_pImportAction);
		pToolBar->addAction(m_pOpenAction);
		pToolBar->addAction(m_pSaveAction);
		pToolBar->addSeparator();
		pToolBar->addAction(m_pCopyAction);
		pToolBar->addAction(m_pPasteAction);
		pToolBar->addSeparator();

		QAction *ViewActions[] =
		{
			m_pChannelRgbAction, m_pChannelRAction, m_pChannelGAction,
			m_pChannelBAction, m_pChannelAAction, m_pMaskAction, m_pTileAction,
		};
		for(QAction *pAction : ViewActions)
		{
			pToolBar->addAction(pAction);
			if(QToolButton *pButton = qobject_cast<QToolButton *>(pToolBar->widgetForAction(pAction)))
				pButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
		}
	}

	void MainWindow::createStatusBar()
	{
		m_pStatusFileName = new QLabel(this);
		m_pStatusInfo1 = new QLabel(this);
		m_pStatusInfo2 = new QLabel(this);

		m_pStatusInfo1->setMinimumWidth(120);
		m_pStatusInfo2->setMinimumWidth(120);

		statusBar()->addWidget(m_pStatusFileName, 1);
		statusBar()->addPermanentWidget(m_pStatusInfo1);
		statusBar()->addPermanentWidget(m_pStatusInfo2);
	}

	void MainWindow::createCentralWidget()
	{
		m_pLeftTabs = new QTabWidget(this);
		m_pRightTabs = new QTabWidget(this);

		m_pImageTab = createImageTab();
		m_pInfoTab = createInfoTab();
		m_pResourcesTab = createResourcesTab();

		m_pImageTab->hide();
		m_pInfoTab->hide();
		m_pResourcesTab->hide();

		// Image view page.
		m_pImageView = new ImageView(this);
		connect(m_pImageView, &ImageView::mouseMovedOverImage, this, &MainWindow::onImageMouseMoved);
		connect(m_pImageView, &QWidget::customContextMenuRequested, this, &MainWindow::onImageContextMenu);

		m_pImageContextMenu = new QMenu(this);
		QAction *pZoomIn = m_pImageContextMenu->addAction(tr("Zoom &In"));
		connect(pZoomIn, &QAction::triggered, this, &MainWindow::onZoomIn);
		QAction *pZoomOut = m_pImageContextMenu->addAction(tr("Zoom &Out"));
		connect(pZoomOut, &QAction::triggered, this, &MainWindow::onZoomOut);
		m_pImageContextMenu->addSeparator();
		QAction *pZoomReset = m_pImageContextMenu->addAction(tr("&Reset Zoom/Pan"));
		connect(pZoomReset, &QAction::triggered, this, &MainWindow::onZoomReset);
		m_pImageContextMenu->addSeparator();
		m_pImageContextMenu->addAction(m_pCopyAction);

		m_pImageScrollArea = new QScrollArea(this);
		m_pImageScrollArea->setWidget(m_pImageView);
		m_pImageScrollArea->setAlignment(Qt::AlignCenter);
		m_pImageScrollArea->setWidgetResizable(false);
		m_pImageScrollArea->viewport()->installEventFilter(this);
		m_pImageView->installEventFilter(this);
		{
			// Grey backdrop
			const bool bDark = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
			const QColor Background = bDark ? QColor(60, 60, 60) : QColor(180, 180, 180);

			QWidget *pViewport = m_pImageScrollArea->viewport();
			pViewport->setBackgroundRole(QPalette::Window);
			pViewport->setAutoFillBackground(true);

			QPalette Palette = pViewport->palette();
			Palette.setColor(QPalette::Window, Background);
			pViewport->setPalette(Palette);
		}

		// VMT editor page.
		m_pVmtEdit = new VmtTextEdit(this);
		m_pVmtEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
		m_pVmtEdit->viewport()->setAcceptDrops(false);
		m_pVmtEdit->installEventFilter(this);

		m_pEmptyDocument = new QTextDocument(this);
		m_pEmptyDocument->setDocumentLayout(new QPlainTextDocumentLayout(m_pEmptyDocument));
		m_pVmtEdit->setDocument(m_pEmptyDocument);

		applyVmtEditorSettings();

		// Follow the system theme
		connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, [this](Qt::ColorScheme)
		{
			if(m_VmtEditorSettings.eTheme == VmtEditorTheme::System)
			{
				applyVmtEditorSettings();
			}
		});

		connect(m_pVmtEdit, &QPlainTextEdit::textChanged, this, &MainWindow::onVmtTextChanged);
		connect(m_pVmtEdit, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::onVmtCursorChanged);

		m_pVmtEdit->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(m_pVmtEdit, &QWidget::customContextMenuRequested, this, &MainWindow::onVmtContextMenu);

		m_pCentralStack = new QStackedWidget(this);
		m_pCentralStack->addWidget(m_pImageScrollArea);
		m_pCentralStack->addWidget(m_pVmtEdit);

		m_pSplitter = new QSplitter(Qt::Horizontal, this);
		m_pSplitter->addWidget(m_pLeftTabs);
		m_pSplitter->addWidget(m_pCentralStack);
		m_pSplitter->addWidget(m_pRightTabs);
		m_pSplitter->setStretchFactor(0, 0);
		m_pSplitter->setStretchFactor(1, 1);
		m_pSplitter->setStretchFactor(2, 0);

		m_pSplitter->setHandleWidth(6);

		connect(m_pSplitter, &QSplitter::splitterMoved, this, [this](int, int)
		{
			rememberSidebarSizes();
		});

		m_pTabBar = new QTabBar(this);
		m_pTabBar->setTabsClosable(true);
		m_pTabBar->setMovable(true);
		m_pTabBar->setDocumentMode(true);
		m_pTabBar->setExpanding(false);
		m_pTabBar->setUsesScrollButtons(true);
		m_pTabBar->setDrawBase(false);
		m_pTabBar->hide();

		connect(m_pTabBar, &QTabBar::currentChanged, this, &MainWindow::onTabChanged);
		connect(m_pTabBar, &QTabBar::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
		connect(m_pTabBar, &QTabBar::tabMoved, this, &MainWindow::onTabMoved);

		QWidget *pCentral = new QWidget(this);
		QVBoxLayout *pCentralLayout = new QVBoxLayout(pCentral);
		pCentralLayout->setContentsMargins(0, 0, 0, 0);
		pCentralLayout->setSpacing(0);
		pCentralLayout->addWidget(m_pTabBar);
		pCentralLayout->addWidget(m_pSplitter, 1);

		setCentralWidget(pCentral);
	}

	QWidget *MainWindow::createImageTab()
	{
		QWidget *pTab = new QWidget(this);
		QVBoxLayout *pLayout = new QVBoxLayout(pTab);

		QGroupBox *pImage = new QGroupBox(tr("Image:"), pTab);
		QFormLayout *pForm = new QFormLayout(pImage);

		struct { QSpinBox **ppSpin; const char *pLabel; } Spins[] =
		{
			{ &m_pFrame, "Frame:" },
			{ &m_pFace, "Face:" },
			{ &m_pSlice, "Slice:" },
			{ &m_pMipmap, "Mipmap:" },
		};
		for(auto &Spin : Spins)
		{
			QSpinBox *pSpin = new QSpinBox(pImage);
			pSpin->setRange(0, 0);
			connect(pSpin, &QSpinBox::valueChanged, this, &MainWindow::onImageParameterChanged);
			pForm->addRow(tr(Spin.pLabel), pSpin);
			*Spin.ppSpin = pSpin;
		}

		m_pHdrExposure = new QSlider(Qt::Horizontal, pImage);
		connect(m_pHdrExposure, &QSlider::valueChanged, this, &MainWindow::onImageParameterChanged);
		pForm->addRow(tr("Exposure:"), m_pHdrExposure);

		m_pAnimateFps = new QSpinBox(pImage);
		m_pAnimateFps->setRange(1, 100);
		m_pAnimateFps->setValue(24);
		connect(m_pAnimateFps, &QSpinBox::valueChanged, this, &MainWindow::onAnimateFpsChanged);
		pForm->addRow(tr("Framerate:"), m_pAnimateFps);

		m_pAnimateButton = new QPushButton(tr("&Play"), pImage);
		connect(m_pAnimateButton, &QPushButton::clicked, this, &MainWindow::onAnimateClicked);
		pForm->addRow(m_pAnimateButton);

		QGroupBox *pFlags = new QGroupBox(tr("Flags:"), pTab);
		QVBoxLayout *pFlagsLayout = new QVBoxLayout(pFlags);
		m_pFlags = new QListWidget(pFlags);
		connect(m_pFlags, &QListWidget::itemChanged, this, &MainWindow::onFlagItemChanged);
		pFlagsLayout->addWidget(m_pFlags);

		pLayout->addWidget(pImage);
		pLayout->addWidget(pFlags, 1);

		return pTab;
	}

	QWidget *MainWindow::createInfoTab()
	{
		QWidget *pTab = new QWidget(this);
		QVBoxLayout *pLayout = new QVBoxLayout(pTab);

		QGroupBox *pFileInfo = new QGroupBox(tr("File Info:"), pTab);
		QFormLayout *pFileForm = new QFormLayout(pFileInfo);
		m_pFileVersion = new QComboBox(pFileInfo);
		for(vlUInt uiMinor = 0; uiMinor <= VTF_MINOR_VERSION; uiMinor++)
		{
			m_pFileVersion->addItem(QStringLiteral("%1.%2").arg(VTF_MAJOR_VERSION).arg(uiMinor), uiMinor);
		}
		m_pFileVersion->setToolTip(tr("Change the version of the VTF file format this texture is "
			"written as.  The encoded image data is carried over unchanged."));
		connect(m_pFileVersion, &QComboBox::currentIndexChanged, this, &MainWindow::onFileVersionChanged);
		pFileForm->addRow(tr("Version:"), m_pFileVersion);

		m_pFileSize = addInfoRow(pFileForm, tr("Size:"));
		m_pFileCompression = addInfoRow(pFileForm, tr("Compression:"));

		QGroupBox *pImageInfo = new QGroupBox(tr("Image Info:"), pTab);
		QFormLayout *pImageForm = new QFormLayout(pImageInfo);
		m_pImageWidth = addInfoRow(pImageForm, tr("Width:"));
		m_pImageHeight = addInfoRow(pImageForm, tr("Height:"));
		m_pImageFormat = addInfoRow(pImageForm, tr("Format:"));
		m_pImageFrames = addInfoRow(pImageForm, tr("Frames:"));

		m_pImageStartFrame = new QSpinBox(pImageInfo);
		m_pImageStartFrame->setRange(0, 0);
		connect(m_pImageStartFrame, &QSpinBox::valueChanged, this, &MainWindow::onVtfPropertyChanged);
		pImageForm->addRow(tr("Start:"), m_pImageStartFrame);

		m_pImageFaces = addInfoRow(pImageForm, tr("Faces:"));
		m_pImageSlices = addInfoRow(pImageForm, tr("Slices:"));
		m_pImageMipmaps = addInfoRow(pImageForm, tr("Mipmaps:"));

		m_pImageBumpmapScale = new QDoubleSpinBox(pImageInfo);
		m_pImageBumpmapScale->setDecimals(2);
		m_pImageBumpmapScale->setSingleStep(0.01);
		m_pImageBumpmapScale->setRange(-100.0, 100.0);
		connect(m_pImageBumpmapScale, &QDoubleSpinBox::valueChanged, this, &MainWindow::onVtfPropertyChanged);
		pImageForm->addRow(tr("Bumpmap:"), m_pImageBumpmapScale);

		m_pImageReflectivity = addInfoRow(pImageForm, tr("Reflectivity:"));

		QGroupBox *pThumbnailInfo = new QGroupBox(tr("Thumbnail Info:"), pTab);
		QFormLayout *pThumbnailForm = new QFormLayout(pThumbnailInfo);
		m_pThumbnailWidth = addInfoRow(pThumbnailForm, tr("Width:"));
		m_pThumbnailHeight = addInfoRow(pThumbnailForm, tr("Height:"));
		m_pThumbnailFormat = addInfoRow(pThumbnailForm, tr("Format:"));

		pLayout->addWidget(pImageInfo);
		pLayout->addWidget(pThumbnailInfo);
		pLayout->addWidget(pFileInfo);
		pLayout->addStretch();

		return pTab;
	}

	QWidget *MainWindow::createResourcesTab()
	{
		QWidget *pTab = new QWidget(this);
		QVBoxLayout *pLayout = new QVBoxLayout(pTab);

		QGroupBox *pResourceInfo = new QGroupBox(tr("Resource Info:"), pTab);
		QFormLayout *pInfoForm = new QFormLayout(pResourceInfo);
		m_pResourceCount = addInfoRow(pInfoForm, tr("Count:"));

		QGroupBox *pResources = new QGroupBox(tr("Resources:"), pTab);
		QVBoxLayout *pResourcesLayout = new QVBoxLayout(pResources);
		m_pResources = new QTreeWidget(pResources);
		m_pResources->setHeaderHidden(true);
		m_pResources->setColumnCount(1);
		pResourcesLayout->addWidget(m_pResources);

		pLayout->addWidget(pResourceInfo);
		pLayout->addWidget(pResources, 1);

		return pTab;
	}

	//
	// Sidebar visibility.
	//

	void MainWindow::updateSidebarsVisible()
	{
		// hidden sidebar sits at zero width in the splitter
		// so remember the  widths before hiding and restore them when the sidebar comes back
		rememberSidebarSizes();

		const bool bLeft = m_pLeftTabs->count() != 0;
		const bool bRight = m_pRightTabs->count() != 0;
		const bool bChanged = bLeft != m_pLeftTabs->isVisibleTo(m_pSplitter)
			|| bRight != m_pRightTabs->isVisibleTo(m_pSplitter);

		m_pLeftTabs->setVisible(bLeft);
		m_pRightTabs->setVisible(bRight);

		if(bChanged)
		{
			applySidebarSizes();
		}
	}

	void MainWindow::rememberSidebarSizes()
	{
		const QList<int> Sizes = m_pSplitter->sizes();

		if(m_pLeftTabs->isVisibleTo(m_pSplitter) && Sizes.value(0, 0) > 0)
		{
			m_iSidebarSplit = Sizes.value(0);
		}
		if(m_pRightTabs->isVisibleTo(m_pSplitter) && Sizes.value(2, 0) > 0)
		{
			m_iSidebarRightSplit = Sizes.value(2);
		}
	}

	void MainWindow::applySidebarSizes()
	{
		const int iLeft = m_pLeftTabs->isVisibleTo(m_pSplitter) ? m_iSidebarSplit : 0;
		const int iRight = m_pRightTabs->isVisibleTo(m_pSplitter) ? m_iSidebarRightSplit : 0;

		m_pSplitter->setSizes({ iLeft,
			qMax(1, m_pSplitter->width() - iLeft - iRight), iRight });
	}

	//
	// VTF rendering.
	//

	void MainWindow::updateVtfFile()
	{
		if(m_pVTFFile == nullptr)
		{
			return;
		}

		// prevent re-entry
		if(m_bUpdatingVtfFile)
		{
			return;
		}
		m_bUpdatingVtfFile = true;

		const vlUInt uiFrame = static_cast<vlUInt>(m_pFrame->value());
		const vlUInt uiFace = static_cast<vlUInt>(m_pFace->value());
		vlUInt uiSlice = static_cast<vlUInt>(m_pSlice->value());
		const vlUInt uiMipmap = static_cast<vlUInt>(m_pMipmap->value());
		const vlSingle sHDRExposure = static_cast<vlSingle>(m_pHdrExposure->value()) / 100.0f;

		vlUInt uiWidth = 0, uiHeight = 0, uiDepth = 0;
		m_pVTFFile->ComputeMipmapDimensions(m_pVTFFile->GetWidth(), m_pVTFFile->GetHeight(),
			m_pVTFFile->GetDepth(), uiMipmap, uiWidth, uiHeight, uiDepth);

		if(uiSlice >= uiDepth)
		{
			uiSlice = uiDepth - 1;
		}

		m_pSlice->setValue(static_cast<int>(uiSlice));
		m_pSlice->setMaximum(static_cast<int>(uiDepth));

		float fMipmapScale = 1.0f;
		if(m_pMipmapFullSizeAction->isChecked())
		{
			fMipmapScale = static_cast<float>(1 << uiMipmap);
		}

		float fScale = m_fImageScale * fMipmapScale;

		// back off the zoom until the image fits the view
		const double dMaximumDimension = static_cast<double>(ImageView::maximumDisplayDimension());

		while(static_cast<double>(uiWidth) * fScale > dMaximumDimension
			|| static_cast<double>(uiHeight) * fScale > dMaximumDimension)
		{
			m_fImageScale *= 0.5f;
			fScale = m_fImageScale * fMipmapScale;
		}

		m_fEffectiveImageScale = fScale;

		// try avoid super duper expensive decoding
		const bool bMipmapChanged = m_pDecodedVTFFile != m_pVTFFile
			|| m_uiDecodedFrame != uiFrame
			|| m_uiDecodedFace != uiFace
			|| m_uiDecodedSlice != uiSlice
			|| m_uiDecodedMipmap != uiMipmap
			|| m_uiDecodedWidth != uiWidth
			|| m_uiDecodedHeight != uiHeight;

		const VTFImageFormat Format = m_pVTFFile->GetFormat();
		const bool bHdr = Format == IMAGE_FORMAT_RGBA16161616F
					   || Format == IMAGE_FORMAT_BC6H
					   || Format == IMAGE_FORMAT_BC6H_UNSIGNED
					   || VTFLib::CVTFFile::IsFloatFormat(Format) != vlFalse;

		if(bMipmapChanged)
		{
			if(bHdr)
			{
				m_DecodedFloatBuffer.resize(static_cast<size_t>(uiWidth) * uiHeight * 4);

				m_pVTFFile->Convert(m_pVTFFile->GetData(uiFrame, uiFace, uiSlice, uiMipmap),
					reinterpret_cast<vlByte *>(m_DecodedFloatBuffer.data()), uiWidth, uiHeight,
					m_pVTFFile->GetDecodeFormat(), IMAGE_FORMAT_RGBA32323232F);
			}
			else
			{
				m_DecodedFloatBuffer.clear();
				m_DecodedFloatBuffer.shrink_to_fit();
			}

			m_pDecodedVTFFile = m_pVTFFile;
			m_uiDecodedFrame = uiFrame;
			m_uiDecodedFace = uiFace;
			m_uiDecodedSlice = uiSlice;
			m_uiDecodedMipmap = uiMipmap;
			m_uiDecodedWidth = uiWidth;
			m_uiDecodedHeight = uiHeight;
		}

		if(bMipmapChanged || (bHdr && m_sDecodedExposure != sHDRExposure))
		{
			m_DecodedBuffer.resize(m_pVTFFile->ComputeImageSize(uiWidth, uiHeight, 1, IMAGE_FORMAT_RGBA8888));

			vlSetFloat(VTFLIB_FP16_HDR_EXPOSURE, sHDRExposure);

			// Decode image data.
			if(bHdr)
			{
				m_pVTFFile->Convert(reinterpret_cast<vlByte *>(m_DecodedFloatBuffer.data()),
					m_DecodedBuffer.data(), uiWidth, uiHeight,
					IMAGE_FORMAT_RGBA32323232F, IMAGE_FORMAT_RGBA8888);
			}
			else
			{
				m_pVTFFile->ConvertToRGBA8888(m_pVTFFile->GetData(uiFrame, uiFace, uiSlice, uiMipmap),
					m_DecodedBuffer.data(), uiWidth, uiHeight, m_pVTFFile->GetDecodeFormat());
			}

			m_sDecodedExposure = sHDRExposure;

			m_bCompositeValid = false;
		}

		// Pick which source channel feeds each output channel.
		int iChannel = -1;
		if(m_pChannelRAction->isChecked())
		{
			iChannel = 0;
		}
		else if(m_pChannelGAction->isChecked())
		{
			iChannel = 1;
		}
		else if(m_pChannelBAction->isChecked())
		{
			iChannel = 2;
		}
		else if(m_pChannelAAction->isChecked())
		{
			iChannel = 3;
		}

		const bool bMask = m_pMaskAction->isChecked();

		if(!m_bCompositeValid || m_iCompositeChannel != iChannel || m_bCompositeMask != bMask)
		{
			m_CompositeImage = QImage(static_cast<int>(uiWidth), static_cast<int>(uiHeight),
				QImage::Format_ARGB32_Premultiplied);

			const vlByte *pBuffer = m_DecodedBuffer.data();

			for(vlUInt j = 0; j < uiHeight; j++)
			{
				QRgb *pScanline = reinterpret_cast<QRgb *>(m_CompositeImage.scanLine(static_cast<int>(j)));
				const vlByte *pSource = pBuffer + static_cast<size_t>(j) * uiWidth * 4;

				for(vlUInt i = 0; i < uiWidth; i++, pSource += 4)
				{
					const vlUInt uiRed = iChannel < 0 ? pSource[0] : pSource[iChannel];
					const vlUInt uiGreen = iChannel < 0 ? pSource[1] : pSource[iChannel];
					const vlUInt uiBlue = iChannel < 0 ? pSource[2] : pSource[iChannel];
					const vlUInt uiAlpha = bMask ? pSource[3] : 255u;

					pScanline[i] = qRgba(static_cast<int>(uiRed * uiAlpha / 255u),
						static_cast<int>(uiGreen * uiAlpha / 255u),
						static_cast<int>(uiBlue * uiAlpha / 255u),
						static_cast<int>(uiAlpha)); // premultiplied alpha ..
				}
			}

			m_iCompositeChannel = iChannel;
			m_bCompositeMask = bMask;
			m_bCompositeValid = true;
		}

		m_pImageView->setTiled(m_pTileAction->isChecked());
		m_pImageView->setCheckerboard(bMask);
		m_pImageView->setScale(fScale);
		m_pImageView->setImage(m_CompositeImage);

		m_pStatusInfo1->setText(QStringLiteral("%1%").arg(m_fImageScale * 100.0f));

		m_bUpdatingVtfFile = false;
	}

	void MainWindow::invalidateImageCache()
	{
		m_pDecodedVTFFile = nullptr;
		m_bCompositeValid = false;
	}

	void MainWindow::showVtfFile(VTFLib::CVTFFile *pVTFFile)
	{
		m_pVTFFile = pVTFFile;

		// the texture data may have changed under us
		invalidateImageCache();

		const bool bWasSwitching = m_bSwitchingDocument;
		m_bSwitchingDocument = true;

		m_pFrame->setValue(0);
		m_pFace->setValue(0);
		m_pSlice->setValue(0);
		m_pMipmap->setValue(0);

		m_pFrame->setMaximum(static_cast<int>(pVTFFile->GetFrameCount()) - 1);
		m_pFace->setMaximum(static_cast<int>(pVTFFile->GetFaceCount()) - 1);
		m_pSlice->setMaximum(static_cast<int>(pVTFFile->GetDepth()) - 1);
		m_pMipmap->setMaximum(static_cast<int>(pVTFFile->GetMipmapCount()) - 1);

		if(pVTFFile->GetStartFrame() < pVTFFile->GetFrameCount())
		{
			m_pFrame->setValue(static_cast<int>(pVTFFile->GetStartFrame()));
		}

		m_pAnimateTimer->stop();
		m_pAnimateButton->setText(tr("&Play"));
		m_pAnimateButton->setEnabled(pVTFFile->GetFrameCount() > 1);
		m_pAnimateFps->setEnabled(pVTFFile->GetFrameCount() > 1);

		m_pHdrExposure->setEnabled(pVTFFile->GetFormat() == IMAGE_FORMAT_RGBA16161616F
			|| pVTFFile->GetFormat() == IMAGE_FORMAT_BC6H
			|| pVTFFile->GetFormat() == IMAGE_FORMAT_BC6H_UNSIGNED
			|| VTFLib::CVTFFile::IsFloatFormat(pVTFFile->GetFormat()) != vlFalse);

		const vlUInt uiFlags = pVTFFile->GetFlags();

		m_bUpdatingFlags = true;
		m_pFlags->clear();
		for(int i = 0; i < FlagCount; i++)
		{
			QListWidgetItem *pItem = new QListWidgetItem(QString::fromLatin1(FlagNames[i]), m_pFlags);
			pItem->setFlags(pItem->flags() | Qt::ItemIsUserCheckable);
			pItem->setCheckState((uiFlags & (1u << i)) != 0 ? Qt::Checked : Qt::Unchecked);
		}
		m_bUpdatingFlags = false;

		updateFileInfo();

		m_pImageWidth->setText(QString::number(pVTFFile->GetWidth()));
		m_pImageHeight->setText(QString::number(pVTFFile->GetHeight()));
		m_pImageFormat->setText(imageFormatString(pVTFFile->GetFormat()));
		m_pImageFrames->setText(QString::number(pVTFFile->GetFrameCount()));

		m_pImageStartFrame->setMaximum(static_cast<int>(pVTFFile->GetFrameCount()) - 1);
		m_pImageStartFrame->setValue(pVTFFile->GetStartFrame() == 0xffff
			? 0 : static_cast<int>(pVTFFile->GetStartFrame()));

		m_pImageFaces->setText(QString::number(pVTFFile->GetFaceCount()));
		m_pImageSlices->setText(QString::number(pVTFFile->GetDepth()));
		m_pImageMipmaps->setText(QString::number(pVTFFile->GetMipmapCount()));
		m_pImageBumpmapScale->setValue(pVTFFile->GetBumpmapScale());

		vlSingle sX = 0.0f, sY = 0.0f, sZ = 0.0f;
		pVTFFile->GetReflectivity(sX, sY, sZ);
		m_pImageReflectivity->setText(QStringLiteral("%1, %2, %3")
			.arg(sX, 0, 'f', 3).arg(sY, 0, 'f', 3).arg(sZ, 0, 'f', 3));

		m_pThumbnailWidth->setText(QString::number(pVTFFile->GetThumbnailWidth()));
		m_pThumbnailHeight->setText(QString::number(pVTFFile->GetThumbnailHeight()));
		m_pThumbnailFormat->setText(imageFormatString(pVTFFile->GetThumbnailFormat()));

		updateResourceList();

		m_bSwitchingDocument = bWasSwitching;

		m_fImageScale = 1.0f;
		updateVtfFile();

		// Reveal the tabs the texture populated.
		if(m_pLeftTabs->indexOf(m_pImageTab) == -1)
		{
			m_pLeftTabs->addTab(m_pImageTab, tr("Image"));
		}
		if(m_pRightTabs->indexOf(m_pInfoTab) == -1)
		{
			m_pRightTabs->addTab(m_pInfoTab, tr("Info"));
		}
		if(m_pRightTabs->indexOf(m_pResourcesTab) == -1)
		{
			m_pRightTabs->addTab(m_pResourcesTab, tr("Resources"));
		}
		updateSidebarsVisible();

		m_pLeftTabs->setCurrentWidget(m_pImageTab);
		m_pRightTabs->setCurrentWidget(m_pInfoTab);

		m_pCentralStack->setCurrentWidget(m_pImageScrollArea);
	}

	void MainWindow::setResourceInformation(QTreeWidgetItem *pItem, VTFLib::Nodes::CVMTGroupNode *pVMTNode)
	{
		for(vlUInt i = 0; i < pVMTNode->GetNodeCount(); i++)
		{
			VTFLib::Nodes::CVMTNode *pVMTChild = pVMTNode->GetNode(i);
			const QString sName = QString::fromLatin1(pVMTChild->GetName());

			switch(pVMTChild->GetType())
			{
			case NODE_TYPE_GROUP:
			{
				QTreeWidgetItem *pChild = new QTreeWidgetItem(pItem, QStringList(sName));
				setResourceInformation(pChild, static_cast<VTFLib::Nodes::CVMTGroupNode *>(pVMTChild));
				break;
			}
			case NODE_TYPE_STRING:
			{
				QTreeWidgetItem *pChild = new QTreeWidgetItem(pItem, QStringList(sName));
				new QTreeWidgetItem(pChild, QStringList(QString::fromLatin1(
					static_cast<VTFLib::Nodes::CVMTStringNode *>(pVMTChild)->GetValue())));
				break;
			}
			case NODE_TYPE_INTEGER:
			{
				QTreeWidgetItem *pChild = new QTreeWidgetItem(pItem, QStringList(sName));
				new QTreeWidgetItem(pChild, QStringList(QString::number(
					static_cast<VTFLib::Nodes::CVMTIntegerNode *>(pVMTChild)->GetValue())));
				break;
			}
			case NODE_TYPE_SINGLE:
			{
				QTreeWidgetItem *pChild = new QTreeWidgetItem(pItem, QStringList(sName));
				new QTreeWidgetItem(pChild, QStringList(QString::number(
					static_cast<VTFLib::Nodes::CVMTSingleNode *>(pVMTChild)->GetValue())));
				break;
			}
			default:
				break;
			}
		}
	}

	void MainWindow::updateFileInfo()
	{
		if(m_pVTFFile == nullptr)
		{
			return;
		}

		m_bUpdatingFileInfo = true;
		m_pFileVersion->setCurrentIndex(m_pFileVersion->findData(m_pVTFFile->GetMinorVersion()));
		m_bUpdatingFileInfo = false;

		m_pFileSize->setText(tr("%1 KB").arg(
			QLocale().toString(static_cast<double>(m_pVTFFile->GetSize()) / 1024.0, 'f', 3)));

		const vlShort sAuxCompressionLevel = m_pVTFFile->GetAuxCompressionLevel();
		if(sAuxCompressionLevel == VTF_AUX_COMPRESSION_LEVEL_NONE)
		{
			m_pFileCompression->setText(tr("None"));
		}
		else
		{
			const QString sMethod = m_pVTFFile->GetAuxCompressionMethod() == AUX_COMPRESSION_METHOD_ZSTD
				? tr("Zstandard") : tr("Deflate");
			m_pFileCompression->setText(sAuxCompressionLevel == VTF_AUX_COMPRESSION_LEVEL_DEFAULT
				? sMethod
				: QStringLiteral("%1 (%2)").arg(sMethod).arg(sAuxCompressionLevel));
		}
	}

	void MainWindow::updateResourceList()
	{
		if(m_pVTFFile == nullptr)
		{
			return;
		}

		m_pResourceCount->setText(QString::number(m_pVTFFile->GetResourceCount()));

		m_pResources->clear();
		for(vlUInt i = 0; i < m_pVTFFile->GetResourceCount(); i++)
		{
			const vlUInt uiResource = m_pVTFFile->GetResourceType(i);

			QString sName;
			switch(uiResource)
			{
			case VTF_LEGACY_RSRC_LOW_RES_IMAGE:		sName = tr("Thumbnail Image"); break;
			case VTF_LEGACY_RSRC_IMAGE:				sName = tr("Image"); break;
			case VTF_RSRC_SHEET:					sName = tr("Sheet"); break;
			case VTF_RSRC_CRC:						sName = tr("Cyclic Redundancy Check"); break;
			case VTF_RSRC_TEXTURE_LOD_SETTINGS:		sName = tr("LOD Settings"); break;
			case VTF_RSRC_TEXTURE_SETTINGS_EX:		sName = tr("Extended Texture Settings"); break;
			case VTF_RSRC_KEY_VALUE_DATA:			sName = tr("Key/Value Data"); break;
			case VTF_RSRC_PARALLAX_CUBEMAP:			sName = tr("Parallax Cubemap"); break;
			default:								sName = tr("Unknown"); break;
			}

			QTreeWidgetItem *pItem = new QTreeWidgetItem(m_pResources, QStringList(sName));

			vlUInt uiSize = 0;
			vlVoid *lpData = m_pVTFFile->GetResourceData(uiResource, uiSize);

			switch(uiResource)
			{
			case VTF_RSRC_SHEET:
			{
				SheetFile Sheet;
				if(lpData && uiSize && Sheet.load(lpData, uiSize))
				{
					for(const SheetSequence &Sequence : Sheet.sequences())
					{
						float fDuration = 0.0f;
						for(const SheetFrame &Frame : Sequence.Frames)
						{
							fDuration += Frame.fDuration;
						}

						new QTreeWidgetItem(pItem, QStringList(
							tr("Sequence %1: %2 frames, %3 s, %4")
								.arg(Sequence.iNumber)
								.arg(Sequence.Frames.count())
								.arg(QString::number(fDuration, 'g', 4))
								.arg(Sequence.bClamp ? tr("clamp") : tr("loop"))));
					}
					break;
				}

				new QTreeWidgetItem(pItem, QStringList(tr("Size: %1 B").arg(QLocale().toString(uiSize))));
				break;
			}

			case VTF_RSRC_CRC:
				if(lpData != nullptr)
				{
					new QTreeWidgetItem(pItem, QStringList(tr("Checksum: 0x%1")
						.arg(hex32(*static_cast<vlUInt *>(lpData)))));
				}
				break;

			case VTF_RSRC_PARALLAX_CUBEMAP:
				if(lpData && uiSize == sizeof(SVTFParallaxCubemapResource))
				{
					const SVTFParallaxCubemapResource *pParallax =
						static_cast<SVTFParallaxCubemapResource *>(lpData);

					new QTreeWidgetItem(pItem, QStringList(tr("Origin: %1, %2, %3, %4")
						.arg(pParallax->Origin[0]).arg(pParallax->Origin[1])
						.arg(pParallax->Origin[2]).arg(pParallax->Origin[3])));

					QTreeWidgetItem *pMatrix = new QTreeWidgetItem(pItem, QStringList(tr("Inverse OBB Matrix")));
					for(vlUInt j = 0; j < 4; j++)
					{
						new QTreeWidgetItem(pMatrix, QStringList(tr("%1, %2, %3, %4")
							.arg(pParallax->InvObbMatrix[j * 4 + 0]).arg(pParallax->InvObbMatrix[j * 4 + 1])
							.arg(pParallax->InvObbMatrix[j * 4 + 2]).arg(pParallax->InvObbMatrix[j * 4 + 3])));
					}
					pMatrix->setExpanded(true);
					break;
				}

				new QTreeWidgetItem(pItem, QStringList(tr("Size: %1 B").arg(QLocale().toString(uiSize))));
				break;

			case VTF_RSRC_TEXTURE_LOD_SETTINGS:
				if(lpData && uiSize == sizeof(SVTFTextureLODControlResource))
				{
					const SVTFTextureLODControlResource *pLODControl =
						static_cast<SVTFTextureLODControlResource *>(lpData);
					new QTreeWidgetItem(pItem, QStringList(tr("Clamp U: %1").arg(pLODControl->ResolutionClampU)));
					new QTreeWidgetItem(pItem, QStringList(tr("Clamp V: %1").arg(pLODControl->ResolutionClampV)));
					break;
				}
				[[fallthrough]];

			case VTF_RSRC_KEY_VALUE_DATA:
				if(lpData && uiSize)
				{
					VTFLib::CVMTFile *pVMTFile = new VTFLib::CVMTFile();

					if(pVMTFile->Load(lpData, uiSize))
					{
						pItem->setText(0, QString::fromLatin1(pVMTFile->GetRoot()->GetName()));
						setResourceInformation(pItem, pVMTFile->GetRoot());
					}

					delete pVMTFile;
				}
				[[fallthrough]];

			default:
				if(lpData && uiSize == sizeof(vlUInt))
				{
					new QTreeWidgetItem(pItem, QStringList(tr("Data: 0x%1")
						.arg(hex32(*static_cast<vlUInt *>(lpData)))));
				}
				else
				{
					new QTreeWidgetItem(pItem, QStringList(tr("Size: %1 B").arg(QLocale().toString(uiSize))));
				}
				break;
			}

			pItem->setExpanded(true);
		}

		updateSheetActions();
		updateHotspotActions();
	}

	void MainWindow::updateSheetActions()
	{
		const bool bSupported = m_pVTFFile != nullptr && m_pVTFFile->GetSupportsResources();

		vlUInt uiSize = 0;
		const bool bHasSheet = bSupported && m_pVTFFile->GetResourceData(VTF_RSRC_SHEET, uiSize) != nullptr;

		m_pEditSheetAction->setEnabled(bSupported);
		m_pEditSheetAction->setText(bHasSheet ? tr("&Edit...") : tr("&Create..."));
		m_pRemoveSheetAction->setEnabled(bHasSheet);

		m_pEditSheetAction->setToolTip(bSupported
			? tr("Edit the sprite sheet (.sht) resource attached to this texture.")
			: tr("Sprite sheets require a version 7.3 or newer texture."));
	}

	void MainWindow::onEditSheet()
	{
		if(m_pVTFFile == nullptr || !m_pVTFFile->GetSupportsResources())
		{
			return;
		}

		SheetFile Sheet;

		vlUInt uiSize = 0;
		if(vlVoid *lpData = m_pVTFFile->GetResourceData(VTF_RSRC_SHEET, uiSize))
		{
			if(uiSize && !Sheet.load(lpData, uiSize))
			{
				if(QMessageBox::question(this, QApplication::applicationName(),
					tr("The sprite sheet resource could not be read. Replace it?"),
					QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
				{
					return;
				}
			}
		}

		SheetDialog Dialog(Sheet, m_pImageView->image(),
			static_cast<int>(m_pVTFFile->GetWidth()), static_cast<int>(m_pVTFFile->GetHeight()), this);

		if(Dialog.exec() != QDialog::Accepted)
		{
			return;
		}

		const SheetFile &NewSheet = Dialog.sheet();

		if(NewSheet.isEmpty())
		{
			m_pVTFFile->SetResourceData(VTF_RSRC_SHEET, 0, nullptr);
		}
		else
		{
			QByteArray Data = NewSheet.save();
			if(m_pVTFFile->SetResourceData(VTF_RSRC_SHEET,
				static_cast<vlUInt>(Data.size()), Data.data()) == nullptr)
			{
				QMessageBox::critical(this, QApplication::applicationName(),
					tr("Failed to write the sprite sheet resource:\n%1")
						.arg(QString::fromLatin1(vlGetLastError())));
				return;
			}
		}

		updateResourceList();
		onVtfPropertyChanged();
	}

	void MainWindow::onRemoveSheet()
	{
		if(m_pVTFFile == nullptr || !m_pVTFFile->GetSupportsResources())
		{
			return;
		}

		if(QMessageBox::question(this, QApplication::applicationName(),
			tr("Remove the sprite sheet resource from this texture?"),
			QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
		{
			return;
		}

		m_pVTFFile->SetResourceData(VTF_RSRC_SHEET, 0, nullptr);

		updateResourceList();
		onVtfPropertyChanged();
	}

	//
	// Hotspot rectangles
	//

	QString MainWindow::hotspotFilePath() const
	{
		if(m_pVTFFile == nullptr)
		{
			return QString();
		}

		return RectFile::pathForTexture(m_sFileName);
	}

	void MainWindow::updateHotspotActions()
	{
		const QString sPath = hotspotFilePath();
		const bool bHasFile = !sPath.isEmpty() && QFileInfo::exists(sPath);

		m_pEditHotspotAction->setEnabled(!sPath.isEmpty());
		m_pEditHotspotAction->setText(bHasFile ? tr("&Edit...") : tr("&Create..."));
		m_pRemoveHotspotAction->setEnabled(bHasFile);

		m_pEditHotspotAction->setToolTip(sPath.isEmpty()
			? tr("Save the texture before creating a hotspot .rect file for it.")
			: tr("Edit the hotspot rectangles in %1.").arg(QDir::toNativeSeparators(sPath)));
	}

	void MainWindow::onEditHotspot()
	{
		const QString sPath = hotspotFilePath();
		if(sPath.isEmpty())
		{
			return;
		}

		RectFile Rectangles;

		QFile File(sPath);
		if(File.exists())
		{
			if(!File.open(QIODevice::ReadOnly))
			{
				QMessageBox::critical(this, QApplication::applicationName(),
					tr("Error opening hotspot file:\n\n%1").arg(File.errorString()));
				return;
			}

			const QByteArray Data = File.readAll();
			File.close();

			QString sError;
			if(!Rectangles.load(Data, &sError))
			{
				if(QMessageBox::question(this, QApplication::applicationName(),
					tr("The hotspot file could not be read:\n\n%1\n\nReplace it?").arg(sError),
					QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
				{
					return;
				}

				Rectangles = RectFile();
			}
		}

		HotspotDialog Dialog(Rectangles, m_pImageView->image(),
			static_cast<int>(m_pVTFFile->GetWidth()), static_cast<int>(m_pVTFFile->GetHeight()), this);

		if(Dialog.exec() != QDialog::Accepted)
		{
			return;
		}

		const RectFile &NewRectangles = Dialog.rectangles();

		if(NewRectangles.isEmpty())
		{
			if(File.exists() && !File.remove())
			{
				QMessageBox::critical(this, QApplication::applicationName(),
					tr("Error removing hotspot file:\n\n%1").arg(File.errorString()));
			}
		}
		else
		{
			if(!File.open(QIODevice::WriteOnly | QIODevice::Truncate))
			{
				QMessageBox::critical(this, QApplication::applicationName(),
					tr("Error saving hotspot file:\n\n%1").arg(File.errorString()));
				return;
			}

			File.write(NewRectangles.save());
			File.close();
		}

		updateHotspotActions();
	}

	void MainWindow::onRemoveHotspot()
	{
		const QString sPath = hotspotFilePath();
		if(sPath.isEmpty() || !QFileInfo::exists(sPath))
		{
			return;
		}

		if(QMessageBox::question(this, QApplication::applicationName(),
			tr("Delete the hotspot file %1?").arg(QDir::toNativeSeparators(sPath)),
			QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
		{
			return;
		}

		QFile File(sPath);
		if(!File.remove())
		{
			QMessageBox::critical(this, QApplication::applicationName(),
				tr("Error removing hotspot file:\n\n%1").arg(File.errorString()));
		}

		updateHotspotActions();
	}

	bool MainWindow::getVtfFile()
	{
		if(m_pVTFFile == nullptr)
		{
			return false;
		}

		m_pVTFFile->SetFlags(0);
		for(int i = 0; i < m_pFlags->count(); i++)
		{
			m_pVTFFile->SetFlag(static_cast<VTFImageFlag>(1u << i),
				m_pFlags->item(i)->checkState() == Qt::Checked);
		}

		// below v7.5 an environment map's start frame doubles as a "no sphere map face" flag
		// clearing it would change the face count out from under the image data
		if(m_pVTFFile->GetStartFrame() != 0xffff)
		{
			m_pVTFFile->SetStartFrame(static_cast<vlUInt>(m_pImageStartFrame->value()));
		}
		m_pVTFFile->SetBumpmapScale(static_cast<vlSingle>(m_pImageBumpmapScale->value()));

		return true;
	}

	//
	// VMT.
	//

	void MainWindow::showVmtFile(Document *pDocument)
	{
		m_pVMTFile = pDocument->pVMTFile;

		m_pVmtEdit->setDocument(pDocument->pTextDocument);

		applyVmtTabStopDistance();

		validateVmtFile();
		updateVmtErrorHighlight();

		m_pCentralStack->setCurrentWidget(m_pVmtEdit);
	}

	bool MainWindow::validateVmtFile()
	{
		if(m_pVMTFile == nullptr)
		{
			return true;
		}

		const QByteArray Text = m_pVmtEdit->toPlainText().toLocal8Bit();
		const vlBool bResult = m_pVMTFile->Load(Text.constData(), static_cast<vlUInt>(Text.length()));

		if(bResult)
		{
			m_iVmtErrorLine = 0;
			m_pStatusInfo1->clear();
		}
		else
		{
			m_iVmtErrorLine = static_cast<int>(m_pVMTFile->GetParseErrorLine());
			m_pStatusInfo1->setText(lastErrorString());
		}

		return bResult != vlFalse;
	}

	bool MainWindow::confirmVmtFile(int iIndex)
	{
		Document *pDocument = m_Documents.at(static_cast<size_t>(iIndex));

		const QByteArray Text = pDocument->pTextDocument->toPlainText().toLocal8Bit();
		if(pDocument->pVMTFile->Load(Text.constData(), static_cast<vlUInt>(Text.length())))
		{
			return true;
		}

		return QMessageBox::warning(this, QApplication::applicationName(),
			tr("\"%1\" has a syntax error:\n\n%2\n\nSave it anyway?")
				.arg(documentTitle(pDocument), lastErrorString()),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
	}

	void MainWindow::updateVmtErrorHighlight()
	{
		QList<QTextEdit::ExtraSelection> Selections;

		if(m_iVmtErrorLine > 0 && m_iVmtErrorLine <= m_pVmtEdit->document()->blockCount())
		{
			QTextEdit::ExtraSelection Selection;
			Selection.format.setBackground(VmtColors::Get(m_VmtEditorSettings.isDark()).ErrorLine);
			Selection.format.setProperty(QTextFormat::FullWidthSelection, true);
			Selection.cursor = QTextCursor(m_pVmtEdit->document()->findBlockByNumber(m_iVmtErrorLine - 1));
			Selection.cursor.clearSelection();
			Selections.append(Selection);
		}

		m_pVmtEdit->setExtraSelections(Selections);
	}

	void MainWindow::applyVmtTabStopDistance()
	{
		m_pVmtEdit->setTabStopDistance(m_VmtEditorSettings.iTabSize
			* m_pVmtEdit->fontMetrics().horizontalAdvance(QLatin1Char(' ')));
	}

	void MainWindow::applyVmtEditorSettings()
	{
		m_pVmtEdit->setFont(m_VmtEditorSettings.font());
		applyVmtTabStopDistance();

		const bool bDark = m_VmtEditorSettings.isDark();
		const VmtColors::Scheme &Colors = VmtColors::Get(bDark);

		QPalette Palette = m_pVmtEdit->palette();
		Palette.setColor(QPalette::Base, Colors.Background);
		Palette.setColor(QPalette::Text, Colors.Text);
		m_pVmtEdit->setPalette(Palette);
		m_pVmtEdit->setLineNumberColors(Colors.LineNumberBackground, Colors.LineNumber, Colors.LineNumberCurrent);

		for(Document *pDocument : m_Documents)
		{
			if(pDocument->pTextDocument != nullptr)
			{
				pDocument->pTextDocument->setDefaultFont(m_VmtEditorSettings.font());
			}
			if(pDocument->pHighlighter != nullptr)
			{
				pDocument->pHighlighter->setDark(bDark);
			}
		}

		updateVmtErrorHighlight();
	}

	//
	// Documents and tabs.
	//

	Document *MainWindow::currentDocument() const
	{
		if(m_iCurrentDocument < 0 || m_iCurrentDocument >= static_cast<int>(m_Documents.size()))
		{
			return nullptr;
		}

		return m_Documents.at(static_cast<size_t>(m_iCurrentDocument));
	}

	QString MainWindow::documentTitle(const Document *pDocument) const
	{
		return pDocument->sFileName.isEmpty()
			? pDocument->sUntitledName
			: QFileInfo(pDocument->sFileName).fileName();
	}

	int MainWindow::indexOfFile(const QString &sFileName) const
	{
		if(sFileName.isEmpty())
		{
			return -1;
		}

		const QString sPath = QFileInfo(sFileName).absoluteFilePath();

		for(size_t i = 0; i < m_Documents.size(); i++)
		{
			const QString &sOther = m_Documents.at(i)->sFileName;

			if(!sOther.isEmpty()
				&& QFileInfo(sOther).absoluteFilePath().compare(sPath, Qt::CaseInsensitive) == 0)
			{
				return static_cast<int>(i);
			}
		}

		return -1;
	}

	void MainWindow::updateTabText(int iIndex)
	{
		Document *pDocument = m_Documents.at(static_cast<size_t>(iIndex));

		// An unsaved document gets an asterisk beside its name.
		QString sTitle = documentTitle(pDocument);
		sTitle.replace(QLatin1Char('&'), QLatin1String("&&"));
		if(pDocument->bModified)
		{
			sTitle += QLatin1Char('*');
		}

		m_pTabBar->setTabText(iIndex, sTitle);
		m_pTabBar->setTabToolTip(iIndex, pDocument->sFileName.isEmpty()
			? documentTitle(pDocument) : QDir::toNativeSeparators(pDocument->sFileName));

		if(iIndex == m_iCurrentDocument)
		{
			updateWindowTitle();
		}
	}

	void MainWindow::setDocumentModified(Document *pDocument, bool bModified)
	{
		if(pDocument->bModified == bModified)
		{
			return;
		}

		pDocument->bModified = bModified;

		for(size_t i = 0; i < m_Documents.size(); i++)
		{
			if(m_Documents.at(i) == pDocument)
			{
				updateTabText(static_cast<int>(i));
				break;
			}
		}
	}

	void MainWindow::setupTextDocument(Document *pDocument, const QString &sText)
	{
		QTextDocument *pTextDocument = new QTextDocument(this);
		pTextDocument->setDocumentLayout(new QPlainTextDocumentLayout(pTextDocument));
		pTextDocument->setDefaultFont(m_VmtEditorSettings.font());
		pTextDocument->setPlainText(sText);
		pTextDocument->setModified(false);

		pDocument->pTextDocument = pTextDocument;
		pDocument->pHighlighter = new VmtHighlighter(pTextDocument, m_VmtEditorSettings.isDark());

		connect(pTextDocument, &QTextDocument::modificationChanged, this, [this, pDocument](bool bModified)
		{
			setDocumentModified(pDocument, bModified);
		});
	}

	int MainWindow::addDocument(Document *pDocument)
	{
		if(pDocument->sFileName.isEmpty() && pDocument->sUntitledName.isEmpty())
		{
			pDocument->sUntitledName = tr("Untitled %1").arg(++m_iUntitledCounter);
		}

		m_Documents.push_back(pDocument);

		const int iIndex = static_cast<int>(m_Documents.size()) - 1;

		// Adding the first tab makes it current, which activates the document.
		m_pTabBar->addTab(QString());
		updateTabText(iIndex);
		m_pTabBar->show();

		if(m_pTabBar->currentIndex() != iIndex)
		{
			m_pTabBar->setCurrentIndex(iIndex);
		}
		else if(m_iCurrentDocument != iIndex)
		{
			activateDocument(iIndex);
		}

		updateActions();

		return iIndex;
	}

	void MainWindow::commitCurrentDocument()
	{
		Document *pDocument = currentDocument();
		if(pDocument == nullptr)
		{
			return;
		}

		pDocument->fImageScale = m_fImageScale;
		pDocument->iVmtErrorLine = m_iVmtErrorLine;

		if(pDocument->pVTFFile != nullptr)
		{
			// push the flags and header fields the user edited back into the texture
			getVtfFile();

			pDocument->iFrame = m_pFrame->value();
			pDocument->iFace = m_pFace->value();
			pDocument->iSlice = m_pSlice->value();
			pDocument->iMipmap = m_pMipmap->value();
			pDocument->iScrollX = m_pImageScrollArea->horizontalScrollBar()->value();
			pDocument->iScrollY = m_pImageScrollArea->verticalScrollBar()->value();
		}
	}

	void MainWindow::activateDocument(int iIndex)
	{
		m_iCurrentDocument = iIndex;

		Document *pDocument = m_Documents.at(static_cast<size_t>(iIndex));

		m_bSwitchingDocument = true;

		m_pVTFFile = nullptr;
		m_pVMTFile = nullptr;
		m_sFileName = pDocument->sFileName;
		m_iVmtErrorLine = pDocument->iVmtErrorLine;
		m_fImageScale = pDocument->fImageScale;

		m_pStatusInfo1->clear();
		m_pStatusInfo2->clear();

		if(pDocument->pVTFFile != nullptr)
		{
			m_pVmtEdit->setDocument(m_pEmptyDocument);

			showVtfFile(pDocument->pVTFFile);

			m_pFrame->setValue(pDocument->iFrame);
			m_pFace->setValue(pDocument->iFace);
			m_pMipmap->setValue(pDocument->iMipmap);
			m_pSlice->setValue(pDocument->iSlice);

			m_fImageScale = pDocument->fImageScale;
			updateVtfFile();

			m_pImageScrollArea->horizontalScrollBar()->setValue(pDocument->iScrollX);
			m_pImageScrollArea->verticalScrollBar()->setValue(pDocument->iScrollY);
		}
		else
		{
			m_pAnimateTimer->stop();
			m_pAnimateButton->setText(tr("&Play"));
			m_pAnimateButton->setEnabled(false);
			m_pAnimateFps->setEnabled(false);
			m_pHdrExposure->setEnabled(false);

			m_pImageView->setImage(QImage());
			invalidateImageCache();

			hideVtfSidebars();

			showVmtFile(pDocument);
		}

		m_bSwitchingDocument = false;

		m_pStatusFileName->setText(pDocument->sFileName);
		updateWindowTitle();
		updateActions();

		if(pDocument->pVMTFile != nullptr)
		{
			onVmtCursorChanged();
			m_pVmtEdit->setFocus();
		}
	}

	bool MainWindow::maybeSaveDocument(int iIndex)
	{
		Document *pDocument = m_Documents.at(static_cast<size_t>(iIndex));

		if(!pDocument->bModified)
		{
			return true;
		}

		if(iIndex != m_iCurrentDocument)
		{
			m_pTabBar->setCurrentIndex(iIndex);
		}

		const QMessageBox::StandardButton Button = QMessageBox::warning(this,
			QApplication::applicationName(),
			tr("\"%1\" has unsaved changes.\n\nDo you want to save them?").arg(documentTitle(pDocument)),
			QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

		if(Button == QMessageBox::Save)
		{
			return saveDocument(iIndex);
		}

		return Button == QMessageBox::Discard;
	}

	bool MainWindow::closeDocument(int iIndex)
	{
		if(iIndex < 0 || iIndex >= static_cast<int>(m_Documents.size()))
		{
			return true;
		}

		if(!maybeSaveDocument(iIndex))
		{
			return false;
		}

		Document *pDocument = m_Documents.at(static_cast<size_t>(iIndex));

		if(iIndex == m_iCurrentDocument)
		{
			// stop mirroring the document before it goes away
			m_pVTFFile = nullptr;
			m_pVMTFile = nullptr;
			m_iCurrentDocument = -1;
			m_pVmtEdit->setDocument(m_pEmptyDocument);
		}
		else if(iIndex < m_iCurrentDocument)
		{
			m_iCurrentDocument--;
		}

		m_Documents.erase(m_Documents.begin() + iIndex);

		{
			const QSignalBlocker Blocker(m_pTabBar);
			m_pTabBar->removeTab(iIndex);
		}

		delete pDocument->pVTFFile;
		delete pDocument->pVMTFile;
		delete pDocument->pHighlighter;
		delete pDocument->pTextDocument;
		delete pDocument;

		if(m_iCurrentDocument == -1)
		{
			clearWidgets();

			// Fall back to the tab that took its place, or the last one
			const int iNext = qMin(iIndex, static_cast<int>(m_Documents.size()) - 1);
			if(iNext >= 0)
			{
				const QSignalBlocker Blocker(m_pTabBar);
				m_pTabBar->setCurrentIndex(iNext);
				activateDocument(iNext);
			}
		}
		else
		{
			const QSignalBlocker Blocker(m_pTabBar);
			m_pTabBar->setCurrentIndex(m_iCurrentDocument);
		}

		m_pTabBar->setVisible(!m_Documents.empty());

		updateActions();

		return true;
	}

	void MainWindow::updateActions()
	{
		Document *pDocument = currentDocument();

		const bool bAny = pDocument != nullptr;
		const bool bVtf = bAny && pDocument->pVTFFile != nullptr;
		const bool bDocuments = !m_Documents.empty();

		m_pSaveAction->setEnabled(bAny);
		m_pSaveAsAction->setEnabled(bAny);
		m_pSaveAllAction->setEnabled(bDocuments);
		m_pCloseAction->setEnabled(bAny);
		m_pCloseAllAction->setEnabled(bDocuments);

		m_pExportAction->setEnabled(bVtf);
		m_pExportAllAction->setEnabled(bVtf);
		m_pCopyAction->setEnabled(bVtf);

		m_pNextTabAction->setEnabled(m_Documents.size() > 1);
		m_pPreviousTabAction->setEnabled(m_Documents.size() > 1);

		m_pTextureMenu->setEnabled(bVtf);
		updateSheetActions();
		updateHotspotActions();
	}

	//
	// File operations.
	//

	void MainWindow::newFile()
	{
		Document *pDocument = new Document();
		pDocument->pVMTFile = new VTFLib::CVMTFile();

		setupTextDocument(pDocument, QStringLiteral("\"LightmappedGeneric\"\n{\n}"));

		addDocument(pDocument);

		// Select the shader name so it can be typed over straight away.
		QTextCursor Cursor = m_pVmtEdit->textCursor();
		Cursor.setPosition(1);
		Cursor.setPosition(19, QTextCursor::KeepAnchor);
		m_pVmtEdit->setTextCursor(Cursor);

		m_pVmtEdit->setFocus();
	}

	void MainWindow::open(const QString &sFileName, bool bTemp)
	{
		// already open? Just show it.
		const int iExisting = indexOfFile(bTemp ? QString() : sFileName);
		if(iExisting != -1)
		{
			m_pTabBar->setCurrentIndex(iExisting);
			return;
		}

		const QByteArray Path = QDir::toNativeSeparators(sFileName).toLocal8Bit();

		if(sFileName.endsWith(QLatin1String(".vtf"), Qt::CaseInsensitive))
		{
			VTFLib::CVTFFile *pVTFFile = new VTFLib::CVTFFile();

			if(!pVTFFile->Load(Path.constData()))
			{
				delete pVTFFile;

				QMessageBox::critical(this, QApplication::applicationName(),
					tr("Error loading VTF texture:\n\n%1").arg(lastErrorString()));
				return;
			}

			Document *pDocument = new Document();
			pDocument->pVTFFile = pVTFFile;
			pDocument->sFileName = bTemp ? QString() : sFileName;

			addDocument(pDocument);

			if(!bTemp)
			{
				addRecentFile(sFileName);
				FileDialogHistory::remember(FileDialogHistory::s_sFileDirectory, sFileName);
			}
		}
		else if(IsKeyValuesFileName(sFileName))
		{
			QFile File(sFileName);
			if(!File.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				QMessageBox::critical(this, QApplication::applicationName(),
					tr("Error loading VMT material:\n\n%1").arg(File.errorString()));
				return;
			}

			const QString sText = QString::fromLocal8Bit(File.readAll());
			File.close();

			VTFLib::CVMTFile *pVMTFile = new VTFLib::CVMTFile();
			pVMTFile->Load(Path.constData());

			Document *pDocument = new Document();
			pDocument->pVMTFile = pVMTFile;
			pDocument->sFileName = bTemp ? QString() : sFileName;

			setupTextDocument(pDocument, sText);

			addDocument(pDocument);

			if(!bTemp)
			{
				addRecentFile(sFileName);
				FileDialogHistory::remember(FileDialogHistory::s_sFileDirectory, sFileName);
			}

			m_pVmtEdit->setFocus();
		}
	}

	bool MainWindow::save(int iIndex, const QString &sFileName)
	{
		if(sFileName.isEmpty())
		{
			return false;
		}

		Document *pDocument = m_Documents.at(static_cast<size_t>(iIndex));

		// make sure the widgets have handed their state back first
		if(iIndex == m_iCurrentDocument)
		{
			commitCurrentDocument();
		}

		if(pDocument->pVTFFile != nullptr)
		{
			const QByteArray Path = QDir::toNativeSeparators(sFileName).toLocal8Bit();

			if(!pDocument->pVTFFile->Save(Path.constData()))
			{
				QMessageBox::critical(this, QApplication::applicationName(),
					tr("Error saving VTF texture:\n\n%1").arg(lastErrorString()));
				return false;
			}

			if(m_pAutoCreateVmtFileAction->isChecked())
			{
				VmtFileUtility::CreateDefaultMaterial(sFileName, QStringLiteral("LightmappedGeneric"));
			}
		}
		else if(pDocument->pVMTFile != nullptr)
		{
			if(!confirmVmtFile(iIndex))
			{
				return false;
			}

			QFile File(sFileName);
			if(!File.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
			{
				QMessageBox::critical(this, QApplication::applicationName(),
					tr("Error saving VMT material:\n\n%1").arg(File.errorString()));
				return false;
			}

			File.write(pDocument->pTextDocument->toPlainText().toLocal8Bit());
			File.close();

			pDocument->pTextDocument->setModified(false);
		}
		else
		{
			return false;
		}

		pDocument->sFileName = sFileName;
		setDocumentModified(pDocument, false);
		updateTabText(iIndex);

		FileDialogHistory::remember(FileDialogHistory::s_sFileDirectory, sFileName);

		if(iIndex == m_iCurrentDocument)
		{
			m_sFileName = sFileName;
			m_pStatusFileName->setText(sFileName);
			updateWindowTitle();

			updateHotspotActions();
		}

		addRecentFile(sFileName);

		return true;
	}

	bool MainWindow::saveDocument(int iIndex)
	{
		if(iIndex < 0 || iIndex >= static_cast<int>(m_Documents.size()))
		{
			return false;
		}

		const Document *pDocument = m_Documents.at(static_cast<size_t>(iIndex));

		if(pDocument->sFileName.isEmpty())
		{
			return saveDocumentAs(iIndex);
		}

		return save(iIndex, pDocument->sFileName);
	}

	bool MainWindow::saveDocumentAs(int iIndex)
	{
		if(iIndex < 0 || iIndex >= static_cast<int>(m_Documents.size()))
		{
			return false;
		}

		const Document *pDocument = m_Documents.at(static_cast<size_t>(iIndex));

		QString sFileName;

		const QString &sDefault = pDocument->sFileName.isEmpty()
			? pDocument->sSuggestedFileName : pDocument->sFileName;

		const QString sStartPath = FileDialogHistory::path(FileDialogHistory::s_sFileDirectory, sDefault);

		if(pDocument->pVTFFile != nullptr)
		{
			sFileName = QFileDialog::getSaveFileName(this, tr("Save VTF File"),
				sStartPath, tr("VTF Files (*.vtf)"));
		}
		else if(pDocument->pVMTFile != nullptr)
		{
			const bool bRect = sDefault.endsWith(QLatin1String(".rect"), Qt::CaseInsensitive);

			sFileName = QFileDialog::getSaveFileName(this,
				bRect ? tr("Save Hotspot File") : tr("Save VMT File"),
				sStartPath, bRect ? tr("Hotspot Files (*.rect)") : tr("VMT Files (*.vmt)"));
		}

		if(sFileName.isEmpty())
		{
			return false;
		}

		return save(iIndex, sFileName);
	}

	void MainWindow::import(const QStringList &sFileNames)
	{
		if(m_pOptionsDialog == nullptr)
		{
			m_pOptionsDialog = new VtfOptionsDialog(&m_Options, this);
		}

		if(m_pOptionsDialog->exec() != QDialog::Accepted)
		{
			return;
		}

		bool bError = false;

		vlUInt uiWidth = 0, uiHeight = 0;
		bool bHasAlpha = false;
		bool bFloat = false;

		std::vector<vlByte *> vImageData;

		for(const QString &sFileName : sFileNames)
		{
			if(bError)
			{
				break;
			}

			const QByteArray Path = QDir::toNativeSeparators( sFileName ).toLocal8Bit();

			if ( !ilLoadImage( Path.constData() ) )
			{
				bError = true;

				QMessageBox::critical(this, QApplication::applicationName(), tr("Error loading image."));
				break;
			}

			const ILuint uiImage = static_cast<ILuint>(ilGetInteger(IL_CUR_IMAGE));
			const vlUInt uiImages = static_cast<vlUInt>(ilGetInteger(IL_NUM_IMAGES)) + 1;

			if(vImageData.empty())
			{
				// TODO only deciding this by first image right now
				bFloat = VtfFileUtility::IsFloatImage();
			}

			const vlUInt uiPixelSize = bFloat ? 4 * static_cast<vlUInt>(sizeof(vlSingle)) : 4;

			// Copy every animation frame the file contains.
			for(vlUInt j = 0; j < uiImages; j++)
			{
				ilBindImage(uiImage);
				ilActiveImage(static_cast<ILuint>(j));

				if(!ilConvertImage(IL_RGBA, bFloat ? IL_FLOAT : IL_UNSIGNED_BYTE))
				{
					bError = true;

					QMessageBox::critical(this, QApplication::applicationName(), tr("Error converting image."));
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
					bError = true;

					QMessageBox::critical(this, QApplication::applicationName(),
						tr("Error loading image:\n\nAll frames and faces must be the same size."));
					break;
				}

				const size_t uiFrameSize = static_cast<size_t>( uiWidth ) * uiHeight * uiPixelSize;

				vlByte *lpFrameData = new vlByte[uiFrameSize];
				memcpy(lpFrameData, ilGetData(), uiFrameSize);
				vImageData.push_back(lpFrameData);

				if(!m_Options.StripAlpha)
				{
					bHasAlpha = bHasAlpha || (bFloat
						? VtfFileUtility::HasAlphaDataRGBA32F(reinterpret_cast<vlSingle *>(lpFrameData), uiWidth, uiHeight)
						: VtfFileUtility::HasAlphaData(lpFrameData, uiWidth, uiHeight));
				}
			}

			// Leave the base image bound for the next file.
			ilBindImage(uiImage);
		}

		if(!bError && m_Options.DistanceAlpha && !bFloat && !vImageData.empty())
		{
			VtfFileUtility::ApplyDistanceAlpha(vImageData, uiWidth, uiHeight, m_Options);
			bHasAlpha = true;
		}

		if(!bError)
		{
			createFromImages(vImageData, uiWidth, uiHeight, bHasAlpha, bFloat,
				sFileNames.isEmpty() ? QString() : sFileNames.first());
		}

		for(vlByte *lpFrameData : vImageData)
		{
			delete[] lpFrameData;
		}
	}

	void MainWindow::createFromImages(const std::vector<vlByte *> &vImageData, vlUInt uiWidth, vlUInt uiHeight,
		bool bHasAlpha, bool bFloat, const QString &sSourceFileName)
	{
		VTFLib::CVTFFile *pVTFFile = new VTFLib::CVTFFile();

		const vlUInt uiImages = static_cast<vlUInt>(vImageData.size());
		vlByte **lpImageData = uiImages != 0 ? const_cast<vlByte **>(&vImageData[0]) : nullptr;

		const vlUInt uiFrames = m_Options.TextureType == VtfTextureType::Animated ? uiImages : 1;
		const vlUInt uiFaces = m_Options.TextureType == VtfTextureType::EnvironmentMap ? uiImages : 1;
		const vlUInt uiSlices = m_Options.TextureType == VtfTextureType::Volume ? uiImages : 1;

		SVTFCreateOptions VTFCreateOptions = VtfFileUtility::GetCreateOptions(m_Options);
		VTFCreateOptions.ImageFormat = bHasAlpha ? m_Options.AlphaFormat : m_Options.NormalFormat;

		const bool bCreated =
			pVTFFile->Create(uiWidth, uiHeight, uiFrames, uiFaces, uiSlices, lpImageData, VTFCreateOptions,
				bFloat ? IMAGE_FORMAT_RGBA32323232F : IMAGE_FORMAT_RGBA8888) != vlFalse;
		if(bCreated)
		{
			VtfFileUtility::ApplyFlags(m_Options, pVTFFile);
		}

		if(bCreated && VtfFileUtility::CreateResources(m_Options, pVTFFile))
		{
			Document *pDocument = new Document();
			pDocument->pVTFFile = pVTFFile;

			// name the document after the imported image
			if(!sSourceFileName.isEmpty())
			{
				const QFileInfo Info(sSourceFileName);

				pDocument->sUntitledName = Info.completeBaseName() + QLatin1String(".vtf");
				pDocument->sSuggestedFileName = Info.absoluteDir().filePath(pDocument->sUntitledName);
			}

			pDocument->bModified = true;

			addDocument(pDocument);
		}
		else
		{
			delete pVTFFile;

			QMessageBox::critical(this, QApplication::applicationName(),
				tr("Error creating VTF texture:\n\n%1").arg(lastErrorString()));
		}
	}

	void MainWindow::exportImage(const QString &sFileName)
	{
		if(m_pVTFFile == nullptr)
		{
			return;
		}

		vlUInt uiWidth = 0, uiHeight = 0, uiDepth = 0;
		m_pVTFFile->ComputeMipmapDimensions(m_pVTFFile->GetWidth(), m_pVTFFile->GetHeight(),
			m_pVTFFile->GetDepth(), static_cast<vlUInt>(m_pMipmap->value()), uiWidth, uiHeight, uiDepth);

		const bool bFloat = VtfFileUtility::IsFloatImageFileName(sFileName);
		const VTFImageFormat DestFormat = bFloat ? IMAGE_FORMAT_RGBA32323232F : IMAGE_FORMAT_RGBA8888;

		std::vector<vlByte> ImageData(m_pVTFFile->ComputeImageSize(uiWidth, uiHeight, 1, DestFormat));

		m_pVTFFile->Convert(
			m_pVTFFile->GetData(static_cast<vlUInt>(m_pFrame->value()), static_cast<vlUInt>(m_pFace->value()),
				static_cast<vlUInt>(m_pSlice->value()), static_cast<vlUInt>(m_pMipmap->value())),
			ImageData.data(), uiWidth, uiHeight, m_pVTFFile->GetDecodeFormat(), DestFormat);

		// DevIL likes image data upside down...
		if(bFloat)
		{
			m_pVTFFile->FlipImageRGBA32F(reinterpret_cast<vlSingle *>(ImageData.data()), uiWidth, uiHeight);
		}
		else
		{
			m_pVTFFile->FlipImage(ImageData.data(), uiWidth, uiHeight);
		}

		const QByteArray Path = QDir::toNativeSeparators(sFileName).toLocal8Bit();

		if(!(ilTexImage(uiWidth, uiHeight, 1, 4, IL_RGBA, bFloat ? IL_FLOAT : IL_UNSIGNED_BYTE, ImageData.data())
			&& ilSaveImage(Path.constData())))
		{
			QMessageBox::critical(this, QApplication::applicationName(), tr("Error saving image."));
		}
	}

	void MainWindow::exportAllImages(const QString &sFileName)
	{
		if(m_pVTFFile == nullptr)
		{
			return;
		}

		const QFileInfo Info(sFileName);
		const QString sSuffix = Info.completeSuffix().isEmpty()
			? QString() : QLatin1Char('.') + Info.completeSuffix();
		const QString sStem = Info.completeSuffix().isEmpty()
			? sFileName : sFileName.left(sFileName.length() - sSuffix.length());

		vlUInt uiWidth = 0, uiHeight = 0, uiDepth = 0;
		m_pVTFFile->ComputeMipmapDimensions(m_pVTFFile->GetWidth(), m_pVTFFile->GetHeight(),
			m_pVTFFile->GetDepth(), static_cast<vlUInt>(m_pMipmap->value()), uiWidth, uiHeight, uiDepth);

		const bool bFloat = VtfFileUtility::IsFloatImageFileName(sFileName);
		const VTFImageFormat DestFormat = bFloat ? IMAGE_FORMAT_RGBA32323232F : IMAGE_FORMAT_RGBA8888;

		std::vector<vlByte> ImageData(m_pVTFFile->ComputeImageSize(uiWidth, uiHeight, 1, DestFormat));

		for(vlUInt i = 0; i < m_pVTFFile->GetFrameCount(); i++)
		{
			for(vlUInt j = 0; j < m_pVTFFile->GetFaceCount(); j++)
			{
				for(vlUInt k = 0; k < m_pVTFFile->GetDepth(); k++)
				{
					m_pVTFFile->Convert(
						m_pVTFFile->GetData(i, j, k, static_cast<vlUInt>(m_pMipmap->value())),
						ImageData.data(), uiWidth, uiHeight, m_pVTFFile->GetDecodeFormat(), DestFormat);

					if(bFloat)
					{
						m_pVTFFile->FlipImageRGBA32F(reinterpret_cast<vlSingle *>(ImageData.data()), uiWidth, uiHeight);
					}
					else
					{
						m_pVTFFile->FlipImage(ImageData.data(), uiWidth, uiHeight);
					}

					const QString sFramePath = QStringLiteral("%1_%2_%3_%4%5")
						.arg(sStem)
						.arg(i, 2, 10, QLatin1Char('0'))
						.arg(j, 2, 10, QLatin1Char('0'))
						.arg(k, 2, 10, QLatin1Char('0'))
						.arg(sSuffix);

					const QByteArray Path = QDir::toNativeSeparators(sFramePath).toLocal8Bit();

					if(!(ilTexImage(uiWidth, uiHeight, 1, 4, IL_RGBA, bFloat ? IL_FLOAT : IL_UNSIGNED_BYTE, ImageData.data())
						&& ilSaveImage(Path.constData())))
					{
						QMessageBox::critical(this, QApplication::applicationName(), tr("Error saving image."));
					}
				}
			}
		}
	}

	void MainWindow::hideVtfSidebars()
	{
		// "Hide" the tab pages
		for(QWidget *pTab : { m_pResourcesTab, m_pInfoTab })
		{
			const int iIndex = m_pRightTabs->indexOf(pTab);
			if(iIndex != -1)
			{
				m_pRightTabs->removeTab(iIndex);
				pTab->setParent(this);
			}
		}

		const int iImageIndex = m_pLeftTabs->indexOf(m_pImageTab);
		if(iImageIndex != -1)
		{
			m_pLeftTabs->removeTab(iImageIndex);
			m_pImageTab->setParent(this);
		}

		updateSidebarsVisible();
	}

	void MainWindow::clearWidgets()
	{
		m_pVTFFile = nullptr;
		m_pVMTFile = nullptr;

		m_pHdrExposure->setEnabled(false);

		m_pAnimateButton->setText(tr("&Play"));
		m_pAnimateButton->setEnabled(false);
		m_pAnimateFps->setEnabled(false);
		m_pAnimateTimer->stop();

		hideVtfSidebars();

		m_pImageView->setImage(QImage());
		invalidateImageCache();

		m_iVmtErrorLine = 0;
		m_pVmtEdit->setDocument(m_pEmptyDocument);
		updateVmtErrorHighlight();

		m_pCentralStack->setCurrentWidget(m_pImageScrollArea);

		m_sFileName.clear();
		m_pStatusFileName->clear();
		m_pStatusInfo1->clear();
		m_pStatusInfo2->clear();

		updateWindowTitle();
	}

	void MainWindow::updateWindowTitle()
	{
		const Document *pDocument = currentDocument();

		if(pDocument == nullptr)
		{
			setWindowTitle(QApplication::applicationName());
			return;
		}

		setWindowTitle(QStringLiteral("%1%2 - %3")
			.arg(documentTitle(pDocument),
				pDocument->bModified ? QStringLiteral("*") : QString(),
				QApplication::applicationName()));
	}

	//
	// Menu handlers.
	//

	void MainWindow::onNew()
	{
		newFile();
	}

	void MainWindow::onOpen()
	{
		const QString sFileName = QFileDialog::getOpenFileName(this, tr("Open"),
			FileDialogHistory::s_sFileDirectory,
			tr("Supported Files (*.vmt *.vtf *.rect);;VMT Files (*.vmt);;VTF Files (*.vtf);;"
				"Hotspot Files (*.rect);;All Files (*.*)"));

		if(!sFileName.isEmpty())
		{
			open(sFileName, false);
		}
	}

	void MainWindow::onSave()
	{
		saveDocument(m_iCurrentDocument);
	}

	void MainWindow::onSaveAs()
	{
		saveDocumentAs(m_iCurrentDocument);
	}

	void MainWindow::onSaveAll()
	{
		// saving may pop a dialog which can change the current tab
		// so work by document rather than by index
		std::vector<Document *> Documents = m_Documents;

		for(Document *pDocument : Documents)
		{
			if(!pDocument->bModified)
			{
				continue;
			}

			for(size_t i = 0; i < m_Documents.size(); i++)
			{
				if(m_Documents.at(i) == pDocument)
				{
					saveDocument(static_cast<int>(i));
					break;
				}
			}
		}
	}

	void MainWindow::onClose()
	{
		closeDocument(m_iCurrentDocument);
	}

	void MainWindow::onCloseAll()
	{
		while(!m_Documents.empty())
		{
			if(!closeDocument(static_cast<int>(m_Documents.size()) - 1))
			{
				break;
			}
		}
	}

	void MainWindow::onNextTab()
	{
		if(m_Documents.size() > 1)
		{
			m_pTabBar->setCurrentIndex((m_pTabBar->currentIndex() + 1) % m_pTabBar->count());
		}
	}

	void MainWindow::onPreviousTab()
	{
		if(m_Documents.size() > 1)
		{
			m_pTabBar->setCurrentIndex((m_pTabBar->currentIndex() + m_pTabBar->count() - 1)
				% m_pTabBar->count());
		}
	}

	void MainWindow::switchToTab(int iIndex)
	{
		const int iCount = m_pTabBar->count();

		if(iIndex < 0)
		{
			iIndex = iCount - 1;
		}

		if(iIndex >= 0 && iIndex < iCount)
		{
			m_pTabBar->setCurrentIndex(iIndex);
		}
	}

	void MainWindow::onTabChanged(int iIndex)
	{
		if(iIndex == m_iCurrentDocument)
		{
			return;
		}

		commitCurrentDocument();

		if(iIndex < 0 || iIndex >= static_cast<int>(m_Documents.size()))
		{
			m_iCurrentDocument = -1;
			clearWidgets();
			updateActions();
			return;
		}

		activateDocument(iIndex);
	}

	void MainWindow::onTabCloseRequested(int iIndex)
	{
		closeDocument(iIndex);
	}

	void MainWindow::onTabMoved(int iFrom, int iTo)
	{
		if(iFrom == iTo
			|| iFrom < 0 || iFrom >= static_cast<int>(m_Documents.size())
			|| iTo < 0 || iTo >= static_cast<int>(m_Documents.size()))
		{
			return;
		}

		Document *pDocument = m_Documents.at(static_cast<size_t>(iFrom));
		m_Documents.erase(m_Documents.begin() + iFrom);
		m_Documents.insert(m_Documents.begin() + iTo, pDocument);

		m_iCurrentDocument = m_pTabBar->currentIndex();
	}

	void MainWindow::onVtfPropertyChanged()
	{
		if(m_bSwitchingDocument)
		{
			return;
		}

		if(Document *pDocument = currentDocument())
		{
			if(pDocument->pVTFFile != nullptr)
			{
				setDocumentModified(pDocument, true);
			}
		}
	}

	void MainWindow::onImport()
	{
		// keep in sync with IsImportableFileName
		const QStringList sFileNames = QFileDialog::getOpenFileNames(this, tr("Import"),
			FileDialogHistory::s_sImageDirectory,
			tr("Supported Files (*.bmp *.dds *.exr *.gif *.hdr *.jpg *.jpeg *.pfm *.png *.tga);;"
				"BMP Files (*.bmp);;DDS Files (*.dds);;EXR Files (*.exr);;GIF Files (*.gif);;"
				"HDR Files (*.hdr);;JPEG Files (*.jpg *.jpeg);;PFM Files (*.pfm);;"
				"PNG Files (*.png);;TGA Files (*.tga);;"
				"All Files (*.*)"));

		if(!sFileNames.isEmpty())
		{
			FileDialogHistory::remember(FileDialogHistory::s_sImageDirectory, sFileNames.first());

			import(sFileNames);
		}
	}

	void MainWindow::onExport()
	{
		const QString sFileName = QFileDialog::getSaveFileName(this, tr("Export"),
			FileDialogHistory::path(FileDialogHistory::s_sImageDirectory,
				QFileInfo(m_sFileName).completeBaseName()),
			tr("BMP Files (*.bmp);;EXR Files (*.exr);;HDR Files (*.hdr);;JPEG Files (*.jpg *.jpeg);;"
				"PFM Files (*.pfm);;PNG Files (*.png);;TGA Files (*.tga)"));

		if(!sFileName.isEmpty())
		{
			FileDialogHistory::remember(FileDialogHistory::s_sImageDirectory, sFileName);

			exportImage(sFileName);
		}
	}

	void MainWindow::onExportAll()
	{
		const QString sFileName = QFileDialog::getSaveFileName(this, tr("Export All"),
			FileDialogHistory::path(FileDialogHistory::s_sImageDirectory,
				QFileInfo(m_sFileName).completeBaseName()),
			tr("BMP Files (*.bmp);;EXR Files (*.exr);;HDR Files (*.hdr);;JPEG Files (*.jpg *.jpeg);;"
				"PFM Files (*.pfm);;PNG Files (*.png);;TGA Files (*.tga)"));

		if(!sFileName.isEmpty())
		{
			FileDialogHistory::remember(FileDialogHistory::s_sImageDirectory, sFileName);

			exportAllImages(sFileName);
		}
	}

	void MainWindow::onCreateVmtFile()
	{
		if(m_pVmtCreateDialog == nullptr)
		{
			m_pVmtCreateDialog = new VmtCreateDialog(this);
		}

		if(m_pVTFFile != nullptr && m_sFileName.endsWith(QLatin1String(".vtf"), Qt::CaseInsensitive))
		{
			m_pVmtCreateDialog->setFromTexture(m_sFileName, *m_pVTFFile);
		}

		m_pVmtCreateDialog->exec();
	}

	void MainWindow::onConvertFolder()
	{
		if(m_pBatchConvertDialog == nullptr)
		{
			m_pBatchConvertDialog = new BatchConvertDialog(&m_Options, &m_BatchConvertSettings, this);
		}

		m_pBatchConvertDialog->exec();
	}

	void MainWindow::onRecentFile()
	{
		QAction *pAction = qobject_cast<QAction *>(sender());
		if(pAction != nullptr)
		{
			open(pAction->data().toString(), false);
		}
	}

	void MainWindow::onReopenRecent()
	{
		for(const QString &sFileName : m_RecentFiles)
		{
			if(indexOfFile(sFileName) < 0 && QFileInfo::exists(sFileName))
			{
				open(sFileName, false);
				return;
			}
		}
	}

	void MainWindow::onCopy()
	{
		if(!m_pImageView->image().isNull())
		{
			QApplication::clipboard()->setImage(
				m_pImageView->image().convertToFormat(QImage::Format_ARGB32));
		}
	}

	void MainWindow::onPaste()
	{
		const QImage Image = QApplication::clipboard()->image();

		if(Image.isNull())
		{
			return;
		}

		if(m_pOptionsDialog == nullptr)
		{
			m_pOptionsDialog = new VtfOptionsDialog(&m_Options, this);
		}

		if(m_pOptionsDialog->exec() != QDialog::Accepted)
		{
			return;
		}

		const QImage Source = Image.convertToFormat(QImage::Format_RGBA8888);

		vlUInt uiWidth = static_cast<vlUInt>(Source.width());
		vlUInt uiHeight = static_cast<vlUInt>(Source.height());

		vlByte *lpImageData = new vlByte[static_cast<size_t>(uiWidth) * uiHeight * 4];
		for(vlUInt j = 0; j < uiHeight; j++)
		{
			memcpy(lpImageData + static_cast<size_t>(j) * uiWidth * 4,
				Source.constScanLine(static_cast<int>(j)), static_cast<size_t>(uiWidth) * 4);
		}

		bool bHasAlpha = !m_Options.StripAlpha
			&& VtfFileUtility::HasAlphaData(lpImageData, uiWidth, uiHeight);

		std::vector<vlByte *> vImageData{ lpImageData };

		if(m_Options.DistanceAlpha)
		{
			VtfFileUtility::ApplyDistanceAlpha(vImageData, uiWidth, uiHeight, m_Options);
			bHasAlpha = true;
		}

		createFromImages(vImageData, uiWidth, uiHeight, bHasAlpha, false, QString());

		delete[] vImageData[0];
	}

	void MainWindow::onChannelChanged()
	{
		updateVtfFile();
	}

	void MainWindow::onViewOptionChanged()
	{
		updateVtfFile();
	}

	void MainWindow::onAbout()
	{
		if(m_pAboutDialog == nullptr)
		{
			m_pAboutDialog = new AboutDialog(this);
		}

		m_pAboutDialog->exec();
	}

	void MainWindow::onClipboardChanged()
	{
		m_pPasteAction->setEnabled(!QApplication::clipboard()->image().isNull());
	}

	//
	// Image tab handlers.
	//

	void MainWindow::onImageParameterChanged()
	{
		if(!m_bHdrResetting)
		{
			updateVtfFile();
		}
	}

	void MainWindow::onAnimateClicked()
	{
		if(m_pAnimateTimer->isActive())
		{
			m_pAnimateTimer->stop();
			m_pAnimateButton->setText(tr("&Play"));
		}
		else
		{
			m_pAnimateTimer->start();
			m_pAnimateButton->setText(tr("&Stop"));
		}
	}

	void MainWindow::onAnimateFpsChanged(int iFps)
	{
		if(iFps > 0)
		{
			m_pAnimateTimer->setInterval(1000 / iFps);
		}
	}

	void MainWindow::onAnimateTick()
	{
		if(!isActiveWindow())
		{
			return;
		}

		int iValue = m_pFrame->value() + 1;
		if(iValue > m_pFrame->maximum())
		{
			iValue = m_pFrame->minimum();
		}

		m_pFrame->setValue(iValue);
	}

	void MainWindow::onFlagItemChanged(QListWidgetItem *pItem)
	{
		if(m_bUpdatingFlags || m_pVTFFile == nullptr)
		{
			return;
		}

		onVtfPropertyChanged();
	}

	void MainWindow::onFileVersionChanged(int iIndex)
	{
		if(m_bUpdatingFileInfo || m_bSwitchingDocument || m_pVTFFile == nullptr || iIndex < 0)
		{
			return;
		}

		const vlUInt uiMinor = m_pFileVersion->itemData(iIndex).value<vlUInt>();

		if(uiMinor == m_pVTFFile->GetMinorVersion())
		{
			return;
		}

		// dropping below v7.3 takes the whole resource directory with it
		if(uiMinor < VTF_MINOR_VERSION_MIN_RESOURCE)
		{
			vlUInt uiExtraResources = 0;
			for(vlUInt i = 0; i < m_pVTFFile->GetResourceCount(); i++)
			{
				const vlUInt uiType = m_pVTFFile->GetResourceType(i);
				if(uiType != VTF_LEGACY_RSRC_LOW_RES_IMAGE && uiType != VTF_LEGACY_RSRC_IMAGE)
				{
					uiExtraResources++;
				}
			}

			if(uiExtraResources != 0
				&& QMessageBox::question(this, QApplication::applicationName(),
					tr("Version %1.%2 has no resource directory, so the %3 resource(s) attached to "
						"this texture will be discarded.\n\nContinue?")
						.arg(VTF_MAJOR_VERSION).arg(uiMinor).arg(uiExtraResources),
					QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
			{
				updateFileInfo();
				return;
			}
		}

		// push the pending header edits in first so they are not lost by the refresh below
		getVtfFile();

		if(!m_pVTFFile->SetVersion(VTF_MAJOR_VERSION, uiMinor))
		{
			QMessageBox::critical(this, QApplication::applicationName(),
				tr("Failed to convert the texture to version %1.%2:\n%3")
					.arg(VTF_MAJOR_VERSION).arg(uiMinor)
					.arg(QString::fromLatin1(vlGetLastError())));

			updateFileInfo();
			return;
		}

		updateFileInfo();

		m_bSwitchingDocument = true;
		m_pImageStartFrame->setValue(m_pVTFFile->GetStartFrame() == 0xffff
			? 0 : static_cast<int>(m_pVTFFile->GetStartFrame()));
		m_bSwitchingDocument = false;

		m_pImageFaces->setText(QString::number(m_pVTFFile->GetFaceCount()));

		updateResourceList();
		updateSheetActions();

		onVtfPropertyChanged();
	}

	void MainWindow::onHdrReset()
	{
		m_bHdrResetting = true;

		m_pHdrExposure->setMinimum(0);
		m_pHdrExposure->setMaximum(8000);
		m_pHdrExposure->setPageStep(1);
		m_pHdrExposure->setTickInterval(160);
		m_pHdrExposure->setValue(2000);

		m_bHdrResetting = false;

		updateVtfFile();
	}

	//
	// Zoom and pan.
	//

	void MainWindow::zoomAt(float fFactor, const QPoint &Anchor)
	{
		if(m_pVTFFile == nullptr || m_pImageView->image().isNull())
		{
			return;
		}

		const QSize ImageSize = m_pImageView->displaySize();

		const float dMaximumDimension = static_cast<float>(ImageView::maximumDisplayDimension());

		if(fFactor > 1.0f
			&& (static_cast<float>(ImageSize.width()) * fFactor > dMaximumDimension
				|| static_cast<float>(ImageSize.height()) * fFactor > dMaximumDimension))
		{
			return;
		}
		if(fFactor < 1.0f && ImageSize.width() <= 1 && ImageSize.height() <= 1)
		{
			return;
		}

		QScrollBar *pHorizontal = m_pImageScrollArea->horizontalScrollBar();
		QScrollBar *pVertical = m_pImageScrollArea->verticalScrollBar();

		const float fOldImageScale = m_fImageScale;

		const float fMargin = static_cast<float>(ImageView::margin());
		const float fContentX = static_cast<float>(Anchor.x() + pHorizontal->value()) - fMargin;
		const float fContentY = static_cast<float>(Anchor.y() + pVertical->value()) - fMargin;

		m_fImageScale *= fFactor;

		// This may clamp the scale
		updateVtfFile();

		const float fRatio = m_fImageScale / fOldImageScale;

		pHorizontal->setValue(static_cast<int>(fContentX * fRatio + fMargin) - Anchor.x());
		pVertical->setValue(static_cast<int>(fContentY * fRatio + fMargin) - Anchor.y());
	}

	void MainWindow::zoom(float fFactor)
	{
		const QSize ViewportSize = m_pImageScrollArea->viewport()->size();

		zoomAt(fFactor, QPoint(ViewportSize.width() / 2, ViewportSize.height() / 2));
	}

	void MainWindow::onZoomIn()
	{
		zoom(2.0f);
	}

	void MainWindow::onZoomOut()
	{
		zoom(0.5f);
	}

	void MainWindow::onZoomReset()
	{
		if(m_pVTFFile == nullptr)
		{
			return;
		}

		m_fImageScale = 1.0f;
		updateVtfFile();

		m_pImageScrollArea->horizontalScrollBar()->setValue(ImageView::margin());
		m_pImageScrollArea->verticalScrollBar()->setValue(ImageView::margin());
	}

	void MainWindow::onImageContextMenu(const QPoint &Position)
	{
		m_pImageContextMenu->exec(m_pImageView->mapToGlobal(Position));
	}

	void MainWindow::onVmtContextMenu(const QPoint &Position)
	{
		QMenu *pMenu = m_pVmtEdit->createStandardContextMenu(Position);

		pMenu->addSeparator();

		QMenu *pValidate = pMenu->addMenu(tr("&Validate"));
		QAction *pLoose = pValidate->addAction(tr("&Loose"));
		connect(pLoose, &QAction::triggered, this, &MainWindow::onValidateLoose);
		QAction *pStrict = pValidate->addAction(tr("&Strict"));
		connect(pStrict, &QAction::triggered, this, &MainWindow::onValidateStrict);

		pMenu->exec(m_pVmtEdit->mapToGlobal(Position));

		delete pMenu;
	}

	void MainWindow::onImageMouseMoved(int iX, int iY)
	{
		if(m_bImagePanning)
		{
			return;
		}

		m_pStatusInfo2->setText(QStringLiteral("%1, %2")
			.arg(static_cast<int>(static_cast<float>(iX) / m_fEffectiveImageScale) + 1)
			.arg(static_cast<int>(static_cast<float>(iY) / m_fEffectiveImageScale) + 1));
	}

	bool MainWindow::eventFilter(QObject *pObject, QEvent *pEvent)
	{
		// keep the indentation of the current line when starting a new one
		if(pObject == m_pVmtEdit && pEvent->type() == QEvent::KeyPress)
		{
			QKeyEvent *pKey = static_cast<QKeyEvent *>(pEvent);

			if((pKey->key() == Qt::Key_Return || pKey->key() == Qt::Key_Enter)
				&& (pKey->modifiers() & ~Qt::KeypadModifier) == Qt::NoModifier
				&& !m_pVmtEdit->isReadOnly())
			{
				QTextCursor Cursor = m_pVmtEdit->textCursor();
				const QString Line = Cursor.block().text();

				int iIndent = 0;
				while(iIndent < Line.length() 
					&& (Line.at(iIndent) == QLatin1Char(' ')
					|| Line.at(iIndent) == QLatin1Char('\t')))
				{
					iIndent++;
				}

				iIndent = qMin(iIndent, Cursor.positionInBlock());

				Cursor.beginEditBlock();
				Cursor.insertText(QLatin1String("\n") + Line.left(iIndent));
				Cursor.endEditBlock();

				m_pVmtEdit->setTextCursor(Cursor);
				m_pVmtEdit->ensureCursorVisible();
				return true;
			}
		}

		const bool bImageArea = pObject == m_pImageScrollArea->viewport() || pObject == m_pImageView;

		if(bImageArea)
		{
			switch(pEvent->type())
			{
			case QEvent::Wheel:
			{
				QWheelEvent *pWheel = static_cast<QWheelEvent *>(pEvent);

				if(m_pVTFFile != nullptr && !m_pImageView->image().isNull() && pWheel->angleDelta().y() != 0)
				{
					const QPoint Anchor = m_pImageScrollArea->viewport()->mapFromGlobal(
						pWheel->globalPosition().toPoint());

					zoomAt(pWheel->angleDelta().y() > 0 ? 2.0f : 0.5f, Anchor);
					return true;
				}
				break;
			}

			case QEvent::MouseButtonPress:
			{
				QMouseEvent *pMouse = static_cast<QMouseEvent *>(pEvent);

				// Left drag pans
				if(m_pVTFFile != nullptr && pMouse->button() == Qt::LeftButton)
				{
					m_bImagePanning = true;
					m_ImagePanStartMouse = pMouse->globalPosition().toPoint();
					m_ImagePanStartScroll = QPoint(
						m_pImageScrollArea->horizontalScrollBar()->value(),
						m_pImageScrollArea->verticalScrollBar()->value());
					m_pImageScrollArea->viewport()->setCursor(Qt::ClosedHandCursor);
					return true;
				}
				break;
			}

			case QEvent::MouseMove:
			{
				if(m_bImagePanning)
				{
					QMouseEvent *pMouse = static_cast<QMouseEvent *>(pEvent);
					const QPoint Delta = pMouse->globalPosition().toPoint() - m_ImagePanStartMouse;

					m_pImageScrollArea->horizontalScrollBar()->setValue(m_ImagePanStartScroll.x() - Delta.x());
					m_pImageScrollArea->verticalScrollBar()->setValue(m_ImagePanStartScroll.y() - Delta.y());
					return true;
				}
				break;
			}

			case QEvent::MouseButtonRelease:
			{
				if(m_bImagePanning)
				{
					m_bImagePanning = false;
					m_pImageScrollArea->viewport()->unsetCursor();
					return true;
				}
				break;
			}

			default:
				break;
			}
		}

		return QMainWindow::eventFilter(pObject, pEvent);
	}

	//
	// VMT handlers.
	//

	void MainWindow::onVmtTextChanged()
	{
		validateVmtFile();
		updateVmtErrorHighlight();
	}

	void MainWindow::onVmtCursorChanged()
	{
		if(m_pVMTFile == nullptr)
		{
			return;
		}

		const QTextCursor Cursor = m_pVmtEdit->textCursor();

		m_pStatusInfo2->setText(tr("Ln %1, Col %2")
			.arg(Cursor.blockNumber() + 1)
			.arg(Cursor.positionInBlock() + 1));
	}

	void MainWindow::onValidateLoose()
	{
		if(validateVmtFile())
		{
			QMessageBox::information(this, QApplication::applicationName(), tr("VMT validation successful."));
		}
		else
		{
			QMessageBox::critical(this, QApplication::applicationName(),
				tr("Error validating VMT:\n\n%1").arg(lastErrorString()));
		}

		updateVmtErrorHighlight();
	}

	void MainWindow::onValidateStrict()
	{
		vlSetInteger(VTFLIB_VMT_PARSE_MODE, PARSE_MODE_STRICT);
		onValidateLoose();
		vlSetInteger(VTFLIB_VMT_PARSE_MODE, PARSE_MODE_LOOSE);
	}

	void MainWindow::onVmtEditorOptions()
	{
		if(m_pVmtEditorOptionsDialog == nullptr)
		{
			m_pVmtEditorOptionsDialog = new VmtEditorOptionsDialog(&m_VmtEditorSettings, this);
		}

		if(m_pVmtEditorOptionsDialog->exec() == QDialog::Accepted)
		{
			applyVmtEditorSettings();
		}
	}

	//
	// Recent files.
	//

	void MainWindow::addRecentFile(const QString &sFileName)
	{
		int iExisting = -1;
		for(int i = 0; i < m_RecentFiles.count(); i++)
		{
			if(m_RecentFiles.at(i).compare(sFileName, Qt::CaseInsensitive) == 0)
			{
				iExisting = i;
				break;
			}
		}

		if(iExisting == 0)
		{
			return;
		}

		if(iExisting > 0)
		{
			m_RecentFiles.removeAt(iExisting);
		}

		m_RecentFiles.prepend(sFileName);

		while(m_RecentFiles.count() > m_iMaximumRecentFiles)
		{
			m_RecentFiles.removeLast();
		}

		updateRecentFiles();
	}

	void MainWindow::updateRecentFiles()
	{
		m_pRecentFilesMenu->clear();

		for(const QString &sFileName : m_RecentFiles)
		{
			QAction *pAction = m_pRecentFilesMenu->addAction(sFileName);
			pAction->setData(sFileName);
			connect(pAction, &QAction::triggered, this, &MainWindow::onRecentFile);
		}

		m_pRecentFilesMenu->menuAction()->setVisible(!m_RecentFiles.isEmpty());
	}

	//
	// Configuration.
	//

	QString MainWindow::configFilePath()
	{
		const QString sDirectory = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

		QDir().mkpath(sDirectory);

		return QDir(sDirectory).filePath(QApplication::applicationName() + QStringLiteral(".ini"));
	}

	// can't think of a better way of doing this.. sorry
	bool MainWindow::readSingleInstanceSetting()
	{
		QFile File(configFilePath());
		if(!File.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			return true;
		}

		QTextStream Stream(&File);

		while(!Stream.atEnd())
		{
			const QString sLine = Stream.readLine();
			const int iIndex = sLine.indexOf(QLatin1Char('='));

			if(iIndex == -1)
			{
				continue;
			}

			if(sLine.left(iIndex).trimmed().compare(QLatin1String("VTFEdit.SingleInstance"), Qt::CaseInsensitive) != 0)
			{
				continue;
			}

			const QString sVal = sLine.mid(iIndex + 1).trimmed();

			return sVal.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0
				|| sVal == QLatin1String("1");
		}

		return true;
	}

	QRect MainWindow::sanitizeWindowGeometry(const QRect &Geometry) const
	{
		QRect Result = Geometry;

		if(Result.width() < qMax(minimumWidth(), 320))
			Result.setWidth(qMax(minimumWidth(), 1024));

		if(Result.height() < qMax(minimumHeight(), 240))
			Result.setHeight(qMax(minimumHeight(), 600));

		const QScreen *pScreen = QGuiApplication::screenAt(Result.center());
		if(pScreen == nullptr)
		{
			pScreen = QGuiApplication::screenAt(Result.topLeft());
		}

		if(pScreen == nullptr)
		{
			pScreen = QGuiApplication::primaryScreen();

			if(pScreen == nullptr)
			{
				return Result;
			}

			const QRect Available = pScreen->availableGeometry();

			Result.setSize(Result.size().boundedTo(Available.size()));
			Result.moveCenter(Available.center());

			return Result;
		}

		const QRect Available = pScreen->availableGeometry();

		Result.setSize(Result.size().boundedTo(Available.size()));

		if(Result.right() > Available.right())
			Result.moveRight(Available.right());
		if(Result.bottom() > Available.bottom())
			Result.moveBottom(Available.bottom());
		if(Result.left() < Available.left())
			Result.moveLeft(Available.left());
		if(Result.top() < Available.top())
			Result.moveTop(Available.top());

		return Result;
	}

	bool MainWindow::readConfigFile(const QString &sConfigFile)
	{
		QFile File(sConfigFile);
		if(!File.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			return false;
		}

		QTextStream Stream(&File);

		QPoint Location = pos();
		QSize Size = size();
		bool bMaximized = false;
		int iSidebarSplit = 258;
		int iSidebarRightSplit = 258;

		const auto toBool = [](const QString &sValue)
		{
			return sValue.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0
				|| sValue == QLatin1String("1");
		};

		while(!Stream.atEnd())
		{
			const QString sLine = Stream.readLine();
			const int iIndex = sLine.indexOf(QLatin1Char('='));

			if(iIndex == -1)
			{
				continue;
			}

			const QString sArg = sLine.left(iIndex).trimmed();
			const QString sVal = sLine.mid(iIndex + 1).trimmed();

			if(sArg.compare(QLatin1String("VTFEdit.AnimationFrameInterval"), Qt::CaseInsensitive) == 0)
			{
				const int iInterval = qBound(10, sVal.toInt(), 1000);
				m_pAnimateTimer->setInterval(iInterval);
				m_pAnimateFps->setValue(1000 / iInterval);
			}
			else if(sArg.compare(QLatin1String("VTFEdit.Mask"), Qt::CaseInsensitive) == 0)
				m_pMaskAction->setChecked(toBool(sVal));
			else if(sArg.compare(QLatin1String("VTFEdit.Tile"), Qt::CaseInsensitive) == 0)
				m_pTileAction->setChecked(toBool(sVal));
			else if(sArg.compare(QLatin1String("VTFEdit.MipmapFullSize"), Qt::CaseInsensitive) == 0)
				m_pMipmapFullSizeAction->setChecked(toBool(sVal));
			else if(sArg.compare(QLatin1String("VTFEdit.AutoCreateVMTFile"), Qt::CaseInsensitive) == 0)
				m_pAutoCreateVmtFileAction->setChecked(toBool(sVal));
			else if(sArg.compare(QLatin1String("VTFEdit.SingleInstance"), Qt::CaseInsensitive) == 0)
				m_pSingleInstanceAction->setChecked(toBool(sVal));
			else if(sArg.compare(QLatin1String("VTFEdit.LastFileDirectory"), Qt::CaseInsensitive) == 0)
			{
				if(QDir(sVal).exists())
					FileDialogHistory::s_sFileDirectory = sVal;
			}
			else if(sArg.compare(QLatin1String("VTFEdit.LastImageDirectory"), Qt::CaseInsensitive) == 0)
			{
				if(QDir(sVal).exists())
					FileDialogHistory::s_sImageDirectory = sVal;
			}

			else if(sArg.compare(QLatin1String("VmtEditor.FontFamily"), Qt::CaseInsensitive) == 0)
			{
				if(!sVal.isEmpty())
					m_VmtEditorSettings.sFontFamily = sVal;
			}
			else if(sArg.compare(QLatin1String("VmtEditor.FontSize"), Qt::CaseInsensitive) == 0)
				m_VmtEditorSettings.iFontSize = qBound(4, sVal.toInt(), 72);
			else if(sArg.compare(QLatin1String("VmtEditor.TabSize"), Qt::CaseInsensitive) == 0)
				m_VmtEditorSettings.iTabSize = qBound(1, sVal.toInt(), 16);
			else if(sArg.compare(QLatin1String("VmtEditor.Theme"), Qt::CaseInsensitive) == 0)
			{
				if(sVal.compare(QLatin1String("Light"), Qt::CaseInsensitive) == 0)
					m_VmtEditorSettings.eTheme = VmtEditorTheme::Light;
				else if(sVal.compare(QLatin1String("Dark"), Qt::CaseInsensitive) == 0)
					m_VmtEditorSettings.eTheme = VmtEditorTheme::Dark;
				else
					m_VmtEditorSettings.eTheme = VmtEditorTheme::System;
			}

			else if(sArg.compare(QLatin1String("Forms.VTFEdit.Location.X"), Qt::CaseInsensitive) == 0)
				Location.setX(sVal.toInt());
			else if(sArg.compare(QLatin1String("Forms.VTFEdit.Location.Y"), Qt::CaseInsensitive) == 0)
				Location.setY(sVal.toInt());
			else if(sArg.compare(QLatin1String("Forms.VTFEdit.Size.Width"), Qt::CaseInsensitive) == 0)
				Size.setWidth(sVal.toInt());
			else if(sArg.compare(QLatin1String("Forms.VTFEdit.Size.Height"), Qt::CaseInsensitive) == 0)
				Size.setHeight(sVal.toInt());
			else if(sArg.compare(QLatin1String("Forms.VTFEdit.WindowState"), Qt::CaseInsensitive) == 0)
				bMaximized = sVal.compare(QLatin1String("Maximized"), Qt::CaseInsensitive) == 0;
			else if(sArg.compare(QLatin1String("Forms.VTFEdit.Sidebar.SplitPosition"), Qt::CaseInsensitive) == 0)
				iSidebarSplit = sVal.toInt();
			else if(sArg.compare(QLatin1String("Forms.VTFEdit.SidebarRight.SplitPosition"), Qt::CaseInsensitive) == 0)
				iSidebarRightSplit = sVal.toInt();

			else if(sArg.compare(QLatin1String("Forms.BatchConvert.InputFolder"), Qt::CaseInsensitive) == 0)
				m_BatchConvertSettings.sInputFolder = sVal;
			else if(sArg.compare(QLatin1String("Forms.BatchConvert.OutputFolder"), Qt::CaseInsensitive) == 0)
				m_BatchConvertSettings.sOutputFolder = sVal;
			else if(sArg.compare(QLatin1String("Forms.BatchConvert.ToVTF"), Qt::CaseInsensitive) == 0)
				m_BatchConvertSettings.bToVTF = toBool(sVal);
			else if(sArg.compare(QLatin1String("Forms.BatchConvert.ToVTFFilter"), Qt::CaseInsensitive) == 0)
				m_BatchConvertSettings.sToVTFFilter = sVal;
			else if(sArg.compare(QLatin1String("Forms.BatchConvert.FromVTFFormat"), Qt::CaseInsensitive) == 0)
				m_BatchConvertSettings.sFromVTFFormat = sVal;
			else if(sArg.compare(QLatin1String("Forms.BatchConvert.FromVTFFilter"), Qt::CaseInsensitive) == 0)
				m_BatchConvertSettings.sFromVTFFilter = sVal;
			else if(sArg.compare(QLatin1String("Forms.BatchConvert.Recurse"), Qt::CaseInsensitive) == 0)
				m_BatchConvertSettings.bRecurse = toBool(sVal);
			else if(sArg.compare(QLatin1String("Forms.BatchConvert.CreateVMTFiles"), Qt::CaseInsensitive) == 0)
				m_BatchConvertSettings.bCreateVMTFiles = toBool(sVal);

			else if(sArg.compare(QLatin1String("VTFOptions.NormalFormat"), Qt::CaseInsensitive) == 0)
				m_Options.NormalFormat = static_cast<VTFImageFormat>(sVal.toInt());
			else if(sArg.compare(QLatin1String("VTFOptions.AlphaFormat"), Qt::CaseInsensitive) == 0)
				m_Options.AlphaFormat = static_cast<VTFImageFormat>(sVal.toInt());
			else if(sArg.compare(QLatin1String("VTFOptions.TextureType"), Qt::CaseInsensitive) == 0)
				m_Options.TextureType = static_cast<VtfTextureType>(sVal.toInt());
			else if(sArg.compare(QLatin1String("VTFOptions.FlagClampS"), Qt::CaseInsensitive) == 0)
				m_Options.FlagClampS = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.FlagClampT"), Qt::CaseInsensitive) == 0)
				m_Options.FlagClampT = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.FlagNoLOD"), Qt::CaseInsensitive) == 0)
				m_Options.FlagNoLOD = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.FlagPointSample"), Qt::CaseInsensitive) == 0)
				m_Options.FlagPointSample = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.StripAlpha"), Qt::CaseInsensitive) == 0)
				m_Options.StripAlpha = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.sRGB"), Qt::CaseInsensitive) == 0)
				m_Options.sRGB = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.DistanceAlpha"), Qt::CaseInsensitive) == 0)
				m_Options.DistanceAlpha = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.DistanceAlphaSpread"), Qt::CaseInsensitive) == 0)
				m_Options.DistanceAlphaSpread = sVal.toFloat();
			else if(sArg.compare(QLatin1String("VTFOptions.DistanceAlphaReduce"), Qt::CaseInsensitive) == 0)
				m_Options.DistanceAlphaReduce = sVal.toUInt();
			else if(sArg.compare(QLatin1String("VTFOptions.DistanceAlphaThreshold"), Qt::CaseInsensitive) == 0)
				m_Options.DistanceAlphaThreshold = sVal.toUInt();

			else if(sArg.compare(QLatin1String("VTFOptions.Resize"), Qt::CaseInsensitive) == 0)
				m_Options.ResizeImage = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.ResizeMethod"), Qt::CaseInsensitive) == 0)
				m_Options.ResizeMethod = static_cast<VTFResizeMethod>(sVal.toInt());
			else if(sArg.compare(QLatin1String("VTFOptions.ResizeFilter"), Qt::CaseInsensitive) == 0)
				m_Options.ResizeFilter = static_cast<VTFMipmapFilter>(sVal.toInt());
			else if(sArg.compare(QLatin1String("VTFOptions.ResizeClamp"), Qt::CaseInsensitive) == 0)
				m_Options.ResizeClamp = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.ResizeClampWidth"), Qt::CaseInsensitive) == 0)
				m_Options.ResizeClampWidth = sVal.toUInt();
			else if(sArg.compare(QLatin1String("VTFOptions.ResizeClampHeight"), Qt::CaseInsensitive) == 0)
				m_Options.ResizeClampHeight = sVal.toUInt();

			else if(sArg.compare(QLatin1String("VTFOptions.GenerateMipmaps"), Qt::CaseInsensitive) == 0)
				m_Options.GenerateMipmaps = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.MipmapFilter"), Qt::CaseInsensitive) == 0)
				m_Options.MipmapFilter = static_cast<VTFMipmapFilter>(sVal.toInt());

			else if(sArg.compare(QLatin1String("VTFOptions.Version"), Qt::CaseInsensitive) == 0)
				m_Options.Version = sVal;
			else if(sArg.compare(QLatin1String("VTFOptions.AuxCompressionLevel"), Qt::CaseInsensitive) == 0)
				m_Options.AuxCompressionLevel = static_cast<vlShort>(sVal.toInt());
			else if(sArg.compare(QLatin1String("VTFOptions.AuxCompressionMethod"), Qt::CaseInsensitive) == 0)
				m_Options.AuxCompressionMethod = static_cast<vlShort>(sVal.toInt());

			else if(sArg.compare(QLatin1String("VTFOptions.ComputeReflectivity"), Qt::CaseInsensitive) == 0)
				m_Options.ComputeReflectivity = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.GenerateThumbnail"), Qt::CaseInsensitive) == 0)
				m_Options.GenerateThumbnail = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.GenerateSphereMap"), Qt::CaseInsensitive) == 0)
				m_Options.GenerateSphereMap = toBool(sVal);

			else if(sArg.compare(QLatin1String("VTFOptions.CorrectGamma"), Qt::CaseInsensitive) == 0)
				m_Options.CorrectGamma = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.GammaCorrection"), Qt::CaseInsensitive) == 0)
				m_Options.GammaCorrection = sVal.toFloat();

			else if(sArg.compare(QLatin1String("VTFOptions.LuminanceWeightR"), Qt::CaseInsensitive) == 0)
				m_Options.LuminanceWeightR = sVal.toFloat();
			else if(sArg.compare(QLatin1String("VTFOptions.LuminanceWeightG"), Qt::CaseInsensitive) == 0)
				m_Options.LuminanceWeightG = sVal.toFloat();
			else if(sArg.compare(QLatin1String("VTFOptions.LuminanceWeightB"), Qt::CaseInsensitive) == 0)
				m_Options.LuminanceWeightB = sVal.toFloat();

			else if(sArg.compare(QLatin1String("VTFOptions.CreateLODControlResource"), Qt::CaseInsensitive) == 0)
				m_Options.CreateLODControlResource = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.LODControlClampU"), Qt::CaseInsensitive) == 0)
				m_Options.LODControlClampU = sVal.toUInt();
			else if(sArg.compare(QLatin1String("VTFOptions.LODControlClampV"), Qt::CaseInsensitive) == 0)
				m_Options.LODControlClampV = sVal.toUInt();

			else if(sArg.compare(QLatin1String("VTFOptions.CreateInformationResource"), Qt::CaseInsensitive) == 0)
				m_Options.CreateInformationResource = toBool(sVal);
			else if(sArg.compare(QLatin1String("VTFOptions.InformationAuthor"), Qt::CaseInsensitive) == 0)
				m_Options.InformationAuthor = sVal;
			else if(sArg.compare(QLatin1String("VTFOptions.InformationContact"), Qt::CaseInsensitive) == 0)
				m_Options.InformationContact = sVal;
			else if(sArg.compare(QLatin1String("VTFOptions.InformationVersion"), Qt::CaseInsensitive) == 0)
				m_Options.InformationVersion = sVal;
			else if(sArg.compare(QLatin1String("VTFOptions.InformationModification"), Qt::CaseInsensitive) == 0)
				m_Options.InformationModification = sVal;
			else if(sArg.compare(QLatin1String("VTFOptions.InformationDescription"), Qt::CaseInsensitive) == 0)
				m_Options.InformationDescription = sVal;
			else if(sArg.compare(QLatin1String("VTFOptions.InformationComments"), Qt::CaseInsensitive) == 0)
				m_Options.InformationComments = sVal;

			else if(sArg.compare(QLatin1String("RecentFiles.Maximum"), Qt::CaseInsensitive) == 0)
				m_iMaximumRecentFiles = qMin(sVal.toInt(), 16);
			else if(sArg.compare(QLatin1String("RecentFiles.File"), Qt::CaseInsensitive) == 0
				&& QFileInfo::exists(sVal))
				addRecentFile(sVal);
		}

		File.close();

		applyVmtEditorSettings();

		const QRect Geometry = sanitizeWindowGeometry(QRect(Location, Size));

		resize(Geometry.size());
		move(Geometry.topLeft());
		if(bMaximized)
		{
			setWindowState(windowState() | Qt::WindowMaximized);
		}

		m_iSidebarSplit = iSidebarSplit > 0 ? iSidebarSplit : 258;
		m_iSidebarRightSplit = iSidebarRightSplit > 0 ? iSidebarRightSplit : 258;
		applySidebarSizes();

		return true;
	}

	bool MainWindow::writeConfigFile(const QString &sConfigFile) const
	{
		QFile File(sConfigFile);
		if(!File.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
		{
			return false;
		}

		QTextStream Stream(&File);

		const auto boolText = [](bool bValue)
		{
			return bValue ? QStringLiteral("True") : QStringLiteral("False");
		};

		// hidden sidebars report a zero width
		const QList<int> Sizes = m_pSplitter->sizes();
		const int iSidebarSplit = Sizes.value(0, 0) > 0 ? Sizes.value(0) : m_iSidebarSplit;
		const int iSidebarRightSplit = Sizes.value(2, 0) > 0 ? Sizes.value(2) : m_iSidebarRightSplit;

		Stream << "[VTFEdit]\n\n";
		Stream << "VTFEdit.AnimationFrameInterval = " << m_pAnimateTimer->interval() << "\n";
		Stream << "VTFEdit.Mask = " << boolText(m_pMaskAction->isChecked()) << "\n";
		Stream << "VTFEdit.Tile = " << boolText(m_pTileAction->isChecked()) << "\n";
		Stream << "VTFEdit.MipmapFullSize = " << boolText(m_pMipmapFullSizeAction->isChecked()) << "\n";
		Stream << "VTFEdit.AutoCreateVMTFile = " << boolText(m_pAutoCreateVmtFileAction->isChecked()) << "\n";
		Stream << "VTFEdit.SingleInstance = " << boolText(m_pSingleInstanceAction->isChecked()) << "\n";
		Stream << "VTFEdit.LastFileDirectory = " << FileDialogHistory::s_sFileDirectory << "\n";
		Stream << "VTFEdit.LastImageDirectory = " << FileDialogHistory::s_sImageDirectory << "\n";
		Stream << "VmtEditor.FontFamily = " << m_VmtEditorSettings.sFontFamily << "\n";
		Stream << "VmtEditor.FontSize = " << m_VmtEditorSettings.iFontSize << "\n";
		Stream << "VmtEditor.TabSize = " << m_VmtEditorSettings.iTabSize << "\n";
		Stream << "VmtEditor.Theme = " << (m_VmtEditorSettings.eTheme == VmtEditorTheme::Light
			? QStringLiteral("Light") : m_VmtEditorSettings.eTheme == VmtEditorTheme::Dark
			? QStringLiteral("Dark") : QStringLiteral("System")) << "\n";

		const QPoint FrameOffset = geometry().topLeft() - frameGeometry().topLeft();
		const QRect NormalGeometry = normalGeometry();

		Stream << "\n[Forms]\n\n";
		Stream << "Forms.VTFEdit.Location.X = " << NormalGeometry.x() - FrameOffset.x() << "\n";
		Stream << "Forms.VTFEdit.Location.Y = " << NormalGeometry.y() - FrameOffset.y() << "\n";
		Stream << "Forms.VTFEdit.Size.Width = " << NormalGeometry.width() << "\n";
		Stream << "Forms.VTFEdit.Size.Height = " << NormalGeometry.height() << "\n";
		Stream << "Forms.VTFEdit.WindowState = " << (isMaximized() ? "Maximized" : "Normal") << "\n";
		Stream << "Forms.VTFEdit.Sidebar.SplitPosition = " << iSidebarSplit << "\n";
		Stream << "Forms.VTFEdit.SidebarRight.SplitPosition = " << iSidebarRightSplit << "\n";

		Stream << "Forms.BatchConvert.InputFolder = " << m_BatchConvertSettings.sInputFolder << "\n";
		Stream << "Forms.BatchConvert.OutputFolder = " << m_BatchConvertSettings.sOutputFolder << "\n";
		Stream << "Forms.BatchConvert.ToVTF = " << boolText(m_BatchConvertSettings.bToVTF) << "\n";
		Stream << "Forms.BatchConvert.ToVTFFilter = " << m_BatchConvertSettings.sToVTFFilter << "\n";
		Stream << "Forms.BatchConvert.FromVTFFormat = " << m_BatchConvertSettings.sFromVTFFormat << "\n";
		Stream << "Forms.BatchConvert.FromVTFFilter = " << m_BatchConvertSettings.sFromVTFFilter << "\n";
		Stream << "Forms.BatchConvert.Recurse = " << boolText(m_BatchConvertSettings.bRecurse) << "\n";
		Stream << "Forms.BatchConvert.CreateVMTFiles = " << boolText(m_BatchConvertSettings.bCreateVMTFiles) << "\n";

		Stream << "\n[VTF Options]\n\n";
		Stream << "VTFOptions.NormalFormat = " << static_cast<int>(m_Options.NormalFormat) << "\n";
		Stream << "VTFOptions.AlphaFormat = " << static_cast<int>(m_Options.AlphaFormat) << "\n";
		Stream << "VTFOptions.TextureType = " << static_cast<int>(m_Options.TextureType) << "\n";
		Stream << "VTFOptions.FlagClampS = " << boolText(m_Options.FlagClampS != vlFalse) << "\n";
		Stream << "VTFOptions.FlagClampT = " << boolText(m_Options.FlagClampT != vlFalse) << "\n";
		Stream << "VTFOptions.FlagNoLOD = " << boolText(m_Options.FlagNoLOD != vlFalse) << "\n";
		Stream << "VTFOptions.FlagPointSample = " << boolText(m_Options.FlagPointSample != vlFalse) << "\n";
		Stream << "VTFOptions.StripAlpha = " << boolText(m_Options.StripAlpha != vlFalse) << "\n";
		Stream << "VTFOptions.sRGB = " << boolText(m_Options.sRGB != vlFalse) << "\n";
		Stream << "VTFOptions.DistanceAlpha = " << boolText(m_Options.DistanceAlpha != vlFalse) << "\n";
		Stream << "VTFOptions.DistanceAlphaSpread = " << m_Options.DistanceAlphaSpread << "\n";
		Stream << "VTFOptions.DistanceAlphaReduce = " << m_Options.DistanceAlphaReduce << "\n";
		Stream << "VTFOptions.DistanceAlphaThreshold = " << m_Options.DistanceAlphaThreshold << "\n";

		Stream << "VTFOptions.Resize = " << boolText(m_Options.ResizeImage != vlFalse) << "\n";
		Stream << "VTFOptions.ResizeMethod = " << static_cast<int>(m_Options.ResizeMethod) << "\n";
		Stream << "VTFOptions.ResizeFilter = " << static_cast<int>(m_Options.ResizeFilter) << "\n";
		Stream << "VTFOptions.ResizeClamp = " << boolText(m_Options.ResizeClamp != vlFalse) << "\n";
		Stream << "VTFOptions.ResizeClampWidth = " << m_Options.ResizeClampWidth << "\n";
		Stream << "VTFOptions.ResizeClampHeight = " << m_Options.ResizeClampHeight << "\n";

		Stream << "VTFOptions.GenerateMipmaps = " << boolText(m_Options.GenerateMipmaps != vlFalse) << "\n";
		Stream << "VTFOptions.MipmapFilter = " << static_cast<int>(m_Options.MipmapFilter) << "\n";

		Stream << "VTFOptions.Version = " << m_Options.Version << "\n";
		Stream << "VTFOptions.AuxCompressionLevel = " << m_Options.AuxCompressionLevel << "\n";
		Stream << "VTFOptions.AuxCompressionMethod = " << m_Options.AuxCompressionMethod << "\n";

		Stream << "VTFOptions.ComputeReflectivity = " << boolText(m_Options.ComputeReflectivity != vlFalse) << "\n";
		Stream << "VTFOptions.GenerateThumbnail = " << boolText(m_Options.GenerateThumbnail != vlFalse) << "\n";
		Stream << "VTFOptions.GenerateSphereMap = " << boolText(m_Options.GenerateSphereMap != vlFalse) << "\n";

		Stream << "VTFOptions.CorrectGamma = " << boolText(m_Options.CorrectGamma != vlFalse) << "\n";
		Stream << "VTFOptions.GammaCorrection = " << m_Options.GammaCorrection << "\n";

		Stream << "VTFOptions.LuminanceWeightR = " << m_Options.LuminanceWeightR << "\n";
		Stream << "VTFOptions.LuminanceWeightG = " << m_Options.LuminanceWeightG << "\n";
		Stream << "VTFOptions.LuminanceWeightB = " << m_Options.LuminanceWeightB << "\n";

		Stream << "VTFOptions.CreateLODControlResource = " << boolText(m_Options.CreateLODControlResource != vlFalse) << "\n";
		Stream << "VTFOptions.LODControlClampU = " << m_Options.LODControlClampU << "\n";
		Stream << "VTFOptions.LODControlClampV = " << m_Options.LODControlClampV << "\n";

		Stream << "VTFOptions.CreateInformationResource = " << boolText(m_Options.CreateInformationResource != vlFalse) << "\n";
		Stream << "VTFOptions.InformationAuthor = " << m_Options.InformationAuthor << "\n";
		Stream << "VTFOptions.InformationContact = " << m_Options.InformationContact << "\n";
		Stream << "VTFOptions.InformationVersion = " << m_Options.InformationVersion << "\n";
		Stream << "VTFOptions.InformationModification = " << m_Options.InformationModification << "\n";
		Stream << "VTFOptions.InformationDescription = " << m_Options.InformationDescription << "\n";
		Stream << "VTFOptions.InformationComments = " << m_Options.InformationComments << "\n";

		Stream << "\n[Recent Files]\n\n";
		Stream << "RecentFiles.Maximum = " << m_iMaximumRecentFiles << "\n";

		// write oldest first so reading them back rebuilds the same order
		for(int i = m_RecentFiles.count() - 1; i >= 0; i--)
		{
			Stream << "RecentFiles.File = " << m_RecentFiles.at(i) << "\n";
		}

		File.close();

		return true;
	}

	//
	// Window events.
	//

	void MainWindow::closeEvent(QCloseEvent *pEvent)
	{
		// give every unsaved document a chance to be written
		while(!m_Documents.empty())
		{
			if(!closeDocument(static_cast<int>(m_Documents.size()) - 1))
			{
				pEvent->ignore();
				return;
			}
		}

		writeConfigFile(configFilePath());

		QMainWindow::closeEvent(pEvent);
	}

	void MainWindow::openCommandLineFiles(const QStringList &sFilePaths)
	{
		QStringList sExisting;
		for(const QString &sFilePath : sFilePaths)
		{
			if(QFileInfo::exists(sFilePath))
			{
				sExisting.append(sFilePath);
			}
		}

		if(!sExisting.isEmpty())
		{
			handleDroppedFiles(sExisting);
		}
	}

	void MainWindow::activateWithFiles(const QStringList &sFilePaths)
	{
		setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);

		if(!isVisible())
		{
			show();
		}

		raise();
		activateWindow();

		if(QWindow *pHandle = windowHandle())
		{
			pHandle->requestActivate();
		}

		openCommandLineFiles(sFilePaths);
	}

	void MainWindow::dragEnterEvent(QDragEnterEvent *pEvent)
	{
		if(!pEvent->mimeData()->hasUrls())
		{
			return;
		}

		const QString sFirst = pEvent->mimeData()->urls().first().toLocalFile();
		if(!sFirst.isEmpty() && IsSupportedFileName(sFirst) && !QFileInfo(sFirst).isDir())
		{
			pEvent->acceptProposedAction();
		}
	}

	void MainWindow::dropEvent(QDropEvent *pEvent)
	{
		QStringList sFiles;
		for(const QUrl &Url : pEvent->mimeData()->urls())
		{
			const QString sFile = Url.toLocalFile();
			if(!sFile.isEmpty() && IsSupportedFileName(sFile))
			{
				sFiles.append(sFile);
			}
		}

		if(!sFiles.isEmpty())
		{
			pEvent->acceptProposedAction();
			handleDroppedFiles(sFiles);
		}
	}

	void MainWindow::handleDroppedFiles(const QStringList &sFiles)
	{
		// TODO maybe handle the case of dropping both vtfs/vmts and other files together

		if(sFiles.first().endsWith(QLatin1String(".vtf"), Qt::CaseInsensitive)
			|| IsKeyValuesFileName(sFiles.first()))
		{
			// each dropped material or texture gets its own tab
			for(const QString &sFile : sFiles)
			{
				if(sFile.endsWith(QLatin1String(".vtf"), Qt::CaseInsensitive)
					|| IsKeyValuesFileName(sFile))
				{
					open(sFile, false);
				}
			}
		}
		else
		{
			// Drop order is not defined by the shell
			// so sort the frames and faces by name
			QStringList sSorted = sFiles;
			std::sort(sSorted.begin(), sSorted.end(), [](const QString &sLeft, const QString &sRight)
			{
				return sLeft.compare(sRight, Qt::CaseInsensitive) < 0;
			});

			import(sSorted);
		}
	}
}
