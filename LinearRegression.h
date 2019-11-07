#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H

#include <cmath>

#include <QVector>
#include<qwidget.h>

#include<qwt_plot.h>
#include<qwt_plot_curve.h>
#include<qwt_symbol.h>



class Linear_Regression
{
public:
	Linear_Regression(void);
	
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
	const double Slope();
	const double Intercept();
	const double Regression_Coefficient();
	const double Standard_Deviation();
	const int Standard_Number();

	//calculate concentration
	double calculateConc(double OD, double dilutionFactor);
	double calculateConc(double OD1, double OD2, double dilutionFactor);
	double IC50();

	//destructor
	~Linear_Regression(void);

private:
	QVector<double> processedData[2];
	bool ableToGeneratedCurve;
	
	//"mark" painting parameters
	int markPenWidth;
	QColor markPenBrush;
	QwtSymbol *markSymbol;
	int markSymbolSize;
	QColor markSymbolPenBrush;
	
	
	double slope;
	double intercept;
	double residual_SD;
	int std_number;

	//"curve" painting parameters
	int curvePenWidth;
	QColor curvePenBrush;

	
};

#endif

