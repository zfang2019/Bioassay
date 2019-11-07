#ifndef METHODDIALOG_H
#define METHODDIALOG_H

#include <QWizard>
#include <QItemSelection>
#include <MyView.h>
#include <TableModel.h>
#include <QModelIndex>

//QT_BEGIN_NAMESPACE
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QGridLayout;
class QHBoxLayout;
class QVBoxLayout;
class QTableView;
class QTableWidget;
class QPushButton;
class QComboBox;
//QT_END_NAMESPACE


class MethodWizard : public QWizard
{
	Q_OBJECT
public:
	MethodWizard(LayoutTableModel *model,QWidget *parent = 0);
	void accept();
private:
	LayoutTableModel *pModel;
	LayoutTableModel *inputModel;//tempary pointer for data transfer purpose
	LayoutView *pView;
	QString elisaName;
	QString catalogNumber;
	QString techName;
	int standardCurveAlgorithm;
	QList<double> stdConcs;
};

class StandardWizard : public QWizard
{
	Q_OBJECT
public:
	StandardWizard(LayoutTableModel *model,QWidget *parent = 0);
	void accept();
private:
	LayoutTableModel *pModel;
	LayoutTableModel *inputModel;//tempary pointer for data transfer purpose
	LayoutView *pView;
	QList<double> stdConcs;
};

class DilutionFactorWizard : public QWizard
{
	Q_OBJECT
public:
	DilutionFactorWizard(LayoutTableModel *model,QWidget *parent = 0);
	void accept();
private:
	LayoutTableModel *pModel;
	LayoutTableModel *inputModel;//tempary pointer for data transfer purpose
	LayoutView *pView;
};

class SampleTypeWizard : public QWizard
{
	Q_OBJECT
public:
	SampleTypeWizard(LayoutTableModel *model,QWidget *parent = 0);
	void accept();
private:
	LayoutTableModel *pModel;
	LayoutTableModel *inputModel;//tempary pointer for data transfer purpose
	LayoutView *pView;
};

class CutOffValueWizard : public QWizard
{
	Q_OBJECT
public:
	CutOffValueWizard(LayoutTableModel *model,QWidget *parent = 0);
	void accept();
private:
	LayoutTableModel *pModel;
	LayoutTableModel *inputModel;//tempary pointer for data transfer purpose
	LayoutView *pView;
};

class IntroPage : public QWizardPage
{
	Q_OBJECT
public:
    IntroPage(QWidget *parent = 0);
private:
    QLabel *label;
};

class GeneralPage : public QWizardPage
{
	Q_OBJECT
public:
	GeneralPage(QWidget *parent = 0);
	bool isComplete() const;
protected:
	void keyPressEvent(QKeyEvent *event);
private:
	QLabel *elisaNameLabel;
	QLineEdit *elisaNameLineEdit;
	QLabel *catalogNumberLabel;
	QLineEdit *catalogNumberLineEdit;
	QLabel *techNameLabel;
	QLineEdit *techNameLineEdit;
	QLabel* algorithmLabel;
	QComboBox* algorithmComboBox;
	QLabel *requiredField;
	QGridLayout *gridLayout;
	QVector<QLineEdit*> lineEdits;//for convenience of key press event only
};

class SelectStdPage : public QWizardPage
{
	Q_OBJECT
	Q_PROPERTY (QStringList StdIndexList READ StdIndexList RESET UpdateStdIndexList NOTIFY StdIndexListChanged)
public:
	SelectStdPage(LayoutTableModel *pModel,QWidget *parent = 0);
	QStringList StdIndexList();
	bool isComplete() const;
	signals:
	void StdIndexListChanged();
public slots:
		void UpdateStdIndexList();
protected:
	void initializePage();
	void cleanupPage();
	int nextId() const;
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);

	private slots:
		void keepCurrentStandard(void);
		void clearCurrentStandard(void);
private:
	LayoutView *view;
	LayoutTableModel *model;
	QStringList stdIndexList;
	QGridLayout *gridLayout;
	QPushButton *keepButton;
	QPushButton *resetButton;
};

//Q_DECLARE_METATYPE(QModelIndex)

class ConcPage : public QWizardPage
{
	Q_OBJECT
public:
	ConcPage(QWidget *parent = 0);
	bool isComplete() const;
protected:
	void initializePage();
	void cleanupPage();
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent *event);
private:
	QLabel *cellPositionHeader,*concHeader,*warningLabel;
	QVector<QLabel*> cellPositions;
	QVector<QLineEdit*> concLineEdits;
	QGridLayout *gridLayout;
};

class DiluFactorPage : public QWizardPage
{
	Q_OBJECT
public:
	DiluFactorPage(LayoutTableModel *pModel,QWidget *parent = 0);
protected:
	void initializePage();
	void cleanupPage();
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
private slots:
		void setDiluFactors(void);
		void clearSelectionAndDiluFactor(void);
		void setButtonEnabled(void);
private:
	LayoutView *view;
	LayoutTableModel *model;
	QLabel *diluFactorLabel;
	QLineEdit *diluFactorLineEdit;
	QPushButton *setButton;
	QPushButton *clearButton;
	QGridLayout *gridLayout;
	QList<QColor> bgColor;
};

class SampleTypePage : public QWizardPage
{
	Q_OBJECT
public:
	SampleTypePage(LayoutTableModel *pModel,QWidget *parent = 0);
protected:
	void initializePage();
	void cleanupPage();
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
private slots:
		void setSampleType(void);
		void clearSelectionAndSampleType(void);
		void setButtonEnabled(void);
private:
	LayoutView *view;
	LayoutTableModel *model;
	QLabel *SampleTypeLabel;
	QLineEdit *SampleTypeLineEdit;
	QPushButton *setButton;
	QPushButton *clearButton;
	QGridLayout *gridLayout;
	QList<QColor> bgColor;
};

class CutOffPage : public QWizardPage
{
	Q_OBJECT
public:
	CutOffPage(LayoutTableModel *pModel,QWidget *parent = 0);
protected:
	void initializePage();
	void cleanupPage();
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
private slots:
		void setCutOffValues(void);
		void clearSelectionAndCutOffValue(void);
		void setButtonEnabled(void);
private:
	LayoutView *view;
	LayoutTableModel *model;
	QLabel *CutOffLabel;
	QLineEdit *CutOffLineEdit;
	QPushButton *setButton;
	QPushButton *clearButton;
	QGridLayout *gridLayout;
	QList<QColor> bgColor;
};


#endif
