#include "MyView.h"
#include <QtGui>


//implemetation for ODView
ODView::ODView(ODTableModel *model, QWidget* parent):QWidget(parent)
{
	od_model = model;
	rows = od_model->rowCount();
	columns = od_model->columnCount();
	isModified = false;
	view = new QTableView(this);
	view->setModel(od_model);

	view->setGridStyle(Qt::SolidLine);
	view->setShowGrid(true);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

ODTableModel* ODView::model() const
{
	return od_model;
}

void ODView::setSelectionModel(QItemSelectionModel* selectionModel)
{
	view->setSelectionModel(selectionModel);
}

QItemSelectionModel* ODView::selectionModel()
{
	return view->selectionModel();
}


void ODView::copy()
{
	QModelIndexList selectedIndex = view->selectionModel()->selectedIndexes();
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
			QModelIndex index = od_model->index(i,j,QModelIndex());
			str += od_model->data(index,Qt::DisplayRole).toString();
		}
	}
	QApplication::clipboard()->setText(str);
	//QMessageBox::warning(this, tr("copy result for subclassed OD View"),str);
}
	
void ODView::paste()
{
	QModelIndexList selectedIndex = view->selectionModel()->selectedIndexes();
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

	QString str = QApplication::clipboard()->text();
	QStringList rows = str.split('\n');
	int numRows = rows.count();
	int numColumns = rows.first().count('\t') + 1;

	int row = rowMin;
	int column = columnMin;

	for (int i = 0; i < numRows; ++i) {
		QStringList columns = rows[i].split('\t');
		for (int j = 0; j < numColumns; ++j) {
			QModelIndex index = od_model->index(rowMin+i,column+j,QModelIndex());
			if (selectedIndex.contains(index)) od_model->setData(index,columns[j],Qt::EditRole);
		}
	}
	
}

void ODView::Delete()
{
	QModelIndexList selectedIndex = view->selectionModel()->selectedIndexes();
	
	for (int i = 0; i < selectedIndex.size(); ++i) {
		QModelIndex index = selectedIndex.at(i);
		int row = index.row();
		int column = index.column();
		QModelIndex od_index = od_model->index(row,column,QModelIndex());
		od_model->setData(od_index,0,Qt::EditRole);   
	}
}


void ODView::getODValue(double* returnValue)
{
	for (int i = 0; i < columns; ++i)
		for (int j = 0; j<rows; ++j) {
			QModelIndex index = od_model->index(j,i,QModelIndex());
			returnValue[i*columns+j] = od_model->data(index,Qt::DisplayRole).toDouble();
		}
}

void ODView::setODValue(double *inputValue) 
{
	double tempOD = 0;
	for (int i = 0; i < columns; ++i)
		for (int j = 0; j<rows; ++j) {
			QModelIndex index = od_model->index(j,i,QModelIndex());
			tempOD = inputValue[i*columns+j];
			od_model->setData(index,tempOD,Qt::EditRole);	
		}
	//emit modified();
}

void ODView::paintEvent(QPaintEvent *event){
	view->resize(this->size());
	for (int i=0; i<columns; ++i) view->setColumnWidth(i,(view->width()- view->verticalHeader()->width())/columns);
	for (int i=0; i< rows; ++i) view->setRowHeight(i,((view->height()-view->horizontalHeader()->height())/rows));
	view->horizontalHeader()->setStretchLastSection(true);
	view->verticalHeader()->setStretchLastSection(true);
	view->setGridStyle(Qt::SolidLine);
	view->setShowGrid(true);
}

void ODView::resizeEvent(QResizeEvent *event){
	view->resize(this->size());
	for (int i=0; i<columns; ++i) view->setColumnWidth(i,(view->width()- view->verticalHeader()->width())/columns);
	for (int i=0; i< rows; ++i) view->setRowHeight(i,((view->height()-view->horizontalHeader()->height())/rows));
	view->horizontalHeader()->setStretchLastSection(true);
	view->verticalHeader()->setStretchLastSection(true);
	view->setGridStyle(Qt::SolidLine);
	view->setShowGrid(true);
}

ODView::~ODView()
{
}




/* ######################################################################
   ######################################################################
   ######################################################################
   ######################################################################
*/

//implemetation for LayoutView
LayoutView::LayoutView(LayoutTableModel *model, QWidget* parent):QTableView(parent){
	plate_model = model;
	rows = plate_model->rowCount();
	columns = plate_model->columnCount();
	isModified = false;
	view = new QTableView(this);
	view->setModel(plate_model);
	
	view->setGridStyle(Qt::SolidLine);
	view->setShowGrid(true);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

LayoutTableModel* LayoutView::model() const
{
	return plate_model;
}

QItemSelectionModel* LayoutView::selectionModel()
{
	return view->selectionModel();
}

void LayoutView::setSelectionModel(QItemSelectionModel* selectionModel)
{
	view->setSelectionModel(selectionModel);
}

void LayoutView::copy()
{
	QModelIndexList selectedIndex = view->selectionModel()->selectedIndexes();
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
			QModelIndex index = plate_model->index(i,j,QModelIndex());
			str += plate_model->data(index,Qt::DisplayRole).toString();
		}
	}
	QApplication::clipboard()->setText(str);
	//QMessageBox::warning(this, tr("copy result for subclassed OD View"),str);
}
	
void LayoutView::paste()
{
	QModelIndexList selectedIndex = view->selectionModel()->selectedIndexes();
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

	QString str = QApplication::clipboard()->text();
	QStringList rows = str.split('\n');
	int numRows = rows.count();
	int numColumns = rows.first().count('\t') + 1;

	int row = rowMin;
	int column = columnMin;

	for (int i = 0; i < numRows; ++i) {
		QStringList columns = rows[i].split('\t');
		for (int j = 0; j < numColumns; ++j) {
			QModelIndex index = plate_model->index(rowMin+i,column+j,QModelIndex());
			if (selectedIndex.contains(index)) plate_model->setData(index,columns[j],Qt::EditRole);
		}
	}
	
}

void LayoutView::Delete()
{
	QModelIndexList selectedIndex = view->selectionModel()->selectedIndexes();
	
	for (int i = 0; i < selectedIndex.size(); ++i) {
		QModelIndex index = selectedIndex.at(i);
		int row = index.row();
		int column = index.column();
		QModelIndex plate_index = plate_model->index(row,column,QModelIndex());
		plate_model->setData(plate_index,QString("%1%2").arg(QChar('A'+ column)).arg(row+1),Qt::EditRole);   
	}
}


void LayoutView::getSampleName(QStringList *returnName)
{
	for (int i = 0; i < columns; ++i)
		for (int j = 0; j<rows; ++j) {
			QModelIndex index = plate_model->index(j,i,QModelIndex());
			returnName->replace(i*columns+j,plate_model->data(index,Qt::DisplayRole).toString());
		}
}

void LayoutView::setSampleName(QStringList *inputName) 
{
	QString tempString;
	for (int i = 0; i < columns; ++i)
		for (int j = 0; j<rows; ++j) {
			QModelIndex index = plate_model->index(j,i,QModelIndex());
			tempString = inputName->at(i*columns+j);
			plate_model->setData(index,tempString,Qt::EditRole);
		}
}

void LayoutView::paintEvent(QPaintEvent *event){
	view->resize(this->size());
	for (int i=0; i<columns; ++i) view->setColumnWidth(i,(view->width()- view->verticalHeader()->width())/columns);
	for (int i=0; i< rows; ++i) view->setRowHeight(i,((view->height()-view->horizontalHeader()->height())/rows));
	view->horizontalHeader()->setStretchLastSection(true);
	view->verticalHeader()->setStretchLastSection(true);
	view->setGridStyle(Qt::SolidLine);
	view->setShowGrid(true);
}

void LayoutView::resizeEvent(QResizeEvent *event)
{
	view->resize(this->size());
	for (int i=0; i<columns; ++i) view->setColumnWidth(i,(view->width()- view->verticalHeader()->width())/columns);
	for (int i=0; i< rows; ++i) view->setRowHeight(i,((view->height()-view->horizontalHeader()->height())/rows));
	view->horizontalHeader()->setStretchLastSection(true);
	view->verticalHeader()->setStretchLastSection(true);
	view->setGridStyle(Qt::SolidLine);
	view->setShowGrid(true);
}

LayoutView::~LayoutView()
{
}
