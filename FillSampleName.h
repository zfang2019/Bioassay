#ifndef FILLSAMPLENAME_H
#define FILLSAMPLENAME_H

#include <QWizard>
#include <QItemSelection>
#include <MyView.h>
#include <TableModel.h>
#include <QModelIndex>

//QT_BEGIN_NAMESPACE
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


class FillSampleNameWizard : public QWizard
{
	Q_OBJECT
public:
	FillSampleNameWizard(LayoutTableModel *model,QWidget *parent = 0);
	void accept();
private:
	LayoutTableModel *pModel;
	LayoutTableModel *inputModel;//tempary pointer for data transfer purpose
	LayoutView *pView;
};

class SampleNamePage : public QWizardPage
{
	Q_OBJECT
public:
	SampleNamePage(LayoutTableModel *pModel,QWidget *parent = 0);
protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
private slots:
		void setSampleName(void);
		void clearSelectionAndSampleName(void);
		void setButtonEnabled(void);
private:
	LayoutView *view;
	LayoutTableModel *model;
	QLabel *SampleNameLabel;
	QLineEdit *SampleNameLineEdit;
	QComboBox *singleDuplicateComboBox;
	QPushButton *setButton;
	QPushButton *clearButton;
	QGridLayout *gridLayout;
};


#endif