#ifndef MYPLOT_H
#define MYPLOT_H

#include<cmath>
#include<qvector.h>

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QWidget;
class QString;

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "LinearRegression.h"
#include "Logit_Log_Regression.h"
#include "Cubic_Spline_Regression.h"
#include "cSpline.h"
#include "FourPL.h"

class MyPlot : public QwtPlot {
	Q_OBJECT
public:
	enum RegressionMethod {linearFit = 1, logitLogFit = 2, cubicSplineFit = 3, fourPLFit = 4};
	MyPlot(QWidget *parent=NULL);
	MyPlot(char* name,QVector<double> stdData[2], RegressionMethod method, QWidget *parent=NULL);
	double calculateConcs(double OD, double dilutionFactor);
	double calculateConcs(double OD1, double OD2, double dilutionFactor);
	bool setData(QVector<double> stdData[2]);
	bool setRegressionMethod(RegressionMethod method);
	QString information();
	~MyPlot();
private:
	QwtPlotCurve *curve;
	QwtPlotCurve *mark;
	RegressionMethod regressionMethod;
	Linear_Regression *linearRegression;
	Logit_Log_Regression *logitLogRegression;
	Cubic_Spline_Regression *cubicSplineRegression;
	cSpline *cubicSpline;
	FourPL *fourPL;
};

class plotView:public QWidget
{
	Q_OBJECT
public:
	plotView(QWidget *parent=NULL);
	plotView(char* name,QVector<double> stdData[2], MyPlot::RegressionMethod method, QWidget *parent=NULL);
	bool setData(QVector<double> stdData[2]);
	bool setRegressionMethod(MyPlot::RegressionMethod method);
	MyPlot* getPlotWidget();
	~plotView();
protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
private:
	MyPlot *plotWidget;
	QLabel *titleLabel;
	QLabel *standard,*concentration;
	QVector<QLabel*> stds,conc;
	QLabel *info;
	QGridLayout *gridLayout;
	QVBoxLayout *topRightLayout,*mainLayout;
	QHBoxLayout *topLayout;
};

#endif