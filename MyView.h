#ifndef MYVIEW_H
#define MYVIEW_H

#include "TableModel.h"
#include <QTableView>

class ODView: public QWidget
{
	Q_OBJECT
public:
	ODView(ODTableModel *model,QWidget* parent=0);
	ODTableModel* model() const;
	QItemSelectionModel* selectionModel();
	void setSelectionModel(QItemSelectionModel* selectionModel);
	void getODValue(double *returnValue);
	void setODValue(double *inputValue);
	~ODView();

public slots:
	void copy();
	void paste();
	void Delete();

protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
signals:
		void modified();
private:
	int rows;
	int columns;
	bool isModified;
	ODTableModel *od_model;
	QTableView *view;
};


//view for plate layout
class LayoutView: public QTableView
{
	Q_OBJECT
public:
	LayoutView(LayoutTableModel *model, QWidget* parent=0);
	LayoutTableModel* model() const;
	QItemSelectionModel* selectionModel();
	void setSelectionModel(QItemSelectionModel* selectionModel);
	void getSampleName(QStringList *returnName);
	void setSampleName(QStringList *inputName);
	~LayoutView();
	
public slots:
	void copy();
	void paste();
	void Delete();

protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
signals:
		void modified();
private:
	int rows;
	int columns;
	bool isModified;
	LayoutTableModel *plate_model;
	QTableView *view;
};
#endif