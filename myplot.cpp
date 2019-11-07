#include "myplot.h"
#include <qvector.h>
#include <qwt_scale_engine.h>
#include <qwt_interval.h>

#include<cmath>
#include<QtGui>

MyPlot::MyPlot(QWidget *parent):QwtPlot(parent) {
	curve = 0;
	mark = 0;
	regressionMethod = linearFit;
	linearRegression = 0;
	logitLogRegression = 0;
	cubicSplineRegression = 0;
	cubicSpline = 0;
	fourPL = 0;
	setAxisTitle( xBottom, "Concentration" );
	setAxisTitle( yLeft, "Realtive Optical Density" );
	replot();
}

MyPlot::MyPlot(char *name,QVector<double> stdData[2], RegressionMethod method, QWidget *parent):QwtPlot(parent)
{
	//initialization
	curve = 0;
	mark = 0;
	regressionMethod = linearFit;
	linearRegression = 0;
	logitLogRegression = 0;
	cubicSplineRegression = 0;
	cubicSpline = 0;
	fourPL = 0;

	// Show the axes
	setAxisTitle( xBottom, "Concentration" );
	setAxisTitle( yLeft, "Realtive Optical Density" );
	regressionMethod = method;
	if (curve) delete curve;
	if (mark) delete mark;
	curve = new QwtPlotCurve();
	mark = new QwtPlotCurve();
	if (linearRegression) {
		delete linearRegression;
		linearRegression = 0;
	}
	if (logitLogRegression) {
		delete logitLogRegression;
		logitLogRegression = 0;
	}
	if (cubicSplineRegression) {
		delete cubicSplineRegression;
		cubicSplineRegression = 0;
	}
	if (cubicSpline) {
		delete cubicSpline;
		cubicSpline = 0;
	}
	if (fourPL) {
		delete fourPL;
		fourPL = 0;
	}

	if (method == linearFit) {
		linearRegression = new Linear_Regression();
		if ( curve && mark && linearRegression){ 
			if (linearRegression->setRawData(stdData)) {
				linearRegression->generateCurve(curve);
				linearRegression->generateMark(mark);
			}
		}
		curve->attach(this);
		mark->attach(this);
	}
	else if (method == logitLogFit) {
		logitLogRegression = new Logit_Log_Regression();
		if ( curve && mark && logitLogRegression){ 
			if (logitLogRegression->setRawData(stdData)) {
				logitLogRegression->generateCurve(curve);
				logitLogRegression->generateMark(mark);
			}
		}
		curve->attach(this);
		mark->attach(this);
	}
	else if (method == cubicSplineFit) {
		/*cubicSplineRegression = new Cubic_Spline_Regression();
		if ( curve && mark && cubicSplineRegression){ 
			if (cubicSplineRegression->setRawData(stdData)) {
				cubicSplineRegression->generateCurve(curve);
				cubicSplineRegression->generateMark(mark);
			}
		}*/
		cubicSpline = new cSpline();
		if ( curve && mark && cubicSpline){ 
			if (cubicSpline->setRawData(stdData)) {
				cubicSpline->generateCurve(curve);
				cubicSpline->generateMark(mark);
			}
		}
		curve->attach(this);
		mark->attach(this);
	}
	else if (method == fourPLFit) {
		fourPL = new FourPL();
		if ( curve && mark && fourPL){ 
			if (fourPL->setRawData(stdData)) {
				fourPL->generateCurve(curve);
				fourPL->generateMark(mark);
			}
		}
		curve->attach(this);
		mark->attach(this);
	}


	replot();
}

double MyPlot::calculateConcs(double OD, double dilutionFactor)
{
	if (this->regressionMethod == RegressionMethod::linearFit) return this->linearRegression->calculateConc(OD,dilutionFactor);
	if (this->regressionMethod == RegressionMethod::logitLogFit) return this->logitLogRegression->calculateConc(OD,dilutionFactor);
	if (this->regressionMethod == RegressionMethod::cubicSplineFit) return this->cubicSpline->calculateConc(OD,dilutionFactor);
	if (this->regressionMethod == RegressionMethod::fourPLFit) return this->fourPL->calculateConc(OD,dilutionFactor);
}

QString MyPlot::information()
{
	return QString("test");
}

MyPlot::~MyPlot() 
{
}


/* ######################################################################
   ######################################################################
   ######################################################################
   ######################################################################
*/

//implemetation for plotView
plotView::plotView(QWidget *parent):QWidget(parent)
{
	plotWidget = new MyPlot(this);
	//plotWidget->setMinimumSize(800,400);
	//plotWidget->setVisible(true);
	
	gridLayout = new QGridLayout(this);
	topLayout = new QHBoxLayout(this);
	topRightLayout = new QVBoxLayout(this);
	mainLayout = new QVBoxLayout(this);

	standard = new QLabel("Optical Density",this);
	//standard->setVisible(true);
	concentration = new QLabel("Concentration",this);
	//concentration->setVisible(true);
	gridLayout->addWidget(standard,0,1,1,1,Qt::AlignCenter);
	gridLayout->addWidget(concentration,0,2,1,1,Qt::AlignCenter);

	topRightLayout->addLayout(gridLayout);
	topRightLayout->addStretch();
	topLayout->addWidget(plotWidget,4);
	topLayout->addLayout(topRightLayout,1);

	info = new QLabel(plotWidget->information(),this);
	//info->setVisible(true);
	mainLayout = new QVBoxLayout(this);
	//mainLayout->addWidget(titleLabel);
	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(info);
	this->setLayout(mainLayout);
	//this->setVisible(true);
}

plotView::plotView(char *name,QVector<double> stdData[2], MyPlot::RegressionMethod method, QWidget *parent):QWidget(parent) {
	titleLabel = new QLabel(name);
	plotWidget = new MyPlot(name,stdData,method,this);

	gridLayout = new QGridLayout(this);
	topLayout = new QHBoxLayout(this);
	topRightLayout = new QVBoxLayout(this);
	mainLayout = new QVBoxLayout(this);

	standard = new QLabel("OD",this);
	//standard->setVisible(true);
	concentration = new QLabel("Conc.",this);
	//concentration->setVisible(true);
	gridLayout->addWidget(standard,0,0,1,1,Qt::AlignCenter);
	gridLayout->addWidget(concentration,0,1,1,1,Qt::AlignCenter);
	for (int i = 0; i <stdData[0].count(); ++i) {
		stds.append(new QLabel(QString("%1").arg(stdData[1].at(i)),this));
		gridLayout->addWidget(stds.at(i),i+1,0,1,1,Qt::AlignCenter);
		conc.append(new QLabel(QString("%1").arg(stdData[0].at(i)),this));
		gridLayout->addWidget(conc.at(i),i+1,1,1,1,Qt::AlignCenter);
	}
	gridLayout->setHorizontalSpacing(10);
	gridLayout->setVerticalSpacing(10);

	topRightLayout->addLayout(gridLayout);
	topRightLayout->addStretch();
	topLayout = new QHBoxLayout(this);
	topLayout->addWidget(plotWidget,4);
	topLayout->addLayout(topRightLayout,1);

	info = new QLabel(plotWidget->information());
	mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(titleLabel);
	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(info);
	this->setLayout(mainLayout);
}

MyPlot* plotView::getPlotWidget()
{
	return this->plotWidget;
}

void plotView::paintEvent(QPaintEvent *event)
{
	if (stds.isEmpty()) standard->setVisible(false);
	else standard->setVisible(true);
	if (conc.isEmpty()) concentration->setVisible(false);
	else concentration->setVisible(true);
	mainLayout->setMargin(50);
	mainLayout->setSpacing(50);
	mainLayout->setContentsMargins(50,50,50,50);
	mainLayout->setGeometry(this->geometry());
}

void plotView::resizeEvent(QResizeEvent *event)
{
	if (stds.isEmpty()) standard->setVisible(false);
	else standard->setVisible(true);
	if (conc.isEmpty()) concentration->setVisible(false);
	else concentration->setVisible(true);
	mainLayout->setMargin(50);
	mainLayout->setSpacing(50);
	mainLayout->setContentsMargins(50,50,50,50);
	mainLayout->setGeometry(this->geometry());
	//mainLayout->setSizeConstraint(QLayout::SetFixedSize);
}

plotView::~plotView()
{
}