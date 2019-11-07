#include <QtGui>
#include <QModelIndex>

#include "FillSampleName.h"


FillSampleNameWizard::FillSampleNameWizard(LayoutTableModel *model,QWidget *parent)
	: QWizard(parent)
{
	pModel= new LayoutTableModel(8,12);
	pView = new LayoutView(pModel);
	inputModel = model;
	
	pModel->CopySampleName(model);
	pModel->CopyStandardIndexList(model);

	this->setPage(0, new SampleNamePage(pModel));
	
	this->setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));
    this->setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/background.png"));
	this->setWindowTitle(tr("Set/Reset Standards"));
}

void FillSampleNameWizard::accept() 
{	
	inputModel->CopySampleName(pModel);
	//inputModel->CopyStandardIndexList(pModel);
	return QDialog::accept();
}

SampleNamePage::SampleNamePage(LayoutTableModel *pModel,QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Automatically fill a sequence of samples"));
	//this->setSubTitle(tr("Select sets of samples with same dilution factors and assign a factor and on and on ..."));
	view = new LayoutView(pModel,this);
	model = pModel;
	view->update();

	view->selectionModel()->reset();
	SampleNameLabel = new QLabel(tr("Put a common name here:"),this);

	SampleNameLineEdit = new QLineEdit(this);
	SampleNameLabel->setBuddy(SampleNameLineEdit);
	
	singleDuplicateComboBox = new QComboBox(this);
	QStringList sampleDuplicateList;
	sampleDuplicateList << "Single" << "Duplicate";
	singleDuplicateComboBox->insertItems(0,sampleDuplicateList);

	setButton = new QPushButton(tr("Fill"),this);
	setButton->setEnabled(false);
	clearButton = new QPushButton(tr("Clear"),this);
	clearButton->setEnabled(false);
	gridLayout = new QGridLayout(this);
	setLayout(gridLayout);

	gridLayout->addWidget(view,0,0,9,13);
	gridLayout->addWidget(SampleNameLabel,9,0,1,3);
	gridLayout->addWidget(SampleNameLineEdit,9,3,1,4);
	gridLayout->addWidget(singleDuplicateComboBox,9,7,1,2);
	gridLayout->addWidget(setButton,9,9,1,2);
	gridLayout->addWidget(clearButton,9,11,1,2);
	
	for (int i = 0; i < gridLayout->rowCount(); ++i)
		gridLayout->setRowStretch(i,1);
	for (int i = 0; i < gridLayout->columnCount(); ++i)
		gridLayout->setColumnStretch(i,1);
	gridLayout->setContentsMargins(4,4,4,4);

	//this->setMinimumSize(540,280);
	/*QRect rect = this->geometry();
	rect.adjust(2,2,-2,-2);
	gridLayout->setGeometry(rect);
	gridLayout->setAlignment(Qt::AlignTop|Qt::AlignLeft);*/

	connect(SampleNameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setButtonEnabled()));
	connect(setButton,SIGNAL(clicked(bool)),this,SLOT(setSampleName(void)));
	connect(clearButton,SIGNAL(clicked(bool)),this,SLOT(clearSelectionAndSampleName(void)));
	connect(view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(setButtonEnabled()));
	//this->update();
	//connect(view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(UpdateStdIndexList()));
	//connect(this,SIGNAL(StdIndexListChanged()),this,SIGNAL(completeChanged()));*/
}


void SampleNamePage::setSampleName(void)
{
	QString tempString = SampleNameLineEdit->text();
	QModelIndexList selectedIndexList = view->selectionModel()->selectedIndexes();
	QModelIndexList standardIndexList = model->getStandardIndexList();
	for (int i = 0; i < standardIndexList.size(); ++i) {
		QModelIndex standardIndex = standardIndexList.at(i);
		selectedIndexList.removeOne(standardIndex);
	}
	if (selectedIndexList.isEmpty()) return;
	
	int i = 1;
	while (!selectedIndexList.isEmpty()){
		QModelIndex index = selectedIndexList.at(0);
		QString sampleName = QString("%1_%2").arg(tempString).arg(i);
		model->setData(index,sampleName,Qt::EditRole);
		
		//QMessageBox::warning(0,"Bioo ELISA",sampleName);
		selectedIndexList.removeOne(index);
		//search duplicate samples in adjacent cell
		QModelIndex nextIndexByColumn;
		if (index.row() == (model->rowCount()-1) ) nextIndexByColumn = model->index(0,index.column()+1,QModelIndex());
		else nextIndexByColumn = model->index(index.row()+1,index.column(),QModelIndex());
		if ( selectedIndexList.contains(nextIndexByColumn) && (this->singleDuplicateComboBox->currentIndex() == 1) ) {
			model->setData(nextIndexByColumn,sampleName,Qt::EditRole);
			selectedIndexList.removeOne(nextIndexByColumn);
		}
		++i;	
	} 
	view->resizeColumnsToContents();
	this->SampleNameLineEdit->setText("");
	view->selectionModel()->reset();
}

void SampleNamePage::clearSelectionAndSampleName(void)
{
	this->SampleNameLineEdit->setText("");// or this->diluFactorLineEdit->clear();
	view->selectionModel()->reset();
}

void SampleNamePage::setButtonEnabled(void)
{
	QString tempString = SampleNameLineEdit->text();
	if (!tempString.isEmpty() && (!view->selectionModel()->selectedIndexes().isEmpty())) {
		setButton->setEnabled(true);
		clearButton->setEnabled(true);
	}
	else {
		setButton->setEnabled(false);
		clearButton->setEnabled(false);
	}
}

void SampleNamePage::paintEvent(QPaintEvent *event)
{
	gridLayout->setContentsMargins(4,4,4,4);
}

void SampleNamePage::resizeEvent(QResizeEvent *event)
{
	gridLayout->setContentsMargins(4,4,4,4);
}
