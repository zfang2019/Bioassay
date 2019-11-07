#include "TableModel.h"
#include <QtGui>

//implemetation for LayoutTableModel
ODTableModel::ODTableModel(int row_number, int column_number, QObject *parent):QAbstractTableModel(parent) {
	rows = row_number;
	columns = column_number;
	ODValues = new double[rows*columns];
	for (int row = 0; row < row_number; ++row)
		for (int column = 0; column < column_number; ++column)
			ODValues[row*columns+column] = 0.0000;
	for (int i = 0; i < column_number; ++i) 
		horizonal.append(QString(QChar('A'+ i)));
	for (int i = 0; i < row_number; ++i)
		vertical.append(QString("%1").arg(i+1));
}

int ODTableModel::rowCount(const QModelIndex &parent) const{
	return rows;
}

int ODTableModel::columnCount(const QModelIndex &parent) const{
	return columns;
}

QVariant ODTableModel::data(const QModelIndex &index, int role) const{
	if (!index.isValid())
		return QVariant();
	if (index.row() >=rows ||index.column()>=columns)
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole){
		QString returnValue;
		returnValue.sprintf("%.4f",ODValues[index.row()*columns+index.column()]);
		return returnValue;
	}
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignCenter;
	if (role == Qt::BackgroundRole) {
		if (index.row()%2) return QBrush(Qt::white);
		else return QBrush(QColor(250,200,200));//(Qt::white);
	}
	if (role == Qt::FontRole) {
		 QFont viewFont;
		 viewFont.setBold(true);
         //boldFont.setBold(true);
         return viewFont;
	}
	if (role == Qt::StatusTipRole || role == Qt::ToolTipRole)
		return QString("OD at Cell %1%2: %3").arg(QChar('A'+ index.column())).arg(index.row()+1).arg(ODValues[index.row()*columns+index.column()]);
	else return QVariant();
}

QVariant ODTableModel::headerData(int section, Qt::Orientation orientation, int role) const{
	//if (role != Qt::DisplayRole)return QVariant();
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) 
			return horizonal.at(section);
		else return vertical.at(section);
	}
	else if (role == Qt::FontRole) {
		 QFont boldFont;
         boldFont.setBold(true);
         return boldFont;
	}
	else return QVariant();
}

Qt::ItemFlags ODTableModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;
	return QAbstractTableModel::flags(index)|Qt::ItemIsEditable;
}

bool ODTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() && role == Qt::EditRole) {
		this->ODValues[index.row()*columns+index.column()] = value.toDouble();
		emit dataChanged(index,index);
		emit ODValueChanged(index,value.toDouble());
		return true;
	}
	return false;
}

bool ODTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,int role){
	if (role != Qt::EditRole) return false;

	if (orientation == Qt::Horizontal) {
		if (section<0 || section > this->columnCount()) return false;
		else {
			horizonal.replace(section,value.toString());
			emit headerDataChanged(orientation,section,section);
			return true;
		}
	}
	
	else {
		if (section<0 || section > this->rowCount()) return false;
		else {
			vertical.replace(section,value.toString());
			emit headerDataChanged(orientation,section,section);
			return true;
		}
	}

	return false;
}

void ODTableModel::updateODValuesFromPlateModel(double* ODs)
{
   for (int i = 0; i < rows; ++i)
	   for (int j = 0; j < columns; ++j) {
		   QModelIndex index = this->index(i,j,QModelIndex());
		   this->setData(index,ODs[i*columns+j],Qt::EditRole);
	   }
}

ODTableModel::~ODTableModel()
{
}

/* ######################################################################
   ######################################################################
   ######################################################################
   ######################################################################
*/

//implemetation for LayoutTableModel
LayoutTableModel::LayoutTableModel(int row_number, int column_number, QObject *parent):QAbstractTableModel(parent) {
	rows = row_number;
	columns = column_number;
	standardIndexList.clear();
	sampleNames = new QString[rows*columns];
	stdConcs = new double[rows*columns];
	ODValues = new double[rows*columns];
	generalAssayInfo = new QString[rows*columns];
	dilutionFactors = new double[rows*columns];
	sampleTypes = new QString[rows*columns];
	this->cutOffValues = new double[rows*columns];
	this->bgColor = 0;//new QColor[rows*columns];
	this->bgColorForDF = new QColor[rows*columns];
	this->bgColorForSampleType =  new QColor[rows*columns];
	this->bgColorForCutOffValue = new QColor[rows*columns];
	for (int row = 0; row < row_number; ++row)
		for (int column = 0; column < column_number; ++column) {
			sampleNames[row*columns+column] = QString("%1%2").arg(QChar('A'+ column)).arg(row+1);//QString("BLK");
			stdConcs[row*columns+column] = -1;
			dilutionFactors[row*columns+column] = 1;
			sampleTypes[row*columns+column] = QString(tr("Generic"));
			//bgColor[row*columns+column] = Qt::white;
			cutOffValues[row*columns+column] = 0;
			this->ODValues[row*columns+column] = 0.0000;
			this->generalAssayInfo[row*columns+column] = QString(tr("N/A"));
			bgColorForDF[row*columns+column] = Qt::white;
			bgColorForSampleType[row*columns+column] = Qt::white;
			bgColorForCutOffValue[row*columns+column] = Qt::white;
		}

	for (int i = 0; i < column_number; ++i) 
		horizonal.append(QString(QChar('A'+ i)));
	for (int i = 0; i < row_number; ++i)
		vertical.append(QString("%1").arg(i+1));
}

void LayoutTableModel::setStandardIndexList(QModelIndexList stdIndexList)
{
	QModelIndexList tempList;
	for (int i = 0; i < this->standardIndexList.size(); ++i) 
		tempList.append(this->standardIndexList.at(i));
	this->standardIndexList.clear();

	for (int i = 0; i <tempList.size(); ++i) {
		QModelIndex index = tempList.at(i);
		this->setData(index,QString("%1%2").arg(QChar('A'+ index.column())).arg(index.row()+1),Qt::EditRole);
		//this->setData(index,Qt::white,Qt::BackgroundRole);
		this->setData(index,-1,Qt::UserRole);//set old std concentrations to -1
		this->setData(index,1,Qt::UserRole+1);//set old std dilution factor to 1
		this->setData(index,QString(tr("Generic")),Qt::UserRole+2);//set old std sample type to generic
		this->setData(index,0,Qt::UserRole+3);	//set old stds cut off values to 0
		bgColorForDF[index.row()*columns+index.column()] = Qt::white;
		bgColorForSampleType[index.row()*columns+index.column()] = Qt::white;
		bgColorForCutOffValue[index.row()*columns+index.column()] = Qt::white;
	}
	
	for (int i = 0; i < stdIndexList.size(); ++i) {
		QModelIndex index = stdIndexList.at(i);
		standardIndexList.append(index);
		if ( this->data(index,Qt::UserRole).toDouble() == 0) this->sampleNames[index.row()*columns+index.column()] = QString("STD(NEG)");
		else this->sampleNames[index.row()*columns+index.column()] = QString("STD");
		this->dilutionFactors[index.row()*columns+index.column()] = 1;
		this->sampleTypes[index.row()*columns+index.column()] = QString(tr("Generic"));
		this->cutOffValues[index.row()*columns+index.column()] = 0;
		bgColorForDF[index.row()*columns+index.column()] = Qt::red;
		bgColorForSampleType[index.row()*columns+index.column()] = Qt::red;
		bgColorForCutOffValue[index.row()*columns+index.column()] = Qt::red;
	}
}

void LayoutTableModel::clearStandardIndexList()
{
	QModelIndexList tempList;
	for (int i = 0; i < this->standardIndexList.size(); ++i) 
		tempList.append(this->standardIndexList.at(i));
	this->standardIndexList.clear();

	for (int i = 0; i <tempList.size(); ++i) {
		QModelIndex index = tempList.at(i);
		this->setData(index,QString("%1%2").arg(QChar('A'+ index.column())).arg(index.row()+1),Qt::EditRole);
		//this->setData(index,Qt::white,Qt::BackgroundRole);
		this->setData(index,-1,Qt::UserRole);//set old std concentrations to -1
		this->setData(index,1,Qt::UserRole+1);//set old std dilution factor to 1
		this->setData(index,QString(tr("Generic")),Qt::UserRole+2);//set old std sample type to generic
		this->setData(index,0,Qt::UserRole+3);	//set old stds cut off values to 0
		bgColorForDF[index.row()*columns+index.column()] = Qt::white;
		bgColorForSampleType[index.row()*columns+index.column()] = Qt::white;
		bgColorForCutOffValue[index.row()*columns+index.column()] = Qt::white;
	}
}

bool LayoutTableModel::hasStandardIndexList(void)
{
	if (this->standardIndexList.isEmpty()) return false;
	for (int i = 0; i < this->standardIndexList.size(); ++i)
		if (this->data(this->standardIndexList.at(i),Qt::UserRole).toDouble() < 0 ) return false;
	return true;
}

int LayoutTableModel::rowCount(const QModelIndex &parent) const{
	return rows;
}

int LayoutTableModel::columnCount(const QModelIndex &parent ) const{
	return columns;
}

QVariant LayoutTableModel::data(const QModelIndex &index, int role) const{
	if (!index.isValid())
		return QVariant();
	if (index.row() >=rows ||index.column()>=columns)
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole)
		return sampleNames[index.row()*columns+index.column()];
	if (role == Qt::UserRole) return stdConcs[index.row()*columns+index.column()];
	if (role == (Qt::UserRole+1)) return this->dilutionFactors[index.row()*columns+index.column()];
	if (role == (Qt::UserRole+2)) return this->sampleTypes[index.row()*columns+index.column()];
	if (role == (Qt::UserRole+3)) return this->cutOffValues[index.row()*columns+index.column()];
	if (role == (Qt::UserRole+4)) return ODValues[index.row()*columns+index.column()];
	if (role == (Qt::UserRole+5)) return this->generalAssayInfo[index.row()*columns+index.column()];
	if (role == Qt::TextAlignmentRole) return Qt::AlignCenter;
	if (role == Qt::BackgroundRole) {
		if ( standardIndexList.contains(index) ) return QBrush(Qt::red);
		else {
			if (bgColor) return QBrush(this->bgColor[index.row()*columns+index.column()]);
			else return QBrush(Qt::white);
		}
	}
	if (role == Qt::FontRole) {
		 QFont viewFont;
		 viewFont.setBold(true);
         //boldFont.setBold(true);
         return viewFont;
	}
	if (role == Qt::StatusTipRole || role == Qt::ToolTipRole)
		return QString("PlateLayout: Cell %1%2").arg(QChar('A'+ index.column())).arg(index.row()+1);
	else return QVariant();
}

QVariant LayoutTableModel::headerData(int section, Qt::Orientation orientation, int role) const{
	//if (role != Qt::DisplayRole) return QVariant();
	if (role == Qt::DisplayRole){
		if (orientation == Qt::Horizontal) 
			return horizonal.at(section);
		else return vertical.at(section);
	}
	else if (role == Qt::FontRole) {
		 QFont boldFont;
         boldFont.setBold(true);
         return boldFont;
	}
	else return QVariant();
}

Qt::ItemFlags LayoutTableModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;
	return QAbstractTableModel::flags(index)|Qt::ItemIsEditable;
}

bool LayoutTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() && role == Qt::EditRole) {
		if ( standardIndexList.contains(index) ) {
			QMessageBox::warning(0,tr("Bioo ELISA"),tr("This cell has been set as standard so it can not be changed!"));
			return false;
		}
		this->sampleNames[index.row()*columns+index.column()] = value.toString();
		emit dataChanged(index,index);
		return true;
	}
	if (index.isValid() && role == Qt::BackgroundRole) {
		if ( standardIndexList.contains(index) ) {
			QMessageBox::warning(0,tr("Bioo ELISA"),tr("This cell has been set as standard so it can not be changed!"));
			return false;
		}
		if (!bgColor) return false;
		this->bgColor[index.row()*columns+index.column()] = value.value<QColor>();
		emit dataChanged(index,index);
		return true;
	}
	if (index.isValid() && role == Qt::UserRole) {
		this->stdConcs[index.row()*columns+index.column()] = value.toDouble();
		emit dataChanged(index,index);
		return true;
	}
	if (index.isValid() && role == (Qt::UserRole+4)) {
		this->ODValues[index.row()*columns+index.column()] = value.toDouble();
		emit dataChanged(index,index);
		return true;
	}
	if (index.isValid() && (role == Qt::UserRole+5)) {
		this->generalAssayInfo[index.row()*columns+index.column()] = value.toString();
		emit dataChanged(index,index);
		return true;
	}
	if (index.isValid() && (role == Qt::UserRole+1)) {
		if ( standardIndexList.contains(index) ) {
			QMessageBox::warning(0,tr("Bioo ELISA"),tr("This cell has been set as standard so it can not be changed!"));
			return false;
		}
		this->dilutionFactors[index.row()*columns+index.column()] = value.toDouble();
		emit dataChanged(index,index);
		return true;
	}
	if (index.isValid() && (role == Qt::UserRole+2)) {
		if ( standardIndexList.contains(index) ) {
			QMessageBox::warning(0,tr("Bioo ELISA"),tr("This cell has been set as standard so it can not be changed!"));
			return false;
		}
		this->sampleTypes[index.row()*columns+index.column()] = value.toString();
		emit dataChanged(index,index);
		return true;
	}
	if (index.isValid() && (role == Qt::UserRole+3)) {
		if ( standardIndexList.contains(index) ) {
			QMessageBox::warning(0,tr("Bioo ELISA"),tr("This cell has been set as standard so it can not be changed!"));
			return false;
		}
		this->cutOffValues[index.row()*columns+index.column()] = value.toDouble();
		emit dataChanged(index,index);
		return true;
	}
	return false;
}

bool LayoutTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,int role){
	if (role != Qt::EditRole) return false;

	if (orientation == Qt::Horizontal) {
		if (section<0 || section > this->columnCount()) return false;
		else {
			horizonal.replace(section,value.toString());
			emit headerDataChanged(orientation,section,section);
			return true;
		}
	}
	
	else {
		if (section<0 || section > this->rowCount()) return false;
		else {
			vertical.replace(section,value.toString());
			emit headerDataChanged(orientation,section,section);
			return true;
		}
	}

	return false;
}

bool LayoutTableModel::readFile(const QString &fileName)
{
	if (fileName.isEmpty()) return false;
			
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(0, tr("ELISA Assay File"),
			tr("Cannot read file %1:\n%2.")
			.arg(file.fileName())
			.arg(file.errorString()));
		return false;
	}

	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_4_7);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	int standardIndexListSize;
	in >> int(standardIndexListSize);

	QModelIndexList tempList;
	for (int i = 0; i < standardIndexListSize; ++i) {
		int tempRow,tempColumn;
		in >> int(tempRow) >> int(tempColumn);
		QModelIndex index = this->index(tempRow,tempColumn,QModelIndex());
		tempList.append(index);
	}
	this->setStandardIndexList(tempList);

	in >> int(rows) >> int(columns);

	for ( int i = 0; i < rows*columns; ++i) {
		in  >> sampleNames[i] 
		    >> stdConcs[i] 
			>> dilutionFactors[i] 
			>> sampleTypes[i] 
			>> cutOffValues[i] 
			>> ODValues[i] 
			>> generalAssayInfo[i]
			>> QColor(bgColorForDF[i]) 
			>> QColor(bgColorForSampleType[i]) 
			>> QColor(bgColorForCutOffValue[i]); 
	}	

	emit ODValueReadFromFile(this->ODValues);
	emit somethingCopied();
	file.close();
	QApplication::restoreOverrideCursor();
	return true;
}

bool LayoutTableModel::writeFile(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(0, tr("ELISA Assay File"),
			tr("Cannot write file %1:\n%2.")
			.arg(file.fileName())
			.arg(file.errorString()));
		return false;
	}

	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_7);
	//out << quint32(MagicNumber);
	QApplication::setOverrideCursor(Qt::WaitCursor);

	out << int(this->standardIndexList.size());//how many standards
	for (int i = 0; i < this->standardIndexList.size(); ++i) {
		QModelIndex index = this->standardIndexList.at(i);
		out <<int(index.row()) << int(index.column());
	}

	out << int(rows) << int(columns);
	for ( int i = 0; i < rows*columns; ++i) {
		out << sampleNames[i] 
		    << stdConcs[i] 
			<< dilutionFactors[i] 
			<< sampleTypes[i] 
			<< cutOffValues[i] 
			<< ODValues[i] 
			<< generalAssayInfo[i]
			<< QColor(bgColorForDF[i]) 
			<< QColor(bgColorForSampleType[i]) 
			<< QColor(bgColorForCutOffValue[i]); 
	}	
	file.close();
	QApplication::restoreOverrideCursor();
	return true;
}

bool LayoutTableModel::readMethod(const QString &fileName)
{
	if (fileName.isEmpty()) return false;
			
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(0, tr("ELISA Method File"),
			tr("Cannot read file %1:\n%2.")
			.arg(file.fileName())
			.arg(file.errorString()));
		return false;
	}

	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_4_7);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	int standardIndexListSize;
	in >> int(standardIndexListSize);

	QModelIndexList tempList;
	for (int i = 0; i < standardIndexListSize; ++i) {
		int tempRow,tempColumn;
		in >> int(tempRow) >> int(tempColumn);
		QModelIndex index = this->index(tempRow,tempColumn,QModelIndex());
		tempList.append(index);
	}
	this->setStandardIndexList(tempList);

	in >> int(rows) >> int(columns);

	for ( int i = 0; i < rows*columns; ++i) {
		in  >> stdConcs[i] 
			>> dilutionFactors[i] 
			>> sampleTypes[i] 
			>> cutOffValues[i] 
			>> generalAssayInfo[i]
			>> QColor(bgColorForDF[i]) 
			>> QColor(bgColorForSampleType[i]) 
			>> QColor(bgColorForCutOffValue[i]); 
	}	

	emit somethingCopied();
	file.close();
	QApplication::restoreOverrideCursor();
	return true;
}

bool LayoutTableModel::writeMethod(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(0, tr("ELISA Method File"),
			tr("Cannot write file %1:\n%2.")
			.arg(file.fileName())
			.arg(file.errorString()));
		return false;
	}

	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_7);
	//out << quint32(MagicNumber);
	QApplication::setOverrideCursor(Qt::WaitCursor);

	out << int(this->standardIndexList.size());//how many standards
	for (int i = 0; i < this->standardIndexList.size(); ++i) {
		QModelIndex index = this->standardIndexList.at(i);
		out <<int(index.row()) << int(index.column());
	}

	out << int(rows) << int(columns);
	for ( int i = 0; i < rows*columns; ++i) {
		out << stdConcs[i] 
			<< dilutionFactors[i] 
			<< sampleTypes[i] 
			<< cutOffValues[i] 
			<< generalAssayInfo[i]
			<< QColor(bgColorForDF[i]) 
			<< QColor(bgColorForSampleType[i]) 
			<< QColor(bgColorForCutOffValue[i]); 
	}	
	file.close();
	QApplication::restoreOverrideCursor();
	return true;
}

void LayoutTableModel::changeBackGroundToNormal()
{
	this->bgColor = 0;
	for (int i = 0; i < this->rowCount(); ++i)
		for (int j = 0; j < this->columnCount(); ++j){
			QModelIndex index = this->index(i,j,QModelIndex());
			if ( !standardIndexList.contains(index)) this->setData(index,Qt::white,Qt::BackgroundRole);
		}
}

void LayoutTableModel::changeBackGroundToDF()
{
	this->bgColor = this->bgColorForDF;
	for (int i = 0; i < this->rowCount(); ++i)
		for (int j = 0; j < this->columnCount(); ++j){
			QModelIndex index = this->index(i,j,QModelIndex());
			if ( !standardIndexList.contains(index)) this->setData(index,bgColor[index.row()*columns+index.column()],Qt::BackgroundRole);
		}
}

void LayoutTableModel::changeBackGroundToSampleType()
{
	this->bgColor = this->bgColorForSampleType;
	for (int i = 0; i < this->rowCount(); ++i)
		for (int j = 0; j < this->columnCount(); ++j){
			QModelIndex index = this->index(i,j,QModelIndex());
			if ( !standardIndexList.contains(index)) this->setData(index,bgColor[index.row()*columns+index.column()],Qt::BackgroundRole);
		}
}

void LayoutTableModel::changeBackGroundToCutOffValue()
{
	this->bgColor = this->bgColorForCutOffValue;
	for (int i = 0; i < this->rowCount(); ++i)
		for (int j = 0; j < this->columnCount(); ++j){
			QModelIndex index = this->index(i,j,QModelIndex());
			if ( !standardIndexList.contains(index)) this->setData(index,bgColor[index.row()*columns+index.column()],Qt::BackgroundRole);
		}
}

void LayoutTableModel::CopySampleName(LayoutTableModel *inputModel)
{
	if (this->rowCount() != inputModel->rowCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The rows in the two models are different.Copy can not be done!"));
		return;
	}
	if (this->columnCount() != inputModel->columnCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The columns in the two models are different.Copy can not be done!"));
		return;
	}
	for ( int i = 0; i < this->rowCount()*this->columnCount(); ++i)
		this->sampleNames[i] = inputModel->getSampleName()[i];
	emit somethingCopied();
	return;
}

void LayoutTableModel::CopyGeneralAssayInfo(LayoutTableModel *inputModel)
{
	if (this->rowCount() != inputModel->rowCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The rows in the two models are different.Copy can not be done!"));
		return;
	}
	if (this->columnCount() != inputModel->columnCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The columns in the two models are different.Copy can not be done!"));
		return;
	}
	for ( int i = 0; i < this->rowCount()*this->columnCount(); ++i)
		this->generalAssayInfo[i] = inputModel->getGeneralAssayInfo()[i];
	emit somethingCopied();
	return;
}

void LayoutTableModel::CopyStandardIndexList(LayoutTableModel *inputModel)
{
	if (this->rowCount() != inputModel->rowCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The rows in the two models are different.Copy can not be done!"));
		return;
	}
	if (this->columnCount() != inputModel->columnCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The columns in the two models are different.Copy can not be done!"));
		return;
	}
	QModelIndexList tempList;
	QModelIndex tempIndex;
	for (int i = 0; i < inputModel->getStandardIndexList().size(); ++i) {
		tempIndex = this->index(inputModel->getStandardIndexList().at(i).row(),inputModel->getStandardIndexList().at(i).column(),QModelIndex());
		tempList.append(tempIndex);
	}
	this->setStandardIndexList(tempList);
	emit somethingCopied();
	return;
}

void LayoutTableModel::CopyStdConcs(LayoutTableModel *inputModel)
{
	if (this->rowCount() != inputModel->rowCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The rows in the two models are different.Copy can not be done!"));
		return;
	}
	if (this->columnCount() != inputModel->columnCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The columns in the two models are different.Copy can not be done!"));
		return;
	}
	for ( int i = 0; i < this->rowCount()*this->columnCount(); ++i)
		this->stdConcs[i] = inputModel->getStdConcs()[i];
	emit somethingCopied();
	return;
}

void LayoutTableModel::CopyDilutionFactors(LayoutTableModel *inputModel)
{
	if (this->rowCount() != inputModel->rowCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The rows in the two models are different.Copy can not be done!"));
		return;
	}
	if (this->columnCount() != inputModel->columnCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The columns in the two models are different.Copy can not be done!"));
		return;
	}
	for ( int i = 0; i < this->rowCount()*this->columnCount(); ++i)
		this->dilutionFactors[i] = inputModel->getDilutionFactors()[i];
	emit somethingCopied();
	return;
}

void LayoutTableModel::CopySampleTypes(LayoutTableModel *inputModel)
{
	if (this->rowCount() != inputModel->rowCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The rows in the two models are different.Copy can not be done!"));
		return;
	}
	if (this->columnCount() != inputModel->columnCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The columns in the two models are different.Copy can not be done!"));
		return;
	}
	for ( int i = 0; i < this->rowCount()*this->columnCount(); ++i)
		this->sampleTypes[i] = inputModel->getSampleTypes()[i];
	emit somethingCopied();
	return;
}
	
void LayoutTableModel::CopyCutOffValues(LayoutTableModel *inputModel)
{
	if (this->rowCount() != inputModel->rowCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The rows in the two models are different.Copy can not be done!"));
		return;
	}
	if (this->columnCount() != inputModel->columnCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The columns in the two models are different.Copy can not be done!"));
		return;
	}
	for ( int i = 0; i < this->rowCount()*this->columnCount(); ++i)
		this->cutOffValues[i] = inputModel->getCutOffValues()[i];
	emit somethingCopied();
	return;
}
	
void LayoutTableModel::CopyBGColorForDF(LayoutTableModel *inputModel)
{
	if (this->rowCount() != inputModel->rowCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The rows in the two models are different.Copy can not be done!"));
		return;
	}
	if (this->columnCount() != inputModel->columnCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The columns in the two models are different.Copy can not be done!"));
		return;
	}
	for ( int i = 0; i < this->rowCount()*this->columnCount(); ++i)
		this->bgColorForDF[i] = inputModel->getBGColorForDF()[i];
	emit somethingCopied();
	return;
}
	
void LayoutTableModel::CopyBGColorForSampleType(LayoutTableModel *inputModel)
{
	if (this->rowCount() != inputModel->rowCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The rows in the two models are different.Copy can not be done!"));
		return;
	}
	if (this->columnCount() != inputModel->columnCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The columns in the two models are different.Copy can not be done!"));
		return;
	}
	for ( int i = 0; i < this->rowCount()*this->columnCount(); ++i)
		this->bgColorForSampleType[i] = inputModel->getBGColorForSampleType()[i];
	emit somethingCopied();
	return;
}
	
void LayoutTableModel::CopyBGColorForCutOffValue(LayoutTableModel *inputModel)
{
	if (this->rowCount() != inputModel->rowCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The rows in the two models are different.Copy can not be done!"));
		return;
	}
	if (this->columnCount() != inputModel->columnCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The columns in the two models are different.Copy can not be done!"));
		return;
	}
	for ( int i = 0; i < this->rowCount()*this->columnCount(); ++i)
		this->bgColorForCutOffValue[i] = inputModel->getBGColorForCutOffValue()[i];
	emit somethingCopied();
	return;
}

void LayoutTableModel::CopyAll(LayoutTableModel *inputModel)
{
	if (this->rowCount() != inputModel->rowCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The rows in the two models are different.Copy can not be done!"));
		return;
	}
	if (this->columnCount() != inputModel->columnCount()) {
		QMessageBox::warning(0,tr("Bioo ELISA"),tr("The columns in the two models are different.Copy can not be done!"));
		return;
	}
	QModelIndexList tempList;
	QModelIndex tempIndex;
	for (int i = 0; i < inputModel->getStandardIndexList().size(); ++i) {
		tempIndex = this->index(inputModel->getStandardIndexList().at(i).row(),inputModel->getStandardIndexList().at(i).column(),QModelIndex());
		tempList.append(tempIndex);
	}
	this->setStandardIndexList(tempList);
	for ( int i = 0; i < this->rowCount()*this->columnCount(); ++i) {
		this->sampleNames[i] = inputModel->getSampleName()[i];
		this->stdConcs[i] = inputModel->getStdConcs()[i];
		this->dilutionFactors[i] = inputModel->getDilutionFactors()[i];
		this->sampleTypes[i] = inputModel->getSampleTypes()[i];
		this->generalAssayInfo[i] = inputModel->getGeneralAssayInfo()[i];
		this->cutOffValues[i] = inputModel->getCutOffValues()[i];
		this->bgColorForDF[i] = inputModel->getBGColorForDF()[i];
		this->bgColorForSampleType[i] = inputModel->getBGColorForSampleType()[i];
		this->bgColorForCutOffValue[i] = inputModel->getBGColorForCutOffValue()[i];
	}
	emit somethingCopied();
	return;
}

QString* LayoutTableModel::getSampleName()
{
	return this->sampleNames;
}
	
QModelIndexList LayoutTableModel::getStandardIndexList()
{
	return this->standardIndexList;
}
	
double* LayoutTableModel::getStdConcs()
{
	return this->stdConcs;
}
	
double* LayoutTableModel::getDilutionFactors()
{
	return this->dilutionFactors;
}
	
QString* LayoutTableModel::getSampleTypes()
{
	return this->sampleTypes;
}

QString* LayoutTableModel::getGeneralAssayInfo()
{
	return this->generalAssayInfo;
}

double* LayoutTableModel::getCutOffValues()
{
	return this->cutOffValues;
}
	
QColor* LayoutTableModel::getBGColorForDF()
{
	return this->bgColorForDF;
}
	
QColor* LayoutTableModel::getBGColorForSampleType()
{
	return this->bgColorForSampleType;
}
	
QColor* LayoutTableModel::getBGColorForCutOffValue()
{
	return this->bgColorForCutOffValue;
}

bool LayoutTableModel::AlgorithmButtonEnabled()
{
	if (!this->hasStandardIndexList()) return false;
	for (int i = 0; i < this->standardIndexList.size(); ++i) {
		QModelIndex index = this->standardIndexList.at(i);
		if (this->data(index,Qt::UserRole+4).toDouble() < 0.005 ) return false;
	}
	return true;
}

void LayoutTableModel::updateODValuesFromODModel(const QModelIndex& index, const double ODValue)
{
	QModelIndex plateIndex = this->index(index.row(),index.column(),QModelIndex());
	if ( ODValue != this->data(plateIndex,Qt::UserRole+4).toDouble() ) this->setData(plateIndex,ODValue,Qt::UserRole+4);
}

LayoutTableModel::~LayoutTableModel()
{
}


/* ######################################################################
   ######################################################################
   ######################################################################
   ######################################################################
*/

//implemetation for ResultTableModel, editable and resizable
ResultTableModel::ResultTableModel()
{
	rows = columns = 0;
	contents.clear();
	header_data = new QStringList();
}

ResultTableModel::ResultTableModel(int row_number, int column_number, QObject *parent):QAbstractTableModel(parent) {
	rows = row_number;
	columns = column_number;
	contents.clear();
	header_data = new QStringList();
	*header_data << "Sample Name" << "Sample Type" << "Cut Off" << "Dilu. Factor" << "OD-1" << "OD-2" << "B/Bo" << "Results" << "Pass/Fail" << "Comments";
}

int ResultTableModel::rowCount(const QModelIndex &parent) const{
	return rows;
}

int ResultTableModel::columnCount(const QModelIndex &parent ) const{
	return columns;
}

QVariant ResultTableModel::data(const QModelIndex &index, int role) const{
	if (!index.isValid())
		return QVariant();
	if (index.row() >=rows ||index.column()>=columns)
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole){
		if (contents.isEmpty()) return 0; //sampleNames[index.row()*columns+index.column()];
		QString row = contents.at(index.row());
		QStringList rowContent = row.split("\t");
		return rowContent.at(index.column());
	}
	if (role == Qt::BackgroundRole) {
		if ( index.column() == 7 || index.column() == 8) {
			QModelIndex cutOffIndex = this->index(index.row(),2,QModelIndex());
			QModelIndex concIndex = this->index(index.row(),7,QModelIndex());
			if ( this->data(cutOffIndex,Qt::DisplayRole).toDouble() < this->data(concIndex,Qt::DisplayRole).toDouble() ) return QBrush(Qt::red);
			else return QBrush(Qt::blue);
		}
		else return QBrush(Qt::white);
	}


	if (role == Qt::TextAlignmentRole)
		return Qt::AlignCenter;
	if (role == Qt::StatusTipRole || role == Qt::ToolTipRole)
		return QString("PlateLayout: Cell %1%2").arg(QChar('A'+ index.column())).arg(index.row()+1);
	else return QVariant();
}

QVariant ResultTableModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if (role != Qt::DisplayRole)
		return QVariant();
	if (orientation == Qt::Horizontal)
		return header_data->at(section);
	else return QString("%1").arg(section+1);
}

Qt::ItemFlags ResultTableModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;
	return QAbstractTableModel::flags(index)|Qt::ItemIsEditable;
}

void ResultTableModel::setContents(QStringList inputList)
{
	for (int i = 0; i < inputList.size(); ++i)
		this->contents.append(inputList.at(i));
}

void ResultTableModel::clearContents()
{
	this->contents.clear();
}



bool ResultTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() && role == Qt::EditRole && index.column() == 9) {
		QString oldRow = this->contents.at(index.row());
		QStringList rowContent = oldRow.split("\t");
		rowContent.replace(9,value.toString());
		QString newRow = rowContent.join("\t");
		this->contents.replace(index.row(),newRow);
		emit dataChanged(index,index);
		return true;
	}
	return false;
}

/*bool ResultTableModel::insertRows(int position, int rows, const QModelIndex &index){
	beginInsertRows(QModelIndex(),position, position+rows-1);

	for (int row = 0; row < rows; ++row) {
	}

	endInsertRows();
	return true;
}*/

/*bool ResultTableModel::removeRows(int position, int rows, const QModelIndex &index){
	beginRemoveRows(QModelIndex(),position,position+rows-1);

	for (int row = 0; row < rows; ++row) {
	}

	endRemoveRows();
	return true;
}*/



ResultTableModel::~ResultTableModel()
{
}