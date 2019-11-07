#include "LinearRegression.h"
#include <qalgorithms.h>
#include <QtGui>

Linear_Regression::Linear_Regression(void)
{
	ableToGeneratedCurve = false;
	slope = 0;
	intercept = 0;
	residual_SD = 0;
	std_number = 0;

	processedData[0].clear();
	processedData[1].clear();

	//"curve" painting parameters
	curvePenWidth = 2;
	curvePenBrush = Qt::black;

	//"mark" painting parameters
	markPenWidth = 2;
	markPenBrush = Qt::red;
	markSymbolSize = 8;
	markSymbolPenBrush = Qt::red;
	markSymbol = 0;
}

bool Linear_Regression::setRawData(const QVector<double> rawData[2]){
	if (rawData[0].size()<2 || rawData[1].size()<2 || rawData[0].size() != rawData[1].size() ) 
		return false;

	QVector<double> temp_conc,temp_OD;
	for (int i=0; i<rawData[0].size(); ++i) {
		temp_conc.append(rawData[0].at(i));
		temp_OD.append(rawData[1].at(i));
	}
	qSort(temp_conc.begin(),temp_conc.end());//increasing order of concentrations

	//calculated how many standards in the ELISA assay
	int std_count = 1;
	QVector<int> processed_index;
	processed_index.append(0);
	for (int i=1; i<temp_conc.size(); ++i) {
		if (temp_conc.at(i) > temp_conc.at(i-1)){
			std_count++;
			processed_index.append(i);
		}
	}

	if (std_count<2) return false;

	for (int i=0; i<std_count; ++i) {
		int  identical_std_count = 0;
		double sum_OD = 0;
		for (int j = 0 ; j <rawData[0].size(); ++j ) {
			if (rawData[0].at(j) == temp_conc.at(processed_index.at(i))){
				sum_OD += rawData[1].at(j);
			    identical_std_count++;
			}
		}
		processedData[0].append(temp_conc.at(processed_index.at(i)));
		processedData[1].append(sum_OD/(double)identical_std_count);
	}

	if (processedData[1].at(0) == 0) return false;
	
	bool highestODForNegativeStd = true;
	int index = 1;
	while (highestODForNegativeStd && ( index < processedData[1].size() ) ) {
		if ( processedData[1].at(0) <= processedData[1].at(index) ) highestODForNegativeStd = false;
		index++;
	}
	if (!highestODForNegativeStd) QMessageBox::warning(0,"ELISA Bioo","The negative control(s) should have the HIGHEST OD value(s)!\nNo RELIABLE standard curve can be generated!\nAnd the calculated concentrations are questionable.");
	

	std_number = std_count;
	ableToGeneratedCurve = true;

	return true;
}

bool Linear_Regression::generateMark(QwtPlotCurve* mark){
	if (!ableToGeneratedCurve) return false;

	if (!mark) return false;

	QPen pen;  // creates a default pen
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(markPenWidth);
	pen.setBrush(markPenBrush);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	mark->setPen(pen);	
	mark->setStyle(QwtPlotCurve::NoCurve);

	if (!markSymbol) markSymbol = new QwtSymbol(QwtSymbol::Cross);
	markSymbol->setStyle(QwtSymbol::Cross);
	markSymbol->setPen(pen);
	markSymbol->setSize(markSymbolSize);
	markSymbol->setColor(markSymbolPenBrush);
	mark->setSymbol(markSymbol);

	QVector<double> x, y;
	for (int i=1; i<std_number; ++i) {
		x.append(log(processedData[0].at(i)));
		y.append(processedData[1].at(i)/processedData[1].at(0));
	}
	mark->setSamples(x,y);

	return true;
}

void Linear_Regression::setMarkPenWidth(int PenWidth, QwtPlotCurve* mark){
	QPen pen;
	pen.setWidth(PenWidth);
	mark->setPen(pen);
}

void Linear_Regression::setMarkPenBrush(QColor BrushColor, QwtPlotCurve* mark){
	QPen pen;
	pen.setBrush(BrushColor);;
	mark->setPen(pen);
}

void Linear_Regression::setMarkSymbolPenSize(int SymbolPenSize, QwtPlotCurve* mark){
	markSymbol->setSize(SymbolPenSize);
	mark->setSymbol(markSymbol);
}

void Linear_Regression::setMarkSymbolPenBrush(QColor SymbolBrushColor, QwtPlotCurve* mark){
	markSymbol->setColor(SymbolBrushColor);
	mark->setSymbol(markSymbol);
}


bool Linear_Regression::generateCurve(QwtPlotCurve* curve){
	if (!ableToGeneratedCurve) return false;

	if (!curve) return false;

	/*
	bool highestODForNegativeStd = true;
	int index = 1;
	while (highestODForNegativeStd && ( index < processedData[1].size() ) ) {
		if ( processedData[1].at(0) <= processedData[1].at(index) ) highestODForNegativeStd = false;
		index++;
	}
	if (!highestODForNegativeStd) QMessageBox::warning(0,"ELISA Bioo","The negative control(s) should have the HIGHEST OD value(s)!\nNo RELIABLE standard curve can be generated!\nAnd the calculated concentrations are questionable.");
	*/

	QVector<double> x, y;
	for (int i=1; i<std_number; ++i) {
		x.append(log(processedData[0].at(i)));
		y.append(processedData[1].at(i)/processedData[1].at(0));
	}
	
	double x_sum = 0, y_sum = 0, x2_sum = 0, xy_sum = 0, y2_sum = 0;
	for (int i = 0; i < (std_number-1); ++i) {
		x_sum += (x.at(i));
		y_sum += (y.at(i));
		x2_sum += (x.at(i)*x.at(i));
		xy_sum += (x.at(i)*y.at(i));
		y2_sum += (y.at(i)*y.at(i));
	}

	intercept = (y_sum*x2_sum - x_sum*xy_sum)/((std_number-1)*x2_sum-x_sum*x_sum);
	slope = ((std_number-1)*xy_sum-x_sum*y_sum)/((std_number-1)*x2_sum-x_sum*x_sum);

    double residual_sum=0, total_sum = 0;
	for (int i = 0; i < (std_number-1); ++i) {
		residual_sum += (y.at(i)-intercept-slope*x.at(i)) * (y.at(i)-intercept-slope*x.at(i));
		total_sum += (y.at(i) - (y_sum/(std_number-1))) *(y.at(i) - (y_sum/(std_number-1)));
	}

	if (total_sum !=0)
		residual_SD = 1 - (residual_sum/total_sum);

	
	QVector<double> y_calculated;
	for (int i=0; i<std_number-1; ++i) 
		y_calculated.append(intercept+slope*x.at(i));

	//set the curve attributes
	QPen pen;  // creates a default pen
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(curvePenWidth);
	pen.setBrush(curvePenBrush);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	curve->setPen(pen);
	curve->setRenderHint(QwtPlotCurve::RenderAntialiased);
	curve->setSamples(x,y_calculated);

	return true;
}

void Linear_Regression::setCurvePenWidth(int PenWidth,QwtPlotCurve* curve ){
	QPen pen;  // creates a default pen
	pen.setWidth(PenWidth);
	curve->setPen(pen);
}

void Linear_Regression::setCurvePenBrush(QColor BrushColor, QwtPlotCurve* curve){
	QPen pen;  // creates a default pen
	pen.setBrush(BrushColor);
	curve->setPen(pen);
}

	
	//get the calculated curve parameters
const double Linear_Regression::Slope(){
	return slope;
}

const double Linear_Regression::Intercept(){
	return intercept;
}

const double Linear_Regression::Regression_Coefficient(){
	return sqrt(residual_SD);
}

const double Linear_Regression::Standard_Deviation(){
	return residual_SD;
}

const int Linear_Regression::Standard_Number(){
	return std_number;
}


double Linear_Regression::calculateConc(double OD, double dilutionFactor){
	if (ableToGeneratedCurve){
		if ( (OD/this->processedData[1].at(0)) >= 1.00) return 0;
		else return exp(((OD/processedData[1].at(0))- intercept)/slope)*dilutionFactor;
	}
	return 0;
}

double Linear_Regression::calculateConc(double OD1, double OD2, double dilutionFactor){
	if (ableToGeneratedCurve){
		double OD = 0.5*(OD1+OD2);
		if ( (OD/this->processedData[1].at(0)) >= 1.00) return 0;
		else return exp(((OD/processedData[1].at(0))- intercept)/slope)*dilutionFactor;
	}
	return 0;
}

double Linear_Regression::IC50(){
	return calculateConc(processedData[1].at(0)/2,1);
}

Linear_Regression::~Linear_Regression(void)
{
}
