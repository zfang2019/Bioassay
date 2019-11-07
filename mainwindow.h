#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qvector.h>
#include <QActionGroup>
//#include <Windows.h>
//#include <afx.h>

#include "MyView.h"
#include "myplot.h"

#include "LinearRegression.h"
#include "Cubic_Spline_Regression.h"
#include "Logit_Log_Regression.h"
#include <TableModel.h>
#include <methodDialog.h>

//forward declaration
class QAction;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QWidget;
class QPrinter;
class QPrintDialog;
class QTabWidget;
class QStackedWidget;
class QTableWidget;
class QTableView;
class QString;

//dialog class forward declaration
//class setStdDialog;
class SetDiluFactorDialog;
class SetSampleTypeDialog;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
signals:
	//void findNext(const QString &str, Qt::CaseSensitivity cs);
	//void findPrevious(const QString &str, Qt::CaseSensitivity cs);
	void standardODValueSetSignal();
	void standardConcentrationSetSignal();
	//void CurveisAbleToBeGenerated();
protected:
	void closeEvent(QCloseEvent *event);
	void paintEvent(QPaintEvent *event);
	
	private slots:
		bool newMethod();
		void newAssay();
		bool openMethod();
		bool openAssay();
		bool saveMethod();
		bool saveAssay();
		bool saveMethodAs();
		bool saveAssayAs();
		void importOD();
		void readPlate();
		void updateStatusBar();
		void print();
		void printPreview();
		void printerSetup();
		void readerSetup();

		void undo();
		void cut();
		void copy();
		void paste();
		void Delete();
		bool fillSampleName();
		bool setStandard();
		bool setDilutionFactor();
		bool setSampleTypes();
		bool setCutOffValues();
		
		void linearFit();
		void logitLogFit();
		void cubicSplineFit();
		void fourPLFit();
		void standardCurveAndResultGenerate(int algorithm);

		void InitView();
		void LateView();
		
		void indexHelp();
		void usingHelp();
		void aboutBioo();
		
		void openRecentFile();
		void setLateViewEnabled();
		void setThisWindowModified();
		bool isAbleToGenerateCurve();
private:
	void init();
	void layoutResize(QTableView *view);
	void createActions();
	void createMenus();
	void createContextMenu();
	void createToolBars();
	void createStatusBar();
	void readSettings();
	void writeSettings();
	bool okToContinue();
	
	bool loadAssayFile(const QString &fileName);
	bool saveAssayFile(const QString &fileName);
	void setCurrentFile(const QString &fileName);
	void updateRecentFileActions();
	void updateResultsLayout();
	QString strippedName(const QString &fullFileName);
	//bool WriteComPort(CString PortSpecifier, CString data);
	
	//two tables for OD and plate layout, the application's initial views
	ODTableModel *ODModel;
	ODView *ODLayout, *ODLayout_1;
	QLabel *ODTitle;
	LayoutTableModel *plateModel;
	LayoutView *plateLayout, *plateLayout_1;
	QLabel *plateTitle;
	QLabel *assayNameLabel;
	QLabel *assayCatalogLabel;
	QLabel *assayTechNameLabel;

	//widget for plot and calculated results
	plotView *curveWindow;
	MyPlot::RegressionMethod regressionMethod;

	//results tab window
	QTableView *resultsLayout;
	ResultTableModel *resultModel;

	//tab widgets, the later views when there are curves and results
	QWidget *initView;
	QTabWidget *lateView;
	QStackedWidget *centralWidget;

	bool hasStandardODValue;
	bool hasStandardConc;
	bool ableToGenerateCurve;
	
	QModelIndexList standardIndexList;
	//[0] for standards' concentration [1] for standards' OD values
	QVector<double> standardData[4];
	int standard_number;

	QGridLayout *plateTitleLayout;
	QVBoxLayout *vBoxLayout;
	//display the current activated widget in status bar
	QLabel *activeWidgetLabel;
	
	//menus
	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *methodMenu;
	QMenu *resultsMenu;
	QMenu *windowMenu;
	QMenu *helpMenu;
	
	QMenu *readSubMenu;

	//tool bar
	QToolBar *fileToolBar;
	QToolBar *editToolBar;
	QToolBar *resultsToolBar;
	QToolBar *helpToolBar;

	//printer
	QPrinter *printer;
	
	//Dialogs
	QPrintDialog *printDialog;
	//setStdDialog stdDialog;
	SetDiluFactorDialog *diluFactorDialog;
    SetSampleTypeDialog *sampleTypeDailog;
	
	//recent file action
	static QStringList recentFiles;
	QString curFile;
	enum { MaxRecentFiles = 5 };
	QAction *recentFileActions[MaxRecentFiles];
	QAction *separatorAction;

	//actions in File menu
	QAction *newAssayAction;
	QAction *openAssayAction;
	QAction *saveAssayAction;
	QAction *saveAssayAsAction;
	QAction *importODDataAction;
	QAction *readPlateAction;
	QAction *readerSetupAction;
	QAction *printAction;
	QAction *printPreviewAction;
	QAction *printerSetupAction;
	QAction *closeAction;
	QAction *exitAction;
	
	//actions in Edit menu
	QAction *undoAction;
	QAction *cutAction;
	QAction *copyAction;
	QAction *pasteAction;
	QAction *deleteAction;
	QAction *fillSampleNameAction;

	//method menu
	QAction *newMethodAction;
	QAction *openMethodAction;
	QAction *saveMethodAction;
	QAction *saveMethodAsAction;
	QAction *setStandardAction;
	QAction *setDilutionFactorAction;
    QAction *classifySampleAction;
	QAction *setCutOffValuesAction;

	//actions in Results menu
	QActionGroup *algorithmGroup;
	QAction *linearAction;
	QAction *logitLogAction;
	QAction *cubicSplineAction;
	QAction *fourPLAction;
	
	//actions in Window menu
	QAction *initViewAction;
	QAction *lateViewAction;
	
	//actions in help menu
	QAction *indexAction;
	QAction *usingHelpAction;
	QAction *aboutBiooAction;
};
#endif

