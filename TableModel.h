#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QModelIndex>
#include <qvariant.h>
#include <qstringlist.h>

//model for OD Layout, editable but not resizable
class ODTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	ODTableModel(int row_number, int column_number, QObject *parent=0);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

	//make it editable
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,int role = Qt::EditRole);

	//make it resizable
	//bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
	//bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
	//bool insertColumns(int position, int columns, const QModelIndex &index = QModelIndex());
	//bool removeColumns(int position, int columns, const QModelIndex &index = QModelIndex());
	
	~ODTableModel();
signals:
	void ODValueChanged(const QModelIndex& index,const double ODValue);
private slots:
	void updateODValuesFromPlateModel(double* ODs);
private:
	int rows;
	int columns;
	double *ODValues;
	QStringList horizonal;
	QStringList vertical;
};


//model for plate Layout, editable but not resizable
class LayoutTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	LayoutTableModel(int row_number, int column_number, QObject *parent=0);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

	//make it editable
	QModelIndexList getStandardIndexList();
	void setStandardIndexList(QModelIndexList stdIndexList);
	void clearStandardIndexList();
	bool hasStandardIndexList(void);
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,int role = Qt::EditRole);

	//make it resizable
	//bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
	//bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
	//bool insertColumns(int position, int columns, const QModelIndex &index = QModelIndex());
	//bool removeColumns(int position, int columns, const QModelIndex &index = QModelIndex());
	
	bool readFile(const QString &fileName);
	bool writeFile(const QString &fileName);
	bool readMethod(const QString &fileName);
	bool writeMethod(const QString &fileName);

	void changeBackGroundToNormal();
	void changeBackGroundToDF();
	void changeBackGroundToSampleType();
	void changeBackGroundToCutOffValue();

	void CopySampleName(LayoutTableModel *inputModel);
	void CopyGeneralAssayInfo(LayoutTableModel *inputModel);
	void CopyStandardIndexList(LayoutTableModel *inputModel);
	void CopyStdConcs(LayoutTableModel *inputModel);
	void CopyDilutionFactors(LayoutTableModel *inputModel);
	void CopySampleTypes(LayoutTableModel *inputModel);
	void CopyCutOffValues(LayoutTableModel *inputModel);
	void CopyBGColorForDF(LayoutTableModel *inputModel);
	void CopyBGColorForSampleType(LayoutTableModel *inputModel);
	void CopyBGColorForCutOffValue(LayoutTableModel *inputModel);
	void CopyAll(LayoutTableModel *inputModel);

	QString* getSampleName();
	
	double* getStdConcs();
	double* getDilutionFactors();
	QString* getSampleTypes();
	QString* getGeneralAssayInfo();
	double* getCutOffValues();
	QColor* getBGColorForDF();
	QColor* getBGColorForSampleType();
	QColor* getBGColorForCutOffValue();

	bool AlgorithmButtonEnabled();

	~LayoutTableModel();

signals:
	void somethingCopied();
	void ODValueReadFromFile(double* ODs);
private slots:
	void updateODValuesFromODModel(const QModelIndex& index, const double ODValue);

private:
	int rows;
	int columns;
	QString *sampleNames;
	double *stdConcs; //Qt::UserRole, store concentrations of standards
	double *dilutionFactors;//Qt::UserRole+1 store dilution factors of samples
	QString *sampleTypes;//Qt::UserRole+2, store sample type
	double *cutOffValues;//Qt::UserRole+3 store cut off values
	double *ODValues;//Qt:;UserRole+4
	QString *generalAssayInfo;//Qt::UserRole+5,(1,1) Assay Name, (2,1) Catlog No. (3,1) Algorithm (4,1) Tech Name
	QColor *bgColor;//background role
	QColor *bgColorForDF;
	QColor *bgColorForSampleType;
	QColor *bgColorForCutOffValue;
	QStringList horizonal;
	QStringList vertical;
	QModelIndexList standardIndexList;
};

//model for result Layout, editable and resizable

class ResultTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	ResultTableModel();
	ResultTableModel(int row_number, int column_number, QObject *parent=0);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	void setContents(QStringList inputList);
	void clearContents();

	//make it editable
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	//bool setHeaderData((int section, Qt::Orientation orientation, const QVariant &value,int role = Qt::DisplayRole);

	//make it resizable
	//bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
	//bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
	//bool insertColumns(int position, int columns, const QModelIndex &index = QModelIndex());
	//bool removeColumns(int position, int columns, const QModelIndex &index = QModelIndex());

	~ResultTableModel();
private:
	int rows;
	int columns;
	QStringList contents;
	QStringList *header_data;
};


#endif // TABLEMODEL_H
