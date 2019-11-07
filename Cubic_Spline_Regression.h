#ifndef CUBIC_SPLINE_REGRESSION_H
#define CUBIC_SPLINE_REGRESSION_H

#include <cmath>

#include <QVector>
#include<qwidget.h>

#include<qwt_plot.h>
#include<qwt_plot_curve.h>
#include<qwt_symbol.h>

class Cubic_Spline_Regression
{
public:
	Cubic_Spline_Regression(void);

	//pretreat raw data 
	//and calculate the number of the standards used in raw data
	bool setRawData(const QVector<double> rawData[2]);
	
	//set "mark" and its painting parameters	
	bool generateMark(QwtPlotCurve* mark);
	void setMarkPenWidth(int PenWidth, QwtPlotCurve* mark);
	void setMarkPenBrush(QColor BrushColor, QwtPlotCurve* mark);
	void setMarkSymbolPenSize(int SymbolPenSize, QwtPlotCurve* mark);
	void setMarkSymbolPenBrush(QColor SymbolBrushColor, QwtPlotCurve* mark);

	//generated a curve,calculate the papameters
	//and set the painting parameter for the curve
	bool generateCurve(QwtPlotCurve* curve);
    void setCurvePenWidth(int PenWidth,QwtPlotCurve* curve );
	void setCurvePenBrush(QColor BrushColor, QwtPlotCurve* curve);
	
	//get the calculated curve parameters
	const QVector<double> Coefficient();
	const double Regression_Coefficient();
	const double Standard_Deviation();
	const int Standard_Number();
    double BisectFunct(double guessedLogConc,double OD);//used in root finding

	//calculate concentration
	double calculateConc(double OD, double dilutionFactor);
	double calculateConc(double OD1, double OD2, double dilutionFactor);
	double IC50();
	double Inflection_Point() {return inflection_point;};
	void Linear_Range(double range[2]);

	~Cubic_Spline_Regression(void);

private:
	QVector<double> processedData[2];
	QVector<double> calculatedMarkData[2];
	QVector<double> calculatedCurveData[2];
	bool ableToGeneratedCurve;
	bool curveGenerated;
	
	//"mark" painting parameters
	int markPenWidth;
	QColor markPenBrush;
	QwtSymbol *markSymbol;
	int markSymbolSize;
	QColor markSymbolPenBrush;
	
	
	QVector<double> *coefficient;
	double residual_SD;
	int std_number;
	double inflection_point;

	//"curve" painting parameters
	int curvePenWidth;
	QColor curvePenBrush;
};

#endif