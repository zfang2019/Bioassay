#include <QtGui>
#include <QModelIndex>

#include "methodDialog.h"
//#include <qmessagebox.h>

MethodWizard::MethodWizard(LayoutTableModel *model,QWidget *parent)
	: QWizard(parent)
{
	pModel= new LayoutTableModel(8,12);
	pView = new LayoutView(pModel);
	inputModel = model;
	
	/*for (int i = 0; i < model->rowCount(); ++i) {
		for (int j = 0; j < model->columnCount(); ++j) {
			QModelIndex index = pModel->index(i,j,QModelIndex());
			pModel->setData(index,model->data(index,Qt::DisplayRole),Qt::EditRole);
		}
	}*/
	pModel->CopyAll(model);
	this->setPage(0,new IntroPage);
	this->setPage(1,new GeneralPage);
	this->setPage(2,new SelectStdPage(pModel));
	this->setPage(3,new ConcPage);
	this->setPage(4,new DiluFactorPage(pModel));
	this->setPage(5,new SampleTypePage(pModel));
	this->setPage(6,new CutOffPage(pModel));


	this->setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));
    this->setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/background.png"));
	this->setWindowTitle(tr("Create A New Method"));
}

void MethodWizard::accept() 
{
	elisaName = field("elisaName").toString();
	catalogNumber = field("catalogNumber").toString();
	techName = field("techName").toString();
	standardCurveAlgorithm = field("StandardCurveAlgorithm").toInt()+1;



	QModelIndex assayNameIndex = pModel->index(1,1,QModelIndex());
	pModel->setData(assayNameIndex,elisaName,Qt::UserRole+5);

	QModelIndex catalogNumberIndex = pModel->index(2,1,QModelIndex());
	pModel->setData(catalogNumberIndex,catalogNumber,Qt::UserRole+5);
	
	QModelIndex algorithmIndex = pModel->index(3,1,QModelIndex());
	pModel->setData(algorithmIndex,standardCurveAlgorithm,Qt::UserRole+5);

	QModelIndex TechNameIndex = pModel->index(4,1,QModelIndex());
	pModel->setData(TechNameIndex,techName,Qt::UserRole+5);

	inputModel->CopyAll(pModel);
	
	/*
	QString tempDisplay;//debug purpose
	QModelIndexList tempList = inputModel->getStandardIndexList();
	for (int i = 0; i < tempList.size(); ++i) {
		QModelIndex index = tempList.at(i);
		tempDisplay += QString("%1%2\t%3\n").arg(QChar('A'+ index.column())).arg(index.row()+1).arg(inputModel->data(index, Qt::UserRole).toDouble());
	}
	QMessageBox::warning(0,tr("ELISA Bioo"),tempDisplay);*/ //for debug purpose
	
	return QDialog::accept();
}

StandardWizard::StandardWizard(LayoutTableModel *model,QWidget *parent)
	: QWizard(parent)
{
	pModel= new LayoutTableModel(8,12);
	pView = new LayoutView(pModel);
	inputModel = model;
	
	pModel->CopySampleName(model);
	pModel->CopyStandardIndexList(model);
	pModel->CopyStdConcs(model);
	//pModel->CopyAll(model);

	this->setPage(2,new SelectStdPage(pModel));
	this->setPage(3,new ConcPage);
	
	this->setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));
    this->setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/background.png"));
	this->setWindowTitle(tr("Set/Reset Standards"));
}

void StandardWizard::accept() 
{	
	pView->selectionModel()->reset();
	if (!pModel->hasStandardIndexList()) {
		QVariantList tempList = field("StdIndexList").toList();
		QModelIndexList stdIndexList;
	
		for (int i = 0; i < tempList.size(); ++i) {
			int tempRow = tempList.at(i).toString().at(1).digitValue()-1;
			int tempColumn = tempList.at(i).toString().at(0).unicode()-QChar('A').unicode();
			QModelIndex index = pModel->index(tempRow,tempColumn,QModelIndex());
			stdIndexList.append(index);
			QString tempString = QString(tr("StdConc_%1").arg(i));
			double tempConc = field(tempString).toString().toDouble();
			pModel->setData(index,tempConc,Qt::UserRole);
		}
		pModel->setStandardIndexList(stdIndexList);

		inputModel->CopyStdConcs(pModel);
		inputModel->CopyStandardIndexList(pModel);
		//inputModel->CopyAll(pModel);
	}
	return QDialog::accept();
}

DilutionFactorWizard::DilutionFactorWizard(LayoutTableModel *model,QWidget *parent)
	: QWizard(parent)
{
	pModel= new LayoutTableModel(8,12);
	pView = new LayoutView(pModel);
	inputModel = model;
	
	pModel->CopySampleName(model);
	pModel->CopyStandardIndexList(model);
	pModel->CopyStdConcs(model);
	pModel->CopyBGColorForDF(model);
	pModel->CopyDilutionFactors(model);
	//pModel->CopyAll(model);

	this->setPage(4,new DiluFactorPage(pModel));
	
	this->setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));
    this->setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/background.png"));
	this->setWindowTitle(tr("Set/Reset Standards"));
}

void DilutionFactorWizard::accept() 
{	
	inputModel->CopyBGColorForDF(pModel);
	inputModel->CopyDilutionFactors(pModel);
	return QDialog::accept();
}

SampleTypeWizard::SampleTypeWizard(LayoutTableModel *model,QWidget *parent)
	: QWizard(parent)
{
	pModel= new LayoutTableModel(8,12);
	pView = new LayoutView(pModel);
	inputModel = model;
	
	pModel->CopySampleName(model);
	pModel->CopyStandardIndexList(model);
	pModel->CopyStdConcs(model);
	pModel->CopyBGColorForSampleType(model);
	pModel->CopySampleTypes(model);
	//pModel->CopyAll(model);

	this->setPage(5,new SampleTypePage(pModel));
	
	this->setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));
    this->setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/background.png"));
	this->setWindowTitle(tr("Set/Reset Standards"));
}

void SampleTypeWizard::accept() 
{	
	inputModel->CopyBGColorForSampleType(pModel);
	inputModel->CopySampleTypes(pModel);
	return QDialog::accept();
}

CutOffValueWizard::CutOffValueWizard(LayoutTableModel *model,QWidget *parent)
	: QWizard(parent)
{
	pModel= new LayoutTableModel(8,12);
	pView = new LayoutView(pModel);
	inputModel = model;
	
	pModel->CopySampleName(model);
	pModel->CopyStandardIndexList(model);
	pModel->CopyStdConcs(model);
	pModel->CopyBGColorForCutOffValue(model);
	pModel->CopyCutOffValues(model);
	//pModel->CopyAll(model);

	this->setPage(6,new CutOffPage(pModel));
	this->setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));
    this->setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/background.png"));
	this->setWindowTitle(tr("Set/Reset Standards"));
}

void CutOffValueWizard::accept() 
{	
	inputModel->CopyBGColorForCutOffValue(pModel);
	inputModel->CopyCutOffValues(pModel);
	return QDialog::accept();
}

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Introduction"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark1.png"));

	label = new QLabel(tr("You will be asked to provide the following information:\n"
							"(1) The name of technician and the name of the ELISA assay\n"
							"(2) The layout of the standards in the plate\n"
							"(3) The concentrations of each corresponding standard\n"
							"(4) The dilution factor of each sample in the plate\n"
							"(5) The sample type of each sample in the plate.\n"
							"(6) Finally, the cut off value for each samples"));
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    setLayout(layout);
}

GeneralPage::GeneralPage(QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Assay Name and the Performer"));
    setSubTitle(tr("Please provide the name and catalog number of the ELISA assay and the name of assay performer"));
    setPixmap(QWizard::LogoPixmap, QPixmap(":/images/logo1.png"));

	elisaNameLabel = new QLabel(tr("<font color=red>*</font>ELISA Name:"),this);
	elisaNameLineEdit = new QLineEdit(this);
	this->lineEdits.append(elisaNameLineEdit);
	elisaNameLabel->setBuddy(elisaNameLineEdit);
	catalogNumberLabel = new QLabel(tr("<font color=red>*</font>ELISA Catalog No:"),this);
	catalogNumberLineEdit = new QLineEdit(this);
	this->lineEdits.append(catalogNumberLineEdit);
	catalogNumberLabel->setBuddy(catalogNumberLineEdit);
	techNameLabel = new QLabel(tr("  Technician Name:"),this);
	techNameLineEdit = new QLineEdit(this);
	this->lineEdits.append(techNameLineEdit);
	techNameLabel->setBuddy(techNameLineEdit);
	algorithmLabel = new QLabel(tr("<font color=red>*</font>Standard Curve Processing Method:"),this);
	algorithmComboBox = new QComboBox(this);
	QStringList algorithmList;
	algorithmList << "Linear Regression" << "Logit-Log Regression" << "Cubic Spline Regression";
	algorithmComboBox->insertItems(0,algorithmList);
	algorithmLabel->setBuddy(algorithmComboBox);
	requiredField = new QLabel(tr("<font color=red>* Required Field</font>"));

	gridLayout = new QGridLayout(this);
	gridLayout->addWidget(elisaNameLabel,0,0,1,1);
	gridLayout->addWidget(elisaNameLineEdit,0,1,1,1);
	gridLayout->addWidget(catalogNumberLabel,1,0,1,1);
	gridLayout->addWidget(catalogNumberLineEdit,1,1,1,1);
	gridLayout->addWidget(techNameLabel,2,0,1,1);
	gridLayout->addWidget(techNameLineEdit,2,1,1,1);
	gridLayout->addWidget(algorithmLabel,3,0,1,1);
	gridLayout->addWidget(algorithmComboBox,3,1,1,1);
	gridLayout->addWidget(requiredField,4,1,1,1);
	this->setLayout(gridLayout);
	registerField("elisaName", elisaNameLineEdit,"text",SIGNAL(textChanged()));
	registerField("catalogNumber", catalogNumberLineEdit,"text",SIGNAL(textChanged()));
	registerField("techName", techNameLineEdit,"text",SIGNAL(textChanged()));
	registerField("StandardCurveAlgorithm",algorithmComboBox,"currentIndex",SIGNAL(currentIndexChanged(int)));
	connect(elisaNameLineEdit,SIGNAL(textChanged(QString)),this,SIGNAL(completeChanged()));
	connect(catalogNumberLineEdit,SIGNAL(textChanged(QString)),this,SIGNAL(completeChanged()));
	connect(techNameLineEdit,SIGNAL(textChanged(QString)),this,SIGNAL(completeChanged()));
}

bool GeneralPage::isComplete() const
{
	if (	(!field("elisaName").toString().isEmpty())
		&&	(!field("catalogNumber").toString().isEmpty()) )
		//&&	(!field("techName").toString().isEmpty()) )
			return true;
	/*
	if (techNameLineEdit->text().isEmpty()) return false;
	if (elisaNameLineEdit->text().isEmpty()) return false;
	if (catalogNumberLineEdit->text().isEmpty()) return false;
	//QString temp = techNameLineEdit->text();
	//QMessageBox::warning(0,tr("ELISA Bioo"),temp);*/
	return false;
}

void GeneralPage::keyPressEvent(QKeyEvent *event)
{
	QLineEdit *temp = qobject_cast<QLineEdit*>(this->focusWidget());
	if (!temp) {
		QWidget::keyPressEvent(event);
		return;
	}
	int index = lineEdits.indexOf(temp);
	switch (event->key()) {
	case Qt::Key_Up:
		if (index>0) lineEdits.at(index-1)->setFocus();
		break;
	case Qt::Key_Down:
		if (index < (lineEdits.count()-1)) lineEdits.at(index+1)->setFocus();
		break;
	}
}

SelectStdPage::SelectStdPage(LayoutTableModel *pModel,QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Select A Set of Standards from the Plate"));
	this->setSubTitle(tr("Select at least 5 standards"));
	gridLayout = new QGridLayout(this);
    this->setLayout(gridLayout);
	pModel->changeBackGroundToNormal();
	view = new LayoutView(pModel);
	model = pModel;
	keepButton = new QPushButton(tr("Keep current standards"),this);
	resetButton = new QPushButton(tr("Create a new set of standards"),this);
	keepButton->setEnabled(model->hasStandardIndexList());
	resetButton->setEnabled(model->hasStandardIndexList());
    gridLayout->addWidget(view,0,0,9,13);
	gridLayout->addWidget(keepButton,9,2,1,4);
	gridLayout->addWidget(resetButton,9,7,1,4);
	for (int i = 0; i < gridLayout->rowCount(); ++i)
		gridLayout->setRowStretch(i,1);
	for (int i = 0; i < gridLayout->columnCount(); ++i)
		gridLayout->setColumnStretch(i,1);
	gridLayout->setContentsMargins(4,4,4,4);
	//qobject_cast<QVBoxLayout*>(layout())->setStretch(0,1);
	//qobject_cast<QVBoxLayout*>(layout())->setContentsMargins(1,1,1,1);
	
	QModelIndexList tempList = view->selectionModel()->selection().indexes();
	for (int i = 0; i < tempList.size(); ++i) 
		stdIndexList.append(QString("%1%2").arg(QChar('A')+tempList.at(i).column()).arg(tempList.at(i).row()+1));
	//qRegisterMetaType<QModelIndex>();
	registerField("StdIndexList", this,"StdIndexList",SIGNAL(StdIndexListChanged()));
	
	connect(view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(UpdateStdIndexList()));
	connect(this,SIGNAL(StdIndexListChanged()),this,SIGNAL(completeChanged()));
	connect(keepButton,SIGNAL(clicked(bool)),this,SLOT(keepCurrentStandard(void)));
	connect(resetButton,SIGNAL(clicked(bool)),this,SLOT(clearCurrentStandard(void)));
}

void SelectStdPage::initializePage() 
{
	model->changeBackGroundToNormal();
	view->selectionModel()->reset();
}

void SelectStdPage::cleanupPage()
{
	model->changeBackGroundToNormal();
	view->selectionModel()->reset();
}

int SelectStdPage::nextId() const
{
	if (this->model->hasStandardIndexList() ) {
		return 4;//go to diluFactorPage directly
	}
	else return 3;//go to concPage
}

void SelectStdPage::paintEvent(QPaintEvent *event)
{
	gridLayout->setContentsMargins(4,4,4,4);
}

void SelectStdPage::resizeEvent(QResizeEvent *event)
{
	/*QRect rect = this->geometry();
	rect.adjust(2,2,-2,-2);
	gridLayout->setGeometry(rect);
	gridLayout->setAlignment(Qt::AlignTop|Qt::AlignLeft);*/
	gridLayout->setContentsMargins(4,4,4,4);
}

void SelectStdPage::keepCurrentStandard(void)
{
	this->keepButton->setEnabled(false);
	this->resetButton->setEnabled(false);
	if (!this->wizard()->hasVisitedPage(1)) this->setFinalPage(true);
	emit completeChanged();
}
		
void SelectStdPage::clearCurrentStandard(void)
{
	this->model->clearStandardIndexList();
	this->keepButton->setEnabled(false);
	this->resetButton->setEnabled(false);
	completeChanged();
}

QStringList SelectStdPage::StdIndexList()
{
	return stdIndexList;
}
void SelectStdPage::UpdateStdIndexList()
{
	QModelIndexList tempList = view->selectionModel()->selection().indexes();
	stdIndexList.clear();
	for (int i = 0; i < tempList.size(); ++i) 
		stdIndexList.append(QString("%1%2").arg(QChar('A'+tempList.at(i).column())).arg(tempList.at(i).row()+1));
	emit StdIndexListChanged();
}

bool SelectStdPage::isComplete() const
{
	if (this->model->hasStandardIndexList() && !this->keepButton->isEnabled() ) return true;
	QVariantList tempList = field("StdIndexList").toList();
	if ( tempList.size() > 4) return true;
	return false;
}

ConcPage::ConcPage(QWidget *parent)
	:QWizardPage(parent)
{
	setTitle(tr("Give Each Standard A Concentration"));
	cellPositionHeader = new QLabel(tr("The standard's position"),this);
	//cellPositionHeader->setWordWrap(true);
	concHeader = new QLabel(tr("The standard's concentration"),this);
	//concHeader->setWordWrap(true);

	gridLayout = new QGridLayout(this);
	gridLayout->addWidget(cellPositionHeader,0,0,1,1,Qt::AlignCenter);
	gridLayout->addWidget(concHeader,0,1,1,1,Qt::AlignCenter);

	warningLabel = new QLabel(tr(""),this);
	this->setLayout(gridLayout);
	//registerField("StdIndexList", this,"StdIndexList",SIGNAL(StdIndexListChanged(QItemSelection,QItemSelection)));
	
	//connect(view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(UpdateStdIndexList()));
	//connect(this,SIGNAL(StdIndexListChanged()),this,SIGNAL(completeChanged()));
}

void ConcPage::initializePage()
{
	QVariantList tempList = field("StdIndexList").toList();
	QStringList cellPositionList;
	for (int i = 0; i < tempList.size(); ++i) cellPositionList.append(tempList.at(i).toString());
	
	for (int i=0; i < cellPositionList.size();++i) {
		cellPositions.append(new QLabel(cellPositionList.at(i),this));
		cellPositions.at(i)->setVisible(true);

		concLineEdits.append(new QLineEdit(this));
		concLineEdits.at(i)->setVisible(true);
		concLineEdits.at(i)->setAcceptDrops(true);
		concLineEdits.at(i)->setDragEnabled(true);
		concLineEdits.at(i)->setFocusPolicy(Qt::WheelFocus);
		if (i>0) concLineEdits.at(i)->setTabOrder(concLineEdits.at(i-1),concLineEdits.at(i));
		cellPositions.at(i)->setBuddy(concLineEdits.at(i));
		
		gridLayout->addWidget(cellPositions.at(i),i+1,0,1,1,Qt::AlignCenter);
		gridLayout->addWidget(concLineEdits.at(i),i+1,1,1,1,Qt::AlignCenter);
		
		registerField(QString(tr("StdConc_%1").arg(i)), concLineEdits.at(i),"text",SIGNAL(textChanged()));
		connect(concLineEdits.at(i),SIGNAL(textChanged(QString)),this,SIGNAL(completeChanged()));
	}	
	gridLayout->addWidget(warningLabel,tempList.size()+1,0,1,2,Qt::AlignHCenter|Qt::AlignBottom);
	if (concLineEdits.size() > 0) this->setFocusProxy(concLineEdits.at(0));
	gridLayout->setSpacing(10);
	//QMessageBox::warning(0,tr("Test"),QString(tr("Number of standards selected:%1").arg(cellPositionList.size())));
}

bool ConcPage::isComplete() const
{
	warningLabel->clear();

	QVariantList tempList = field("StdIndexList").toList();
	for ( int i = 0; i < tempList.size(); ++i) {
		bool ok;
		QString tempString = QString(tr("StdConc_%1").arg(i));
		if (field(tempString).toString().isEmpty()) return false;
		field(tempString).toDouble(&ok);
		if (!ok) {
			warningLabel->setText(tr("<font color=red>Warning: The concentration should be a number.</font>"));
			warningLabel->show();
			return false;
		}
	}

	QList<double> assignedConcs;
	for (int i = 0; i < tempList.size(); ++i) {
		QString tempString = QString(tr("StdConc_%1").arg(i));
		assignedConcs.append(field(tempString).toDouble());
	}
	qSort(assignedConcs.begin(),assignedConcs.end());//increasing order of concentrations
	
	//handle situation when there is no negative standards
	if (assignedConcs.at(0) != 0) {
		warningLabel->setText(tr("<font color=red>Warning: There should exist a negative control.</font>"));
		warningLabel->show();
		return false;
	}

	//calculated how many standards in the ELISA assay
	int std_count = 1;
	for (int i = 1; i < assignedConcs.size(); ++i) 
		if (assignedConcs.at(i) > assignedConcs.at(i-1)) std_count += 1;
		
	if (std_count < 2) {
		warningLabel->setText(tr("<font color=red>Warning: There should exist at least two standards.</font>"));
		warningLabel->show();
		return false;//fewer than 2 standards
	}
	return true;
}

void ConcPage::cleanupPage()
{
	for (int i = 0; i < cellPositions.size(); ++i) delete cellPositions.at(i);
	for (int i = 0; i < concLineEdits.size(); ++i) delete concLineEdits.at(i);
	cellPositions.clear();
	concLineEdits.clear();
	QStringList tempList;
	tempList.clear();
	this->setField("StdIndexList",tempList);
}

void ConcPage::paintEvent(QPaintEvent *event)
{
	gridLayout->setContentsMargins(10,10,10,10);
	gridLayout->setAlignment(Qt::AlignCenter);
	for (int i = 0; i < cellPositions.size();++i) {
		cellPositions.at(i)->show();
		concLineEdits.at(i)->show();
	}
}

void ConcPage::resizeEvent(QResizeEvent *event)
{
	gridLayout->setContentsMargins(10,10,10,10);
	gridLayout->setAlignment(Qt::AlignCenter);
	for (int i = 0; i < cellPositions.size();++i) {
		cellPositions.at(i)->show();
		concLineEdits.at(i)->show();
	}
}

void ConcPage::keyPressEvent(QKeyEvent *event)
{
	QLineEdit *temp = qobject_cast<QLineEdit*>(this->focusWidget());
	if (!temp) {
		QWidget::keyPressEvent(event);
		return;
	}
	int index = concLineEdits.indexOf(temp);
	switch (event->key()) {
	case Qt::Key_Up:
		if (index>0) concLineEdits.at(index-1)->setFocus();
		break;
	case Qt::Key_Down:
		if (index < (concLineEdits.count()-1)) concLineEdits.at(index+1)->setFocus();
		break;
	}
}

DiluFactorPage::DiluFactorPage(LayoutTableModel *pModel,QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Set different dilution factors to different samples"));
	//this->setSubTitle(tr("Select sets of samples with same dilution factors and assign a factor and on and on ..."));
	view = new LayoutView(pModel,this);
	model = pModel;
	view->update();

	view->selectionModel()->reset();
	diluFactorLabel = new QLabel(tr("Set Dilution Factor As:"),this);
	diluFactorLineEdit = new QLineEdit(this);
	diluFactorLabel->setBuddy(diluFactorLineEdit);
	setButton = new QPushButton(tr("Set"),this);
	setButton->setEnabled(false);
	clearButton = new QPushButton(tr("Clear"),this);
	clearButton->setEnabled(false);
	gridLayout = new QGridLayout(this);
	setLayout(gridLayout);

	gridLayout->addWidget(view,0,0,9,13);
	gridLayout->addWidget(diluFactorLabel,9,0,1,3);
	gridLayout->addWidget(diluFactorLineEdit,9,3,1,4);
	gridLayout->addWidget(setButton,9,7,1,3);
	gridLayout->addWidget(clearButton,9,10,1,3);
	
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

	connect(diluFactorLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setButtonEnabled()));
	connect(setButton,SIGNAL(clicked(bool)),this,SLOT(setDiluFactors(void)));
	connect(clearButton,SIGNAL(clicked(bool)),this,SLOT(clearSelectionAndDiluFactor(void)));
	connect(view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(setButtonEnabled()));
	//this->update();
	//connect(view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(UpdateStdIndexList()));
	//connect(this,SIGNAL(StdIndexListChanged()),this,SIGNAL(completeChanged()));*/
}

void  DiluFactorPage::initializePage()
{
	model->changeBackGroundToDF();
	view->selectionModel()->reset();
	if (model->hasStandardIndexList()) return;

	QVariantList tempList = field("StdIndexList").toList();
	QModelIndexList *stdIndexList = new QModelIndexList;
	
	for (int i = 0; i < tempList.size(); ++i) {
		int tempRow = tempList.at(i).toString().at(1).digitValue()-1;
		int tempColumn = tempList.at(i).toString().at(0).unicode()-QChar('A').unicode();
		QModelIndex index = model->index(tempRow,tempColumn,QModelIndex());
		stdIndexList->append(index);
		QString tempString = QString(tr("StdConc_%1").arg(i));
		double tempConc = field(tempString).toString().toDouble();
		model->setData(index,tempConc,Qt::UserRole);
	}
	model->setStandardIndexList(*stdIndexList);
}

void  DiluFactorPage::cleanupPage()
{
	model->changeBackGroundToNormal();
	view->selectionModel()->reset();
}

void DiluFactorPage::setDiluFactors(void)
{
	double tempDouble = diluFactorLineEdit->text().toDouble();
	if (bgColor.isEmpty()) {
		QColor tempColor1 = Qt::green;
		QColor tempColor2 = tempColor1.lighter(110);
		bgColor.append(tempColor2);
	}
	else {
		QColor tempColor = bgColor.at(bgColor.size()-1).lighter(110);
		bgColor.append(tempColor);
	}
	
	for (int i = 0; i < view->selectionModel()->selectedIndexes().size(); ++i) {
		QModelIndex index = view->selectionModel()->selectedIndexes().at(i);
		view->model()->setData(index,tempDouble,(Qt::UserRole+1));
		view->model()->setData(index,bgColor.at(bgColor.size()-1),Qt::BackgroundRole);
	}
	this->diluFactorLineEdit->setText("");// or this->diluFactorLineEdit->clear();
	view->selectionModel()->reset();
}

void DiluFactorPage::clearSelectionAndDiluFactor(void)
{
	this->diluFactorLineEdit->setText("");// or this->diluFactorLineEdit->clear();
	view->selectionModel()->reset();
}

void DiluFactorPage::setButtonEnabled(void)
{
	bool ok;
	diluFactorLineEdit->text().toDouble(&ok);
	if (ok && (!view->selectionModel()->selectedIndexes().isEmpty())) {
		setButton->setEnabled(true);
		clearButton->setEnabled(true);
	}
	else {
		setButton->setEnabled(false);
		clearButton->setEnabled(false);
	}
}

void DiluFactorPage::paintEvent(QPaintEvent *event)
{
	gridLayout->setContentsMargins(4,4,4,4);
}

void DiluFactorPage::resizeEvent(QResizeEvent *event)
{
	/*QRect rect = this->geometry();
	rect.adjust(2,2,-2,-2);
	gridLayout->setGeometry(rect);
	gridLayout->setAlignment(Qt::AlignTop|Qt::AlignLeft);*/
	gridLayout->setContentsMargins(4,4,4,4);
}


SampleTypePage::SampleTypePage(LayoutTableModel *pModel,QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Set different sample types to different samples"));
	//this->setSubTitle(tr("Select sets of samples with same dilution factors and assign a factor and on and on ..."));
	view = new LayoutView(pModel,this);
	model = pModel;
	view->update();

	view->selectionModel()->reset();
	SampleTypeLabel = new QLabel(tr("Set Sample Type As:"),this);
	SampleTypeLineEdit = new QLineEdit(this);
	SampleTypeLabel->setBuddy(SampleTypeLineEdit);
	setButton = new QPushButton(tr("Set"),this);
	setButton->setEnabled(false);
	clearButton = new QPushButton(tr("Clear"),this);
	clearButton->setEnabled(false);
	gridLayout = new QGridLayout(this);
	setLayout(gridLayout);

	gridLayout->addWidget(view,0,0,9,13);
	gridLayout->addWidget(SampleTypeLabel,9,0,1,3);
	gridLayout->addWidget(SampleTypeLineEdit,9,3,1,4);
	gridLayout->addWidget(setButton,9,7,1,3);
	gridLayout->addWidget(clearButton,9,10,1,3);
	
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

	connect(SampleTypeLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setButtonEnabled()));
	connect(setButton,SIGNAL(clicked(bool)),this,SLOT(setSampleType(void)));
	connect(clearButton,SIGNAL(clicked(bool)),this,SLOT(clearSelectionAndSampleType(void)));
	connect(view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(setButtonEnabled()));
	//this->update();
	//connect(view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(UpdateStdIndexList()));
	//connect(this,SIGNAL(StdIndexListChanged()),this,SIGNAL(completeChanged()));*/
}

void  SampleTypePage::initializePage()
{	
	model->changeBackGroundToSampleType();
	view->selectionModel()->reset();
	if (model->hasStandardIndexList()) return;

	QVariantList tempList = field("StdIndexList").toList();
	QModelIndexList *stdIndexList = new QModelIndexList;
	for (int i = 0; i < tempList.size(); ++i) {
		int tempRow = tempList.at(i).toString().at(1).digitValue()-1;
		int tempColumn = tempList.at(i).toString().at(0).unicode()-QChar('A').unicode();
		QModelIndex index = model->index(tempRow,tempColumn,QModelIndex());
		stdIndexList->append(index);
		QString tempString = QString(tr("StdConc_%1").arg(i));
		double tempConc = field(tempString).toString().toDouble();
		model->setData(index,tempConc,Qt::UserRole);
	}
	model->setStandardIndexList(*stdIndexList);
}

void  SampleTypePage::cleanupPage()
{
	model->changeBackGroundToDF();
	view->selectionModel()->reset();
}

void SampleTypePage::setSampleType(void)
{
	QString tempString = SampleTypeLineEdit->text();
	if (bgColor.isEmpty()) {
		QColor tempColor1 = Qt::yellow;
		QColor tempColor2 = tempColor1.lighter(110);
		bgColor.append(tempColor2);
	}
	else {
		QColor tempColor = bgColor.at(bgColor.size()-1).lighter(110);
		bgColor.append(tempColor);
	}
	
	for (int i = 0; i < view->selectionModel()->selectedIndexes().size(); ++i) {
		QModelIndex index = view->selectionModel()->selectedIndexes().at(i);
		view->model()->setData(index,tempString,(Qt::UserRole+2));
		view->model()->setData(index,bgColor.at(bgColor.size()-1),Qt::BackgroundRole);
	}
	this->SampleTypeLineEdit->setText("");// or this->diluFactorLineEdit->clear();
	view->selectionModel()->reset();
}

void SampleTypePage::clearSelectionAndSampleType(void)
{
	this->SampleTypeLineEdit->setText("");// or this->diluFactorLineEdit->clear();
	view->selectionModel()->reset();
}

void SampleTypePage::setButtonEnabled(void)
{
	QString tempString = SampleTypeLineEdit->text();
	if (!tempString.isEmpty() && (!view->selectionModel()->selectedIndexes().isEmpty())) {
		setButton->setEnabled(true);
		clearButton->setEnabled(true);
	}
	else {
		setButton->setEnabled(false);
		clearButton->setEnabled(false);
	}
}

void SampleTypePage::paintEvent(QPaintEvent *event)
{
	gridLayout->setContentsMargins(4,4,4,4);
}

void SampleTypePage::resizeEvent(QResizeEvent *event)
{
	gridLayout->setContentsMargins(4,4,4,4);
}

CutOffPage::CutOffPage(LayoutTableModel *pModel,QWidget *parent)
	: QWizardPage(parent)
{
	setTitle(tr("Set different cut off values to different samples"));
	//this->setSubTitle(tr("Select sets of samples with same dilution factors and assign a factor and on and on ..."));
	view = new LayoutView(pModel,this);
	model = pModel;
	view->update();

	view->selectionModel()->reset();
	CutOffLabel = new QLabel(tr("Set Cut Off Value As:"),this);
	CutOffLineEdit = new QLineEdit(this);
	CutOffLabel->setBuddy(CutOffLineEdit);
	setButton = new QPushButton(tr("Set"),this);
	setButton->setEnabled(false);
	clearButton = new QPushButton(tr("Clear"),this);
	clearButton->setEnabled(false);
	gridLayout = new QGridLayout(this);
	setLayout(gridLayout);

	gridLayout->addWidget(view,0,0,9,13);
	gridLayout->addWidget(CutOffLabel,9,0,1,3);
	gridLayout->addWidget(CutOffLineEdit,9,3,1,4);
	gridLayout->addWidget(setButton,9,7,1,3);
	gridLayout->addWidget(clearButton,9,10,1,3);
	
	for (int i = 0; i < gridLayout->rowCount(); ++i)
		gridLayout->setRowStretch(i,1);
	for (int i = 0; i < gridLayout->columnCount(); ++i)
		gridLayout->setColumnStretch(i,1);
	gridLayout->setContentsMargins(4,4,4,4);

	connect(CutOffLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setButtonEnabled()));
	connect(setButton,SIGNAL(clicked(bool)),this,SLOT(setCutOffValues(void)));
	connect(clearButton,SIGNAL(clicked(bool)),this,SLOT(clearSelectionAndCutOffValue(void)));
	connect(view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(setButtonEnabled()));
}

void  CutOffPage::initializePage()
{
	model->changeBackGroundToCutOffValue();
	view->selectionModel()->reset();
	if (model->hasStandardIndexList()) return;

	QVariantList tempList = field("StdIndexList").toList();
	QModelIndexList *stdIndexList = new QModelIndexList;
	
	for (int i = 0; i < tempList.size(); ++i) {
		int tempRow = tempList.at(i).toString().at(1).digitValue()-1;
		int tempColumn = tempList.at(i).toString().at(0).unicode()-QChar('A').unicode();
		QModelIndex index = model->index(tempRow,tempColumn,QModelIndex());
		stdIndexList->append(index);
		QString tempString = QString(tr("StdConc_%1").arg(i));
		double tempConc = field(tempString).toString().toDouble();
		model->setData(index,tempConc,Qt::UserRole);
	}
	model->setStandardIndexList(*stdIndexList);
}

void  CutOffPage::cleanupPage()
{
	model->changeBackGroundToSampleType();
	view->selectionModel()->reset();
}

void CutOffPage::setCutOffValues(void)
{
	double tempDouble = CutOffLineEdit->text().toDouble();
	if (bgColor.isEmpty()) {
		QColor tempColor1 = Qt::blue;
		QColor tempColor2 = tempColor1.lighter(110);
		bgColor.append(tempColor2);
	}
	else {
		QColor tempColor = bgColor.at(bgColor.size()-1).lighter(110);
		bgColor.append(tempColor);
	}
	
	for (int i = 0; i < view->selectionModel()->selectedIndexes().size(); ++i) {
		QModelIndex index = view->selectionModel()->selectedIndexes().at(i);
		view->model()->setData(index,tempDouble,(Qt::UserRole+3));
		view->model()->setData(index,bgColor.at(bgColor.size()-1),Qt::BackgroundRole);
	}
	this->CutOffLineEdit->setText("");// or this->diluFactorLineEdit->clear();
	view->selectionModel()->reset();
}

void CutOffPage::clearSelectionAndCutOffValue(void)
{
	this->CutOffLineEdit->setText("");// or this->diluFactorLineEdit->clear();
	view->selectionModel()->reset();
}

void CutOffPage::setButtonEnabled(void)
{
	bool ok;
	CutOffLineEdit->text().toDouble(&ok);
	if (ok && (!view->selectionModel()->selectedIndexes().isEmpty())) {
		setButton->setEnabled(true);
		clearButton->setEnabled(true);
	}
	else {
		setButton->setEnabled(false);
		clearButton->setEnabled(false);
	}
}

void CutOffPage::paintEvent(QPaintEvent *event)
{
	gridLayout->setContentsMargins(4,4,4,4);
}

void CutOffPage::resizeEvent(QResizeEvent *event)
{
	gridLayout->setContentsMargins(4,4,4,4);
}