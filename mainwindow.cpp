#include <QtGui>
#include <QLibrary>
#include <QtSql>
#include <QtPlugin>

#include<Windows.h>

//#include <QtWebKit>
//#include <QWebView>
#include "mainwindow.h"
#include "qextserialport.h"
#include <qextserialenumerator.h>

#include <qvector.h>
#include <qstringlist.h>
#include <TableModel.h>
#include <QModelIndex>
#include <setStdDialog.h>
#include <FillSampleName.h>

#include <highgui.h>

QStringList MainWindow::recentFiles;

Q_IMPORT_PLUGIN(qsqlodbc)
Q_IMPORT_PLUGIN(qsqlmysql)
//Q_IMPORT_PLUGIN(qsqlibase)

MainWindow::MainWindow()
{
	//initialization of the private members
	this->hasStandardODValue = false;
	this->hasStandardConc = false;
	this->ableToGenerateCurve = false;
	this->standard_number = 0;
	
	this->ODLayout = 0;
	this->ODLayout_1 = 0;
	this->ODTitle = 0;
	this->plateLayout = 0;
	this->plateLayout_1 = 0;
	this->plateTitle = 0;
	this->curveWindow = 0;
	this->resultsLayout = 0;
	this->resultModel = 0;
	this->initView = 0;
	this->lateView = 0;
	this->centralWidget = 0;
	this->vBoxLayout = 0;
	this->activeWidgetLabel = 0;
	this->regressionMethod = MyPlot::linearFit;

	init();
	createActions();
	createMenus();
	createContextMenu();
	//QMessageBox::warning(this, tr("Select Cells As Standards"),
			//tr("after context"));
	createToolBars();
	createStatusBar();
	
	readSettings();
	printer = 0;
	printDialog = 0;
	setWindowIcon(QIcon("images/icon.png"));
	setAttribute(Qt::WA_DeleteOnClose);
	this->setCurrentFile("");
	connect(this, SIGNAL(standardConcentrationSetSignal()), this, SLOT(setLateViewEnabled()));
	connect(this, SIGNAL(standardODValueSetSignal()), this, SLOT(setLateViewEnabled()));
	connect(this->ODModel, SIGNAL(ODValueChanged(QModelIndex,double)), this->plateModel, SLOT(updateODValuesFromODModel(QModelIndex,double)));
	connect(this->plateModel, SIGNAL(ODValueReadFromFile(double*)),this->ODModel,SLOT(updateODValuesFromPlateModel(double*)));
	connect(this->plateModel, SIGNAL(somethingCopied()), this, SLOT(setThisWindowModified()));
	connect(this->plateModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(setThisWindowModified()));
		
	this->update();
}

//event handling functions
void MainWindow::closeEvent(QCloseEvent *event)
{
	if (okToContinue()) {
		writeSettings();
		event->accept();
	} else {
		event->ignore();
	}
}
void MainWindow::paintEvent(QPaintEvent *event)
{
	//event->
	//this->layoutResize(plateLayout);
	//this->layoutResize(plateLayout_1);
	this->layoutResize(this->resultsLayout);
}

//private initization functions
void MainWindow::init() {
	//initview initialization
	ODModel = new ODTableModel(8,12,this);
	ODLayout = new ODView(ODModel,this);
	ODLayout_1 = new ODView(ODModel,this);
	plateModel = new LayoutTableModel(8,12,this);
	plateLayout = new LayoutView(plateModel,this);
	plateLayout_1 = new LayoutView(plateModel,this);

	//get initial values from the models
	//QMessageBox::warning(this, tr("Select Cells As Standards"),
			//tr("after context"));
	//ODLayout->getODValue(data);
	//plateLayout->getSampleName(&sampleNames);

	//using the same selection model
	plateLayout->setSelectionModel(ODLayout->selectionModel());
		
	plateTitleLayout = new QGridLayout(this);
	vBoxLayout = new QVBoxLayout;
	ODTitle = new QLabel("Plate Value");
	plateTitle = new QLabel(tr("Plate Layout"));
	QModelIndex assayNameIndex = plateModel->index(1,1,QModelIndex());
	assayNameLabel = new QLabel(QString("Assay: %1").arg(plateModel->data(assayNameIndex,Qt::UserRole+5).toString()));
	QModelIndex catalogNumberIndex = plateModel->index(2,1,QModelIndex());
	assayCatalogLabel = new QLabel(QString("MaxSignal Catalog No.: %1").arg(plateModel->data(catalogNumberIndex,Qt::UserRole+5).toString()));
	QModelIndex TechNameIndex = plateModel->index(4,1,QModelIndex());
	assayTechNameLabel = new QLabel(QString("Performed By: %1").arg(plateModel->data(TechNameIndex,Qt::UserRole+5).toString()));
	plateTitleLayout->addWidget(plateTitle,0,0,1,1);
	plateTitleLayout->addWidget(assayNameLabel,0,2,1,1);
	plateTitleLayout->addWidget(assayCatalogLabel,0,3,1,1);
	plateTitleLayout->addWidget(assayTechNameLabel,0,4,1,1);
	
	vBoxLayout->addLayout(plateTitleLayout);
	vBoxLayout->addWidget(plateLayout,9);
	vBoxLayout->addWidget(ODTitle);
	vBoxLayout->addWidget(ODLayout,9);
	
	initView = new QWidget(this);
	initView->setAccessibleName(tr("Initial View"));
	initView->setLayout(vBoxLayout);
	//initView->setMinimumSize(800,800);

	//late view initialization
	lateView = new QTabWidget(this);
	lateView->addTab(plateLayout_1, tr("Plate Layout"));
	lateView->addTab(ODLayout_1, tr("Plate Value"));
	curveWindow = new plotView();
	lateView->addTab(curveWindow,tr("Standard Curve"));
	resultsLayout = new QTableView(this);
	lateView->addTab(resultsLayout, tr("Test Summary"));
	lateView->setTabPosition(QTabWidget::North);
	lateView->setTabShape(QTabWidget::Triangular);

	ODLayout->setStatusTip("Optical Density");
	ODLayout_1->setStatusTip("Optical Density");
	plateLayout->setStatusTip("Plate Layout");
	plateLayout_1->setStatusTip("Plate Layout");
	curveWindow->setStatusTip("Standard Curve");
	resultsLayout->setStatusTip("Test results");

	centralWidget = new QStackedWidget(this);
	centralWidget->addWidget(initView);
	centralWidget->addWidget(lateView);
	centralWidget->setCurrentWidget(initView);
	this->setCentralWidget(centralWidget);
	this->setWindowIcon(QIcon("icon1.ico"));
	this->setMinimumSize(1200,700);
}

void MainWindow::layoutResize(QTableView *view){
	if (!view) return;
	
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
	if (!view->model()) return;
	for (int i=0; i<view->model()->columnCount(); ++i)
		view->setColumnWidth(i,(view->width()-view->verticalHeader()->width())/view->model()->columnCount());
	//for (int i=0; i<view->model()->rowCount(); ++i)
		//view->setRowHeight(i,(view->height()-view->horizontalHeader()->height())/view->model()->rowCount());
	view->horizontalHeader()->setStretchLastSection(true);
	//view->verticalHeader()->setStretchLastSection(true);
}

void MainWindow::createActions()
{
	//recent file action
	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActions[i] = new QAction(this);
		recentFileActions[i]->setVisible(false);
		connect(recentFileActions[i], SIGNAL(triggered()),
			this, SLOT(openRecentFile()));
	}
	
	/* ************************************************ */
	newAssayAction = new QAction(tr("New"), this);
	newAssayAction->setIcon(QIcon("images/new.png"));
	newAssayAction->setShortcut(tr(""));
	newAssayAction->setStatusTip(tr("Create A New Assay"));
	connect(newAssayAction, SIGNAL(triggered()), this, SLOT(newAssay()));
	/* ************************************************** */
	openAssayAction = new QAction(tr("Open"), this);
	openAssayAction->setIcon(QIcon("images/open.png"));
	openAssayAction->setShortcut(tr(""));
	openAssayAction->setStatusTip(tr("Open"));
	connect(openAssayAction, SIGNAL(triggered()), this, SLOT(openAssay()));
	/* ********************************************************** */
	saveAssayAction = new QAction(tr("Save"), this);
	saveAssayAction->setIcon(QIcon("images/save.png"));
	saveAssayAction->setShortcut(tr(""));
	saveAssayAction->setStatusTip(tr("Save the Current Assay"));
	connect(saveAssayAction, SIGNAL(triggered()), this, SLOT(saveAssay()));	
	/* ************************************************************ */
	saveAssayAsAction = new QAction(tr("Save As"), this);
	saveAssayAsAction->setIcon(QIcon(""));
	saveAssayAsAction->setShortcut(tr(""));
	saveAssayAsAction->setStatusTip(tr("Save the Current Assay As"));
	connect(saveAssayAsAction, SIGNAL(triggered()), this, SLOT(saveAssayAs()));
	//importOD action, File_Menu
	importODDataAction = new QAction(tr("Read OD From File"), this);
	importODDataAction->setIcon(QIcon(""));
	importODDataAction->setShortcut(tr(""));
	importODDataAction->setStatusTip(tr("Read Optical Density Values from File"));
	connect(importODDataAction,SIGNAL(triggered()),this, SLOT(importOD()));
	//read plate action, file menu
	readPlateAction = new QAction(tr("Read OD From Plate Reader"),this);
	readPlateAction->setIcon(QIcon(""));
	readPlateAction->setShortcut(tr(""));
	readPlateAction->setStatusTip(tr("Read Optical Density Values from Plate Reader"));
	connect(readPlateAction,SIGNAL(triggered()),this,SLOT(readPlate()));

	//Reader setup action , file menu
	readerSetupAction = new QAction(tr("Configure Plate Reader"),this);
	readerSetupAction->setStatusTip(tr("Set up the reader"));
	connect(readerSetupAction,SIGNAL(triggered()),this,SLOT(readerSetup()));
	//print,printPreview, and printerSetup action, File_Menu
	printAction = new QAction(tr("&Print"), this);
	printAction->setShortcut(tr("Ctrl+P"));
	printAction->setStatusTip(tr("Print this window"));
	connect(printAction, SIGNAL(triggered()), this, SLOT(print()));
	/* ************************************************************ */
	printPreviewAction = new QAction(tr("&Print Preview"), this);
	printPreviewAction->setShortcut(tr(""));
	printPreviewAction->setStatusTip(tr("Print Preview"));
	connect(printPreviewAction, SIGNAL(triggered()), this, SLOT(printPreview()));
	/* *********************************************************** */
	printerSetupAction = new QAction(tr("&Printer Set Up"), this);
	printerSetupAction->setShortcut(tr(""));
	printerSetupAction->setStatusTip(tr("Configure the Printer"));
	connect(printerSetupAction, SIGNAL(triggered()), this, SLOT(printerSetup()));
	
	
	//close action, File_Menu
	closeAction = new QAction(tr("&Close"), this);
	closeAction->setShortcut(tr(""));
	closeAction->setStatusTip(tr("Close the window"));
	connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
	//exit action, File_Menu
	exitAction = new QAction(tr("E&xit"), this);
	exitAction->setShortcut(tr(""));
	exitAction->setStatusTip(tr("Exit the Program"));
	connect(exitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
		
	//undo action, Edit_Menu
	undoAction = new QAction(tr("undo"), this);
	undoAction->setIcon(QIcon(""));
	undoAction->setShortcut(tr(""));
	undoAction->setStatusTip(tr("Undo"));
	connect(undoAction, SIGNAL(triggered()),this, SLOT(undo()));
	//cut action, Edit_Menu
	cutAction = new QAction(tr("Cut"), this);
	cutAction->setIcon(QIcon("images/cut.png"));
	cutAction->setShortcut(tr(""));
	cutAction->setStatusTip(tr("Cut"));
	connect(cutAction, SIGNAL(triggered()),this, SLOT(cut()));
	//copy action edit menu
	copyAction = new QAction(tr("Copy"),this);
	copyAction->setIcon(QIcon("images/copy.png"));
	copyAction->setStatusTip(tr("Copy"));
	connect(copyAction,SIGNAL(triggered()),this,SLOT(copy()));
	//paste action, Edit-Menu
	pasteAction = new QAction(tr("&Paste"), this);
	pasteAction->setIcon(QIcon("images/paste.png"));
	pasteAction->setShortcut(tr(""));
	pasteAction->setStatusTip(tr("Paste"));
	connect(pasteAction, SIGNAL(triggered()),this, SLOT(paste()));
	//delete action, edit menu
	deleteAction = new QAction(tr("Delete"),this);
	deleteAction->setIcon(QIcon(""));
	pasteAction->setShortcut(tr(""));
	pasteAction->setStatusTip(tr("Delete"));
	connect(deleteAction, SIGNAL(triggered()),this, SLOT(Delete()));
	//fillSampleNameAction, Edit Menu
	//paste action, Edit-Menu
	fillSampleNameAction = new QAction(tr("&Fill"), this);
	connect(fillSampleNameAction, SIGNAL(triggered()),this, SLOT(fillSampleName()));


	//new method action, Method_Menu
	newMethodAction = new QAction(tr("New Method"), this);
	newMethodAction->setIcon(QIcon(""));
	newMethodAction->setShortcut(tr(""));
	newMethodAction->setStatusTip(tr("Create A New Method"));
	connect(newMethodAction, SIGNAL(triggered()), this, SLOT(newMethod()));
	//open method action, methode_Menu
	openMethodAction = new QAction(tr("Open Method"), this);
	openMethodAction->setIcon(QIcon(""));
	openMethodAction->setShortcut(tr(""));
	openMethodAction->setStatusTip(tr("Open An Existing Method"));
	connect(openMethodAction, SIGNAL(triggered()), this, SLOT(openMethod()));
	//save method action, method_Menu
	saveMethodAction = new QAction(tr("Save Method"), this);
	saveMethodAction->setIcon(QIcon(""));
	saveMethodAction->setShortcut(tr(""));
	saveMethodAction->setStatusTip(tr("Save the Current Method"));
	connect(saveMethodAction, SIGNAL(triggered()), this, SLOT(saveMethod()));
    //save method as action, method_Menu
	saveMethodAsAction = new QAction(tr("Save Method As"), this);
	saveMethodAsAction->setIcon(QIcon(""));
	saveMethodAsAction->setShortcut(tr(""));
	saveMethodAsAction->setStatusTip(tr("Save the Current Method As"));
	connect(saveMethodAsAction, SIGNAL(triggered()), this, SLOT(saveMethodAs()));
	//setStandard action, method menu
	setStandardAction = new QAction(tr("Set/Reset Standards"), this);
	setStandardAction->setIcon(QIcon(""));
	setStandardAction->setShortcut(tr(""));
	setStandardAction->setStatusTip(tr("Set/Reset Standards"));
	connect(setStandardAction, SIGNAL(triggered()),this, SLOT(setStandard()));
	//setDilutionFactorAction, method menu
	setDilutionFactorAction = new QAction(tr("Set Dilution Factors"), this);
	setDilutionFactorAction->setIcon(QIcon(""));
	setDilutionFactorAction->setShortcut(tr(""));
	setDilutionFactorAction->setStatusTip(tr("Set Dilution Factors for the Samples"));
	connect(setDilutionFactorAction, SIGNAL(triggered()),this, SLOT(setDilutionFactor()));
	//set sample type, method menu
	classifySampleAction = new QAction(tr("Set Sample Type"), this);
	classifySampleAction->setIcon(QIcon(""));
	classifySampleAction->setShortcut(tr(""));
	classifySampleAction->setStatusTip(tr("Set Dilution Factors for the Samples"));
	connect(classifySampleAction, SIGNAL(triggered()),this, SLOT(setSampleTypes()));
	//set cut off values, method menu
	setCutOffValuesAction = new QAction(tr("Set Cut Off Values"),this);
	setCutOffValuesAction->setIcon(QIcon(""));
	setCutOffValuesAction->setShortcut(tr(""));
	setCutOffValuesAction->setStatusTip(tr("Set Cut Off Values for the Samples"));
	connect(setCutOffValuesAction, SIGNAL(triggered()),this, SLOT(setCutOffValues()));

	algorithmGroup = new QActionGroup(this);
	algorithmGroup->setExclusive(true);
	algorithmGroup->setDisabled(true);
	//linearAction action, Results_Menu
	linearAction = new QAction(tr("&Linear Regression"),algorithmGroup);
	//linearAction->setIcon(QIcon("images/linearRegression.png"));
	linearAction->setShortcut(tr(""));
	linearAction->setStatusTip(tr("Linear Regression on the Data"));
	//linearAction->setDisabled(true);
	linearAction->setCheckable(true);
	linearAction->setChecked(false);
	connect(linearAction, SIGNAL(toggled(bool)),this, SLOT(linearFit()));
	//logitLogAction action, Results_Menu
	logitLogAction = new QAction(tr("&Logit/Log"),algorithmGroup);
	//logitLogAction->setIcon(QIcon("images/logitLogRegression.png"));
	logitLogAction->setShortcut(tr(""));
	logitLogAction->setStatusTip(tr("Logit/Log Regression on the Data"));
	//logitLogAction->setDisabled(true);
	logitLogAction->setCheckable(true);
	logitLogAction->setChecked(false);
	connect(logitLogAction, SIGNAL(toggled(bool)),this, SLOT(logitLogFit()));
	//cubicSplineAction action, Results_Menu
	cubicSplineAction = new QAction(tr("&Cubic Spline"),algorithmGroup);
	//cubicSplineAction->setIcon(QIcon("images/cubicSplineRegression.png"));
	cubicSplineAction->setShortcut(tr(""));
	cubicSplineAction->setStatusTip(tr("Cubic Spline Regression on the Data"));
	//cubicSplineAction->setDisabled(true);
	cubicSplineAction->setCheckable(true);
	cubicSplineAction->setChecked(false);
	connect(cubicSplineAction, SIGNAL(toggled(bool)),this, SLOT(cubicSplineFit()));
	//fourPLAction action, Results_Menu
	fourPLAction = new QAction(tr("4 PL"),algorithmGroup);
	//fourPLAction->setIcon(QIcon("images/4PLRegression.png"));
	fourPLAction->setShortcut(tr(""));
	fourPLAction->setStatusTip(tr("4PL Regression on the Data"));
	//fourPLAction->setDisabled(true);
	fourPLAction->setCheckable(true);
	fourPLAction->setChecked(false);
	connect(fourPLAction, SIGNAL(toggled(bool)),this, SLOT(fourPLFit()));

	//initViewAction, window menu
	initViewAction = new QAction(tr("Plate Layout"), this);
	initViewAction->setIcon(QIcon(""));
	initViewAction->setShortcut(tr(""));
	initViewAction->setStatusTip(tr("Show OD and Sample Layout Only"));
	connect(initViewAction, SIGNAL(triggered()),this, SLOT(InitView()));
	//lateViewAction, window menu
	lateViewAction = new QAction(tr("Method Layout"), this);
	lateViewAction->setIcon(QIcon(""));
	lateViewAction->setShortcut(tr(""));
	lateViewAction->setStatusTip(tr("Method Layout"));
	lateViewAction->setDisabled(true);
	connect(lateViewAction, SIGNAL(triggered()),this, SLOT(LateView()));


	//index action, Help_Menu
	indexAction = new QAction(tr("&Index"), this);
	indexAction->setShortcut(tr(""));
	indexAction->setStatusTip(tr("Browse Through The Index"));
	connect(indexAction, SIGNAL(triggered()), this, SLOT(indexHelp()));
	//Using Help action, Help_Menu
	usingHelpAction = new QAction(tr("How to Use the Help"), this);
	usingHelpAction->setShortcut(tr(""));
	usingHelpAction->setStatusTip(tr("How to use the help"));
	connect(usingHelpAction, SIGNAL(triggered()), this, SLOT(usingHelp()));
	//AboutBiooAction , Help_Menu
	aboutBiooAction = new QAction(tr("About This Software"), this);
	aboutBiooAction->setShortcut(tr(""));
	aboutBiooAction->setStatusTip(tr("About this software"));
	connect(aboutBiooAction, SIGNAL(triggered()), this, SLOT(aboutBioo()));
}
void MainWindow::createMenus()
{
	//file menu
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAssayAction);
	fileMenu->addAction(openAssayAction);
	fileMenu->addAction(saveAssayAction);
	fileMenu->addAction(saveAssayAsAction);
	readSubMenu = fileMenu->addMenu(tr("&Read OD"));
	readSubMenu->addAction(importODDataAction);
	readSubMenu->addAction(readPlateAction);
	fileMenu->addAction(this->readerSetupAction);
	fileMenu->addAction(printerSetupAction);
	//fileMenu->addAction(printPreviewAction);
	fileMenu->addAction(printAction);
	separatorAction = fileMenu->addSeparator();
	for (int i = 0; i < MaxRecentFiles; ++i)
		fileMenu->addAction(recentFileActions[i]);
	fileMenu->addSeparator();

	fileMenu->addAction(closeAction);
	fileMenu->addAction(exitAction);

	//edit menu
	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(undoAction);
	editMenu->addAction(cutAction);
	editMenu->addAction(copyAction);
	editMenu->addAction(pasteAction);
	editMenu->addAction(deleteAction);
	editMenu->addAction(this->fillSampleNameAction);

	//method menu
	methodMenu = menuBar()->addMenu(tr("&Method"));
	methodMenu->addAction(newMethodAction);
	methodMenu->addAction(openMethodAction);
	methodMenu->addAction(saveMethodAction);
	//methodMenu->addAction(saveMethodAsAction);
	methodMenu->addSeparator();
	methodMenu->addAction(setStandardAction);
	methodMenu->addAction(setDilutionFactorAction);
    methodMenu->addAction(classifySampleAction);
	methodMenu->addAction(setCutOffValuesAction);

	//Results menu
	resultsMenu = menuBar()->addMenu(tr("&Results"));
	resultsMenu->addAction(linearAction);
	resultsMenu->addAction(logitLogAction);
	resultsMenu->addAction(cubicSplineAction);
	resultsMenu->addAction(fourPLAction);

	//windows menu
	windowMenu = menuBar()->addMenu(tr("Windows"));
	windowMenu->addAction(initViewAction);
	windowMenu->addAction(lateViewAction);

	//help Menu
	helpMenu = menuBar()->addMenu(tr("Help"));
	helpMenu->addAction(indexAction);
	helpMenu->addAction(usingHelpAction);
	helpMenu->addAction(aboutBiooAction);
}
void MainWindow::createContextMenu()
{
	this->initView->addAction(cutAction);
	this->initView->addAction(copyAction);
	this->initView->addAction(pasteAction);
	this->initView->addAction(deleteAction);
	initView->setContextMenuPolicy(Qt::ActionsContextMenu);

	if (this->lateView) {
		lateView->addAction(cutAction);
		lateView->addAction(copyAction);
		lateView->addAction(pasteAction);
		lateView->addAction(deleteAction);
		lateView->setContextMenuPolicy(Qt::ActionsContextMenu);
	}
}

void MainWindow::createToolBars()
{
	fileToolBar = addToolBar(tr("File"));
	//fileToolBar->addAction(newMethodAction);
	fileToolBar->addAction(newAssayAction);
	//fileToolBar->addAction(openMethodAction);
	fileToolBar->addAction(openAssayAction);
	//fileToolBar->addAction(saveMethodAction);
	fileToolBar->addAction(saveAssayAction);
	//fileToolBar->addAction(saveMethodAsAction);
	//fileToolBar->addAction(saveAssayAsAction);
	//fileToolBar->addAction(importODDataAction);
	//fileToolBar->addAction(printAction);
	//fileMenu->addAction(readerSetupAction);	

	//edit tool bar
	editToolBar = addToolBar(tr("Edit"));
	//editToolBar->addAction(undoAction);
	editToolBar->addAction(cutAction);
	editToolBar->addAction(copyAction);
	editToolBar->addAction(pasteAction);
	//editToolBar->addAction(setStandardAction);
	//editToolBar->addAction(setDilutionFactorAction);
    //editToolBar->addAction(classifySampleAction);

	//results tool bar
	//resultsToolBar = addToolBar(tr("Results"));
	//resultsToolBar->addAction(linearAction);
	//resultsToolBar->addAction(logitLogAction);
	//resultsToolBar->addAction(cubicSplineAction);
	//resultsToolBar->addAction(fourPLAction);

	//help tool bar
	//helpToolBar = addToolBar(tr("Help"));
	//helpToolBar->addAction(aboutBiooAction);
}

void MainWindow::createStatusBar()
{
	activeWidgetLabel = new QLabel("");
	activeWidgetLabel->setAlignment(Qt::AlignHCenter);
	statusBar()->addWidget(activeWidgetLabel);
	//this->updateStatusBar();
}

void MainWindow::writeSettings()
{
	QSettings settings("Software Inc.", "ELISA Bioo");
	settings.setValue("geometry", geometry());
	settings.setValue("recentFiles", recentFiles);
	//settings.setValue("showGrid", showGridAction->isChecked());
	//settings.setValue("autoRecalc", autoRecalcAction->isChecked());
}

void MainWindow::readSettings()
{
	QSettings settings("Software Inc.", "ELISA Bioo");
	QRect rect = settings.value("geometry",
		QRect(200, 200, 400, 400)).toRect();
	move(rect.topLeft());
	resize(rect.size());
	recentFiles = settings.value("recentFiles").toStringList();
	
	foreach (QWidget *win, QApplication::topLevelWidgets()) {
		if (MainWindow *mainWin = qobject_cast<MainWindow *>(win))
			mainWin->updateRecentFileActions();
	}
}

bool MainWindow::okToContinue()
{
	if (isWindowModified()) {
		int r = QMessageBox::warning(this, tr("ELISA"),
			tr("The document has been modified.\n"
			"Do you want to save your changes?"),
			QMessageBox::Yes | QMessageBox::Default,
			QMessageBox::No,
			QMessageBox::Cancel | QMessageBox::Escape);
		if (r == QMessageBox::Yes) {
			return saveAssay();
		} else if (r == QMessageBox::Cancel) {
			return false;
		}
	}
	return true;
}

bool MainWindow::isAbleToGenerateCurve(){
	if ( !this->hasStandardConc) {
		QMessageBox::warning(this,tr("ELISA Bioo"),tr("Standards' concentrations are not set."));
		return false;
	}
	if (!this->hasStandardODValue) {
		QMessageBox::warning(this,tr("ELISA Bioo"),tr("Standards' optical density values are not set."));
		return false;
	}

	if (this->standardData[0].count()<2 || this->standardData[1].count()<2 ||  this->standardData[0].count()!= this->standardData[1].count() ) {
		QMessageBox::warning(this,tr("ELISA Bioo"),tr("Too few standards or some standards' do not have corresponding concentrations."));
		return false;
	}

	QVector<double> temp_conc,temp_OD;
	for (int i=0; i<standardData[1].count(); ++i) {
		temp_conc.append(standardData[0].at(i));
		temp_OD.append(standardData[1].at(i));
	}
	qSort(temp_conc.begin(),temp_conc.end());//increasing order of concentrations
	
	//handle situation when there is no negative standards
	if (temp_conc.at(0) != 0) {
		QMessageBox::warning(this,tr("ELISA Bioo"),tr("There should exist a negative control standard"));
		return false;
	}

	//calculated how many standards in the ELISA assay
	int std_count = 1;
	QVector<int> processed_index;
	processed_index.append(0);
	for (int i=1; i<temp_conc.size(); ++i) {
		if (temp_conc.at(i) > temp_conc.at(i-1)){
			std_count++;
			processed_index.append(i);
		}
	}

	if (std_count<2) {
		QMessageBox::warning(this,tr("ELISA Bioo"),tr("Too few standards to generate a standard curve."));
		return false;//fewer than 2 standards
	}

	QVector<double> processedElisaStdData[2];
	for (int i=0; i<std_count; ++i) {
		int  identical_std_count = 0;
		double sum_OD = 0;
		for (int j = 0 ; j <this->standardData[0].count(); ++j ) {
			if (this->standardData[0].at(j) == temp_conc.at(processed_index.at(i))){
				sum_OD += standardData[1].at(j);
			    identical_std_count++;
			}
		}
		processedElisaStdData[0].append(temp_conc.at(processed_index.at(i)));
		processedElisaStdData[1].append(sum_OD/(double)identical_std_count);
	}

	if (processedElisaStdData[1].at(0) == 0) {
		QMessageBox::warning(this,tr("ELISA Bioo"),tr("The negative control standards have a zero OD value."));
		return false;
	}
	
	ableToGenerateCurve = true;
	return true;
}

QString MainWindow::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

//action slot handlers for File menu
void MainWindow::newAssay()
{
	MainWindow *mainWin = new MainWindow;
	mainWin->show();
}

bool MainWindow::openAssay()
{
	//if (!okToContinue()) return false;

	QString fileName = QFileDialog::getOpenFileName(this,
			tr("Open ELISA Assay"), ".",
			"ELISA Assay File (*.ELISA)\n");
	if (fileName.isEmpty()) return false;
			
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(this, tr("ELISA Assay File"),
			tr("Cannot read file %1:\n%2.")
			.arg(file.fileName())
			.arg(file.errorString()));
		return false;
	}
	else {
		return this->loadAssayFile(fileName);
	}
}

bool MainWindow::loadAssayFile(const QString &fileName)
{
	if (!this->plateModel->readFile(fileName)) {
		statusBar()->showMessage(tr("Loading canceled"), 2000);
		return false;
	}
	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File loaded"), 2000);
	this->resize(this->size().width()+1,this->size().height()+1);
	this->resize(this->size().width()-1,this->size().height()-1);
	return true;
}

bool MainWindow::saveAssay()
{
	if (curFile.isEmpty()) {
		return saveAssayAs();
	} else {
		return saveAssayFile(curFile);
	}
}

bool MainWindow::saveAssayFile(const QString &fileName)
{
	if (!plateModel->writeFile(fileName)) {
		statusBar()->showMessage(tr("Saving canceled"), 2000);
		return false;
	}
	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File saved"), 2000);
	return true;
}

bool MainWindow::saveAssayAs()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save Assay"), ".",
		tr("ELISA Assay File (*.ELISA)"));
	
	if (fileName.isEmpty()) return false;
	return saveAssayFile(fileName);
}

void MainWindow::importOD() {
	
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Import OD from a File"), ".",
		tr("Text Documents (*.txt)\n"));
	if (fileName.isEmpty()) return;

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		statusBar()->showMessage(tr("Can not open file"), 2000);
		return;
	}

	QTextStream in(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);

	QString line[9];
	for (int i=0; i<9; i++) {
		line[i] = in.readLine();
	}
	
	for (int i=1; i<9; i++) {
		QStringList OD = line[i].split(QRegExp("\\s+"), QString::SkipEmptyParts);
		for (int j=1; j<OD.count(); j++){
			QModelIndex index = this->ODModel->index(i-1,j-1,QModelIndex());
			this->ODModel->setData(index,OD.at(j).toDouble(),Qt::EditRole);
		}
	}

	//if (lines.size() <1 ) statusBar()->showMessage(tr("OD Values Importation Failed"), 2000);
	//else  statusBar()->showMessage(tr("OD Values Importation Succeeded"), 2000);
	QApplication::restoreOverrideCursor();
	//hasStandardODValue = true;
	emit this->standardODValueSetSignal();
	return;
}

void MainWindow::updateStatusBar()
{
	/*
	if (this->focusWidget() == ODLayout || this->focusWidget() == ODLayout_1 )
		activeWidgetLabel->setText("Optical Density of Samples");
	else if (this->focusWidget() == plateLayout || this->focusWidget() == plateLayout_1)
		activeWidgetLabel->setText("Layout of Standards and Samples");
	else if ( this->focusWidget() == resultsLayout)
		activeWidgetLabel->setText("Results of Samples in This Test");
	else activeWidgetLabel->setText("Standard Curves of The Test Using Selected Standards");
	this->update();*/
}

void MainWindow::readPlate()
{
	//IplImage* img = cvLoadImage("C:\Program Files (x86)\Adobe\Acrobat 9.0\Acrobat");
	/*IplImage* img = cvLoadImage("DSCN2216.JPG");
	cvNamedWindow( "Example1", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Example1", img );
	cvWaitKey(0);
	cvReleaseImage( &img );
	cvDestroyWindow( "Example1" );*/

	
	cvNamedWindow( "Example2", CV_WINDOW_AUTOSIZE );
	//CvCapture* capture = cvCreateFileCapture("10302010.mp4");
	CvCapture* capture = cvCreateCameraCapture(0);
	IplImage* frame;
	while(1) {
		frame = cvQueryFrame( capture );
		if( !frame ) break;
		cvShowImage( "Example2", frame );
		char c = cvWaitKey(33);
		if( c == 27 ) break;
	}
	cvReleaseCapture( &capture );
	cvDestroyWindow( "Example2" );
	


	/*
	QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
	QString s;
	QTextStream out(&s);
	out.setFieldWidth(10);
	out.setFieldAlignment(QTextStream::AlignCenter);

    out << "List of ports:";
    for (int i = 0; i < ports.size(); i++) {
        out << "port name:" << ports.at(i).portName <<" \n";
        out << "friendly name:" << ports.at(i).friendName <<" \n" ;
        out << "physical name:" << ports.at(i).physName <<" \n" ;
        out << "enumerator name:" << ports.at(i).enumName <<" \n";
        out << "vendor ID:" << QString::number(ports.at(i).vendorID, 16) <<" \n" ;
        out << "product ID:" << QString::number(ports.at(i).productID, 16) <<" \n";
        out << "===================================";
    }
	QMessageBox::information(this,tr("ELISA Bioo"),s);


	QextSerialPort *port = new QextSerialPort("COM3",QextSerialPort::EventDriven);
	if (port)  QMessageBox::information(this,tr("ELISA Bioo"),tr("This COM3 port is opened!"));
	//connect(port,SIGNAL(readyRead()),this,SLOT(testPort()));
	//if ( port->open(QIODevice::ReadWrite) ) QMessageBox::information(this,tr("ELISA Bioo"),tr("This COM3 port will be implemented soon."));
	*/

	/*
	QLibrary lmLibrary("../Lib/LMmethod.dll",this);
	if (lmLibrary.load()) {
		QString name = QString(lmLibrary.fileName()).append(" is loaded!");
		QMessageBox::information(this,tr("ELISA Bioo"),name);
	}*/


	QSqlDatabase new_db = QSqlDatabase::addDatabase("QMYSQL");
	new_db.setDatabaseName("Driver={MySQL ODBC 5.1 Driver}; DATABASE=localhost;");
	new_db.setUserName("root");
	new_db.setPassword("");
	bool open = new_db.open(); 
	if (!open) {
		QMessageBox::information(this,tr("ELISA Bioo"),new_db.lastError().text());
		new_db.close();
		return;
	}

	

	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC"); 
	//db.setDatabaseName("Driver={Microsoft Access Driver (*.mdb, *.accdb)};DSN='FirstDSN';DBQ=Mycotoxin.accdbb"); 
	//db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};DSN='';DBQ=Database1.mdb");

	db.setDatabaseName("Driver={Microsoft Access Driver (*.mdb, *.accdb)};Dbq=D:\\12-C++ Programs\\00-ELISA\\03-ELISA BIOO\\Debug\\Mycotoxin.accdb;Uid=Admin;Pwd=;"); //this one worked
	//db.setDatabaseName("Driver={Microsoft Access Driver (*.mdb, *.accdb)};Dbq=Mycotoxin.accdb;Uid=Admin;Pwd=;");
	bool Success = db.open(); 
	if (!Success) {
		QMessageBox::information(this,tr("ELISA Bioo"),db.lastError().text());
		db.close();
		return;
	}
	QSqlQuery query;
	query.exec("SELECT Article,Toxin FROM Mycotoxin WHERE ID >= 1");
	while (query.next()) {
		QMessageBox::information(this,tr("ELISA Bioo"),"Query succeeded!");
		QString title = query.value(0).toString();
		QMessageBox::information(this,tr("ELISA Bioo"),title);
	}
	
	/*QSqlQuery query2;
	query2.prepare("INSERT INTO Mycotoxin (Article, Toxin) WHERE ID = '5' "
				   "VALUES (?, ?)");
	//query2.addBindValue(3);
	query2.addBindValue(" New JOC3");
	query2.addBindValue("New OTA3");	
	query2.exec();*/

	//QSqlQuery query3;
    
    //query3.addBindValue("ACIE2");
	//query3.addBindValue("ZER2");
    //query3.addBindValue(0);
	
	QByteArray inside="";
	QFile file("F:\\Users\\Zhenglai Fang\\Desktop\\1.txt");
	if (file.exists()) {
		if (file.open(QFile::ReadOnly)) {
			inside = file.readAll();
			file.close();
		}
		//inside.prepend(" first roll");
		//QMessageBox::warning(0,tr("ELISA Bioo"),inside);
	} 
	//inside.append(" second roll");          
	//QMessageBox::warning(0,tr("ELISA Bioo"),inside);
	//query3.prepare("UPDATE Mycotoxin SET Toxin = 'inside2', BLOB = inside WHERE ID = 4");
	//QBuffer buffer(&inside);
	
	//query3.prepare( "INSERT INTO Mycotoxin(ID) VALUES (:ID)" );
	//query3.bindValue( ":ID", 8 );
	//query3.bindValue( ":Article", "BMCL" );
	//query3.bindValue( ":Toxin", "BMCL1" );
	//query3.bindValue( ":BLOB", "ddddd");
	//query3.exec();
	//if( !query3.exec() )qFatal( "Failed to add data" );

	


	QSqlTableModel *model = new QSqlTableModel;
    model->setTable("Mycotoxin");
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	//model->setFilter("ID >= 1");
    model->select();
    model->removeColumn(0); // don't show the ID
    model->setHeaderData(0, Qt::Horizontal, tr("Article"));
    model->setHeaderData(1, Qt::Horizontal, tr("Toxin"));

	



	int ArticleIndex = model->record().indexOf("Article");
	int ToxinIndex = model->record().indexOf("Toxin");
	for (int i = 0; i < model->rowCount(); ++i) {
		QSqlRecord record = model->record(i);
		QString article = record.value(ArticleIndex).toString();
		QString toxin = record.value(ToxinIndex).toString();
		model->setData(model->index(i,ArticleIndex,QModelIndex()),article,Qt::EditRole);
		model->setData(model->index(i,ToxinIndex,QModelIndex()),toxin,Qt::EditRole);
	}
	

     QTableView *view = new QTableView;
     view->setModel(model);
     view->show();


	//QSqlDatabase::database().commit();
	db.close();
	//db.setUserName("Fang");
	//db.setPassword("Todd3913");
	//db.open();
	//QMessageBox::information(this,tr("ELISA Bioo"),db.userName());
	
	//QMessageBox::information(this,tr("ELISA Bioo"),tr("This function will be implemented soon."));
	//QWebView *view = new QWebView(this);
    //view->load(QUrl("http://www.biooscientific.com/"));
    //view->show();
	//db.setDatabaseName("Driver={Microsoft Access Driver (*.mdb, *.accdb)};Dbq=D:\\12-C++ Programs\\00-ELISA\\03-ELISA BIOO\\Debug\\Mycotoxin.accdb;Uid=Admin;Pwd=;");
	
	/*QSqlDatabase new_db = QSqlDatabase::addDatabase("QODBC");
	new_db.setDatabaseName("Driver={MySQL ODBC 5.1 Driver}; DATABASE=localhost;");
	new_db.setUserName("root");
	new_db.setPassword("");
	QMessageBox::warning(0,tr("ELISA Bioo"),"Mysql Database Opened!");
	if(!new_db.open()) {
		QMessageBox::warning(0,tr("ELISA Bioo"),"Mysql Database Opened!");
	}*/
		


}

void MainWindow::print(){
	
	if (!printer) printer = new QPrinter;  
	if (!printDialog) printDialog = new QPrintDialog(printer,this); 

	if (printDialog->exec() == QDialog::Accepted) {        
		QPainter p(printer); 


		//QRect rect = p.viewport();
		QPixmap pm1 = QPixmap::grabWidget(this->initView);
		QPixmap pm2 = QPixmap::grabWidget(this->curveWindow);
	    QPixmap pm3 = QPixmap::grabWidget(this->resultsLayout);
		
		p.drawPixmap(50,100,900,700,pm1);
		p.drawPixmap(50,850,900,400,pm2);

		if (! printer->newPage()) {
			qWarning("failed in flushing page to disk, disk full?");
			return;
		 }
		QPixmap pixmap(this->resultsLayout->size());
		this->resultsLayout->render(&pixmap);
		p.drawPixmap(50,100,900,1200,pixmap);
		p.end();


		//p.drawPixmap(0,900,800,300,pm3);
		//p.drawPixmap(0, 500, pm1);

		//printer->newPage();
		//QRect rect1 = p1.viewport();
		
	
		//QSize size1 = pm1.size();
		//size1.scale(rect1.size(), Qt::KeepAspectRatioByExpanding);
		//p1.setViewport(rect1.x(), rect1.y(),size1.width(), size1.height());
		//p1.setWindow(pm1.rect());
		//p.drawPixmap(0,0,pm1);
	}
}

void MainWindow::printPreview()
{
	QMessageBox::information(this,tr("ELISA Bioo"),tr("This function will be implemented soon."));
	//if (!printer) printer = new QPrinter;
	//QPrintPreviewDialog printPreview(this);
	//printPreview.exec();
}

void MainWindow::printerSetup()
{
	QMessageBox::information(this,tr("ELISA Bioo"),tr("This function will be implemented soon."));
	//QPageSetupDialog pageSetupDialog(this);
	//pageSetupDialog.exec();
}

void MainWindow::readerSetup()
{
	QMessageBox::information(this,tr("ELISA Bioo"),tr("This function will be implemented soon."));
}

//action slot handlers for Edit menu
void MainWindow::undo()
{
	QMessageBox::information(this,tr("ELISA Bioo"),tr("This function will be implemented soon."));
}

void MainWindow::copy()
{
	if (this->ODLayout->selectionModel()->hasSelection()) {
		this->ODLayout->copy();
		this->ODLayout->selectionModel()->reset();
		return;
	}
	if (this->ODLayout_1->selectionModel()->hasSelection()) {
		this->ODLayout_1->copy();
		this->ODLayout_1->selectionModel()->reset();
		return;
	}
	if (this->plateLayout->selectionModel()->hasSelection()) {
		this->plateLayout->copy();
		this->plateLayout->selectionModel()->reset();
		return;
	}
	if (this->plateLayout_1->selectionModel()->hasSelection()) {
		this->plateLayout_1->copy();
		this->plateLayout_1->selectionModel()->reset();
		return;
	}
	
	if ( this->resultsLayout->selectionModel()->hasSelection() ) {
		QModelIndexList selectedIndex = this->resultsLayout->selectionModel()->selectedIndexes();
		int rowMin, rowMax,columnMin, columnMax;
		rowMin = rowMax = selectedIndex.at(0).row();
		columnMin = columnMax = selectedIndex.at(0).column();

		for (int i = 1; i < selectedIndex.size(); ++i) {
			QModelIndex index = selectedIndex.at(i);
			if (index.row() < rowMin) rowMin = index.row();
			if (index.row() > rowMax) rowMax = index.row();
			if (index.column() < columnMin) columnMin = index.column();
			if (index.column() > columnMax) columnMax = index.column();
		}
	
		QString str;
		for (int i = rowMin; i <= rowMax; ++i) {
			if ( i > rowMin) str +="\n";
			for (int j = columnMin; j <= columnMax; ++j) {
				if (j > columnMin) str += "\t";
				QModelIndex index = this->resultModel->index(i,j,QModelIndex());
				str += this->resultModel->data(index,Qt::DisplayRole).toString();
			}
		}
		QApplication::clipboard()->setText(str);

		this->resultsLayout->selectionModel()->reset();
		return;
	}

	return;
}
void MainWindow::paste()
{
	if (this->ODLayout->selectionModel()->hasSelection()) {
		this->ODLayout->paste();
		this->ODLayout->selectionModel()->reset();
		return;
	}
	if (this->ODLayout_1->selectionModel()->hasSelection()) {
		this->ODLayout_1->paste();
		this->ODLayout_1->selectionModel()->reset();
		return;
	}
	if (this->plateLayout->selectionModel()->hasSelection()) {
		this->plateLayout->paste();
		this->plateLayout->selectionModel()->reset();
		return;
	}
	if (this->plateLayout_1->selectionModel()->hasSelection()) {
		this->plateLayout_1->paste();
		this->plateLayout_1->selectionModel()->reset();
		return;
	}
	
	return;
}

void MainWindow::Delete() 
{
	if (this->ODLayout->selectionModel()->hasSelection()) {
		this->ODLayout->Delete();
		this->ODLayout->selectionModel()->reset();
		return;
	}
	if (this->ODLayout_1->selectionModel()->hasSelection()) {
		this->ODLayout_1->Delete();
		this->ODLayout_1->selectionModel()->reset();
		return;
	}
	if (this->plateLayout->selectionModel()->hasSelection()) {
		this->plateLayout->Delete();
		this->plateLayout->selectionModel()->reset();
		return;
	}
	if (this->plateLayout_1->selectionModel()->hasSelection()) {
		this->plateLayout_1->Delete();
		this->plateLayout_1->selectionModel()->reset();
		return;
	}
	
	return;
}

void MainWindow::cut()
{
	if (this->ODLayout->selectionModel()->hasSelection()) {
		this->ODLayout->copy();
		this->ODLayout->Delete();
		this->ODLayout->selectionModel()->reset();
		return;
	}
	if (this->ODLayout_1->selectionModel()->hasSelection()) {
		this->ODLayout_1->copy();
		this->ODLayout_1->Delete();
		this->ODLayout_1->selectionModel()->reset();
		return;
	}
	if (this->plateLayout->selectionModel()->hasSelection()) {
		this->plateLayout->copy();
		this->plateLayout->Delete();
		this->plateLayout->selectionModel()->reset();
		return;
	}
	if (this->plateLayout_1->selectionModel()->hasSelection()) {
		this->plateLayout_1->copy();
		this->plateLayout_1->Delete();
		this->plateLayout_1->selectionModel()->reset();
		return;
	}
	
	return;
	
}

bool MainWindow::fillSampleName()
{
	FillSampleNameWizard w(this->plateModel,this);
	return w.exec();
}


bool MainWindow::newMethod()
{
	MethodWizard w(this->plateModel,this);
	return w.exec();
}

bool MainWindow::openMethod()
{
	QString fileName = QFileDialog::getOpenFileName(this,
			tr("Open Method File"), ".",
			"ELISA Method File (*.Met)\n");
	if (fileName.isEmpty()) return false;
			
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(this, tr("ELISA Method File"),
			tr("Cannot read file %1:\n%2.")
			.arg(file.fileName())
			.arg(file.errorString()));
		return false;
	}
	else {
		if (!this->plateModel->readMethod(fileName)) {
			statusBar()->showMessage(tr("Loading canceled"), 2000);
			return false;
		}
		statusBar()->showMessage(tr("Method File loaded"), 2000);
		return true;
	}
}

bool MainWindow::saveMethod()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save Method File"), ".",
		tr("ELISA Method File (*.Met)"));
	
	if (fileName.isEmpty()) return false;
	else {
		if (!plateModel->writeMethod(fileName)) {
			statusBar()->showMessage(tr("Saving Method File Canceled"), 2000);
			return false;
		}
		statusBar()->showMessage(tr("File saved"), 2000);
		return true;
	}
}

bool MainWindow::saveMethodAs()
{
	return true;
}

bool MainWindow::setStandard()
{
	StandardWizard w(this->plateModel,this);
	return w.exec();
}

bool MainWindow::setDilutionFactor()
{
	DilutionFactorWizard w(this->plateModel,this);
	return w.exec();
}

bool MainWindow::setSampleTypes()
{
	SampleTypeWizard w(this->plateModel,this);
	return w.exec();
}
	
bool MainWindow::setCutOffValues()
{
	CutOffValueWizard w(this->plateModel,this);
	return w.exec();
}

void MainWindow::linearFit() 
{
	//QModelIndex algorithmIndex = plateModel->index(3,1,QModelIndex());
	//plateModel->setData(algorithmIndex,1,Qt::UserRole+5);

	regressionMethod = MyPlot::linearFit;
	if (curveWindow) {
		lateView->removeTab(2);
		delete curveWindow;
	}
	QVector<double> stdData[2];
	for (int i = 0; i < this->plateModel->getStandardIndexList().size(); ++i) {
		QModelIndex index = this->plateModel->getStandardIndexList().at(i);
		stdData[0].append(this->plateModel->data(index,Qt::UserRole).toDouble());
		stdData[1].append(this->plateModel->data(index,Qt::UserRole+4).toDouble());
	}
	curveWindow = new plotView("Standard Curve",stdData,regressionMethod,this);
	lateView->insertTab(2,curveWindow,tr("Standard Curve"));
	this->LateView();
	lateView->setCurrentWidget(curveWindow);
	this->lateViewAction->setEnabled(true);
	this->updateResultsLayout();
	this->update();
}

void MainWindow::logitLogFit() {
	//QModelIndex algorithmIndex = plateModel->index(3,1,QModelIndex());
	//plateModel->setData(algorithmIndex,2,Qt::UserRole+5);

	regressionMethod = MyPlot::logitLogFit;
	if (curveWindow) {
		lateView->removeTab(2);
		delete curveWindow;
	}
	QVector<double> stdData[2];
	for (int i = 0; i < this->plateModel->getStandardIndexList().size(); ++i) {
		QModelIndex index = this->plateModel->getStandardIndexList().at(i);
		stdData[0].append(this->plateModel->data(index,Qt::UserRole).toDouble());
		stdData[1].append(this->plateModel->data(index,Qt::UserRole+4).toDouble());
	}
	curveWindow = new plotView("Standard Curve",stdData,regressionMethod,this);
	lateView->insertTab(2,curveWindow,tr("Standard Curve"));
	this->LateView();
	lateView->setCurrentWidget(curveWindow);
	this->lateViewAction->setEnabled(true);
	this->updateResultsLayout();
	this->update();
}

void MainWindow::cubicSplineFit() {
	//QModelIndex algorithmIndex = plateModel->index(3,1,QModelIndex());
	//plateModel->setData(algorithmIndex,3,Qt::UserRole+5);

	regressionMethod = MyPlot::cubicSplineFit;
	if (curveWindow) {
		lateView->removeTab(2);
		delete curveWindow;
	}
	QVector<double> stdData[2];
	for (int i = 0; i < this->plateModel->getStandardIndexList().size(); ++i) {
		QModelIndex index = this->plateModel->getStandardIndexList().at(i);
		stdData[0].append(this->plateModel->data(index,Qt::UserRole).toDouble());
		stdData[1].append(this->plateModel->data(index,Qt::UserRole+4).toDouble());
	}
	curveWindow = new plotView("Standard Curve",stdData,regressionMethod,this);
	lateView->insertTab(2,curveWindow,tr("Standard Curve"));
	this->LateView();
	lateView->setCurrentWidget(curveWindow);
	this->lateViewAction->setEnabled(true);
	this->updateResultsLayout();
	this->update();
}

void MainWindow::fourPLFit()
{
	regressionMethod = MyPlot::fourPLFit;
	if (curveWindow) {
		lateView->removeTab(2);
		delete curveWindow;
	}
	QVector<double> stdData[2];
	for (int i = 0; i < this->plateModel->getStandardIndexList().size(); ++i) {
		QModelIndex index = this->plateModel->getStandardIndexList().at(i);
		stdData[0].append(this->plateModel->data(index,Qt::UserRole).toDouble());
		stdData[1].append(this->plateModel->data(index,Qt::UserRole+4).toDouble());
	}
	curveWindow = new plotView("Standard Curve",stdData,regressionMethod,this);
	lateView->insertTab(2,curveWindow,tr("Standard Curve"));
	this->LateView();
	lateView->setCurrentWidget(curveWindow);
	this->lateViewAction->setEnabled(true);
	this->updateResultsLayout();
	this->update();
}

void MainWindow::standardCurveAndResultGenerate(int algorithm)
{
	//MyPlot::RegressionMethod regressionAlgorithm;
	if ( algorithm == 1) this->linearFit();	
	else if (algorithm == 2) this->logitLogFit();
	else if (algorithm == 3) this->cubicSplineFit();
	else this->fourPLFit();
	return;
}
//action slot handlers for windows menu
void MainWindow::InitView()
{
	centralWidget->setCurrentWidget(initView);
}
void MainWindow::LateView()
{
	centralWidget->setCurrentWidget(lateView);
	lateView->setCurrentWidget(curveWindow);
}

//action slot handlers for help menu
void MainWindow::indexHelp()
{
	QMessageBox::information(this,tr("ELISA Bioo"),tr("This function will be implemented soon."));
}
void MainWindow::usingHelp()
{
	QMessageBox::information(this,tr("ELISA Bioo"),tr("This function will be implemented soon."));
}
void MainWindow::aboutBioo()
{
	QMessageBox::about(this, tr("About ELISA"),
		tr("<h2>ELISA 1.0</h2>"
		"<p>Copyright &copy; 2012 Bioo Scientific Inc."
		"<p>ELISA is a small application for "
		"reduction of immunological assay data such as ELISA, "
		"and many other biological assays."));
}


void MainWindow::setLateViewEnabled()
{
	if (hasStandardODValue && hasStandardConc) {
		lateViewAction->setEnabled(true);
	}
}

void MainWindow::setThisWindowModified()
{
	this->setWindowModified(true);

	//update the assay info in the plate title
	QModelIndex assayNameIndex = plateModel->index(1,1,QModelIndex());
	assayNameLabel->setText(QString("Assay: %1").arg(plateModel->data(assayNameIndex,Qt::UserRole+5).toString()));
	QModelIndex catalogNumberIndex = plateModel->index(2,1,QModelIndex());
	assayCatalogLabel->setText(QString("MaxSignal Catalog No.: %1").arg(plateModel->data(catalogNumberIndex,Qt::UserRole+5).toString()));
	QModelIndex TechNameIndex = plateModel->index(4,1,QModelIndex());
	assayTechNameLabel->setText(QString("Performed By: %1").arg(plateModel->data(TechNameIndex,Qt::UserRole+5).toString()));
	
	//check whether a standard curve can be generated and the buttons in result menu can be enabled
	//this->setAlgorithmButtonEnabled();
	this->algorithmGroup->setEnabled(this->plateModel->AlgorithmButtonEnabled());

	//update the standard curve algorithm. If a curve can be generated and caluclation can be done, then proceed to the calculation
	QModelIndex algorithmIndex = plateModel->index(3,1,QModelIndex());
	int algorithm = plateModel->data(algorithmIndex,Qt::UserRole+5).toInt();
	if ( algorithm  && algorithmGroup->isEnabled()) this->standardCurveAndResultGenerate(algorithm);

	//QMessageBox::warning(this,tr("ELISA Bioo"),tr("OD Values in some cells changed."));
	//for ( int i = 0; i < this->standardIndexList.size(); ++i ) {
		//QModelIndex index = this->standardIndexList.at(i);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
	curFile = fileName;
	setWindowModified(false);
	QString shownName = "Untitled";
	if (!curFile.isEmpty()) {
		shownName = strippedName(curFile);
		recentFiles.removeAll(curFile);
		recentFiles.prepend(curFile);
		foreach (QWidget *win, QApplication::topLevelWidgets()) {
			if (MainWindow *mainWin = qobject_cast<MainWindow *>(win))
				mainWin->updateRecentFileActions();
		}
	}
	setWindowTitle(tr("%1[*] - %2").arg(shownName)
		.arg(tr("ELISA Bioo")));
}

void MainWindow::updateRecentFileActions()
{
	QMutableStringListIterator i(recentFiles);
	while (i.hasNext()) {
		if (!QFile::exists(i.next()))
			i.remove();
	}
	for (int j = 0; j < MaxRecentFiles; ++j) {
		if (j < recentFiles.count()) {
			QString text = tr("&%1 %2")
				.arg(j + 1)
				.arg(strippedName(recentFiles[j]));
			recentFileActions[j]->setText(text);
			recentFileActions[j]->setData(recentFiles[j]);
			recentFileActions[j]->setVisible(true);
		} else {
			recentFileActions[j]->setVisible(false);
		}
	}
	separatorAction->setVisible(!recentFiles.isEmpty());
}

void MainWindow::updateResultsLayout()
{
	double sumOfNegControlOD = 0;
	int numberOfNegControl = 0;
	for (int i = 0; i < this->plateModel->getStandardIndexList().size(); ++i) {
		QModelIndex index = this->plateModel->getStandardIndexList().at(i);
		if (this->plateModel->data(index,Qt::UserRole).toDouble() == 0) {
			sumOfNegControlOD += this->plateModel->data(index,Qt::UserRole+4).toDouble();
			numberOfNegControl += 1;
		}
	}
	double NegControlAverageOD = 0;
	if (numberOfNegControl != 0)  NegControlAverageOD = sumOfNegControlOD/numberOfNegControl;
	if (NegControlAverageOD == 0) return;

	QModelIndexList sampleList;
	for (int i = 0; i < plateModel->columnCount(); ++i) {
		for (int j = 0; j < plateModel->rowCount(); ++j) {
			QModelIndex index = plateModel->index(j,i,QModelIndex());
			if (!plateModel->getStandardIndexList().contains(index) && plateModel->data(index,Qt::UserRole+4).toDouble() >= 0.005) sampleList.append(index);
		}
	}
	

	QString currentSampleName, currentSampleType,currentCutOffValue, currentDF, OD1, OD2, B_Bo, calculate_Conc, pass_fail, comments;
	QString nextSampleNameByColumn, nextSampleNameByRow;
	QStringList results;
	while (!sampleList.isEmpty()) {
		QModelIndex index = sampleList.at(0);
		currentSampleName = plateModel->data(index,Qt::DisplayRole).toString();
		currentSampleType = plateModel->data(index,Qt::UserRole+2).toString();
		currentCutOffValue = plateModel->data(index,Qt::UserRole+3).toString();
		currentDF = plateModel->data(index,Qt::UserRole+1).toString();
		OD1 = plateModel->data(index,Qt::UserRole+4).toString();

		//search duplicate samples in adjacent cell
		QModelIndex nextIndexByColumn, nextIndexByRow;
		int toBeRemovedIndex;
		if (index.row() == (plateModel->rowCount()-1) ) nextIndexByColumn = plateModel->index(0,index.column()+1,QModelIndex());
		else nextIndexByColumn = plateModel->index(index.row()+1,index.column(),QModelIndex());
		if (index.column() == (plateModel->columnCount()-1) ) nextIndexByRow = plateModel->index(index.row()+1,0,QModelIndex());
		else nextIndexByRow = plateModel->index(index.row(),index.column()+1,QModelIndex());
		
		if ( sampleList.contains(nextIndexByColumn) && (currentSampleName == plateModel->data(nextIndexByColumn,Qt::DisplayRole).toString()) ) {
			OD2 = plateModel->data(nextIndexByColumn,Qt::UserRole+4).toString();
			toBeRemovedIndex = 1;
		}
		else if ( sampleList.contains(nextIndexByRow) && (currentSampleName == plateModel->data(nextIndexByRow,Qt::DisplayRole).toString()) ) {
			OD2 = plateModel->data(nextIndexByRow,Qt::UserRole+4).toString();
			toBeRemovedIndex = 2;
		}
		else {
			OD2 = QString("N/A");
			toBeRemovedIndex = 0;
		}

		double average_OD;
		if ( toBeRemovedIndex > 0 ) average_OD = (OD1.toDouble() + OD2.toDouble() )/2;
		else average_OD = OD1.toDouble();
		B_Bo.sprintf("%.3f",average_OD/NegControlAverageOD);
		double calcConc = this->curveWindow->getPlotWidget()->calculateConcs(average_OD,currentDF.toDouble());
		calculate_Conc.sprintf("%.3f",calcConc);
		if (currentCutOffValue.toDouble() < calculate_Conc.toDouble() ) pass_fail = QString(tr("Fail"));
		else pass_fail = QString(tr("Pass"));
		comments = QString(tr(""));
		QStringList rowContent;
		rowContent << currentSampleName << currentSampleType << currentCutOffValue << currentDF << OD1 << OD2 << B_Bo << calculate_Conc << pass_fail << comments;
		QString rowContentString = rowContent.join("\t");
		results.append(rowContentString);

		//remove processed samples's index
		sampleList.removeOne(index);
		if (toBeRemovedIndex == 1 ) sampleList.removeOne(nextIndexByColumn);
		if (toBeRemovedIndex == 2 ) sampleList.removeOne(nextIndexByRow);
	}
	
	//QString numberOfSample = QString("%1 samples found").arg(results.size());
	//QMessageBox::warning(0,tr("ELISA Bioo"),numberOfSample);


	if (!resultModel) {
		resultModel = new ResultTableModel(results.size(),10);
		this->resultsLayout->setModel(resultModel);
		resultModel->setContents(results);
	}
	else {
		ResultTableModel* tempModel = resultModel;
		resultModel = new ResultTableModel(results.size(),10);
		this->resultsLayout->setModel(resultModel);
		resultModel->setContents(results);
		delete tempModel;
	}
}

void MainWindow::openRecentFile()
{
	if (okToContinue()) {
		QAction *action = qobject_cast<QAction *>(sender());
		if (action)
			loadAssayFile(action->data().toString());
	}
}

/*
bool WriteComPort(CString PortSpecifier, CString data)
{
	DCB dcb;
	DWORD byteswritten;
	HANDLE hPort = CreateFile(
		PortSpecifier,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
		);
	if (!GetCommState(hPort,&dcb))
		return false;
	dcb.BaudRate = CBR_9600; //9600 Baud
	dcb.ByteSize = 8; //8 data bits
	dcb.Parity = NOPARITY; //no parity
	dcb.StopBits = ONESTOPBIT; //1 stop
	if (!SetCommState(hPort,&dcb))
		return false;
	bool retVal = WriteFile(hPort,data,1,&byteswritten,NULL);
	CloseHandle(hPort); //close the handle
	return retVal;
}*/