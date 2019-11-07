#include "Cubic_Spline_Regression.h"
#include <Matrix45.h>
#include "LinearRegression.h"

Cubic_Spline_Regression::Cubic_Spline_Regression(void)
{
	ableToGeneratedCurve = false;
	curveGenerated = false;
	coefficient = 0;
	residual_SD = 0;
	std_number = 0;
	inflection_point = 0;

	processedData[0].clear();
	processedData[1].clear();
	calculatedMarkData[0].clear();
	calculatedMarkData[1].clear();
	calculatedCurveData[0].clear();
	calculatedCurveData[1].clear();

	//"curve" painting parameters
	curvePenWidth = 2;
	curvePenBrush = Qt::green;

	//"mark" painting parameters
	markPenWidth = 2;
	markPenBrush = Qt::red;
	markSymbolSize = 12;
	markSymbolPenBrush = Qt::red;
	markSymbol = 0;
}

bool Cubic_Spline_Regression::setRawData(const QVector<double> rawData[2]){
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

	std_number = std_count;
	ableToGeneratedCurve = true;

	return true;
}


bool Cubic_Spline_Regression::generateMark(QwtPlotCurve* mark){
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

	for (int i=1; i<std_number; ++i) {
		calculatedMarkData[0].append(log(processedData[0].at(i)));
		calculatedMarkData[1].append(processedData[1].at(i)/processedData[1].at(0));
	}
	mark->setSamples(calculatedMarkData[0],calculatedMarkData[1]);

	return true;
}

void Cubic_Spline_Regression::setMarkPenWidth(int PenWidth, QwtPlotCurve* mark){
	QPen pen;
	pen.setWidth(PenWidth);
	mark->setPen(pen);
}

void Cubic_Spline_Regression::setMarkPenBrush(QColor BrushColor, QwtPlotCurve* mark){
	QPen pen;
	pen.setBrush(BrushColor);;
	mark->setPen(pen);
}

void Cubic_Spline_Regression::setMarkSymbolPenSize(int SymbolPenSize, QwtPlotCurve* mark){
	markSymbol->setSize(SymbolPenSize);
	mark->setSymbol(markSymbol);
}

void Cubic_Spline_Regression::setMarkSymbolPenBrush(QColor SymbolBrushColor, QwtPlotCurve* mark){
	markSymbol->setColor(SymbolBrushColor);
	mark->setSymbol(markSymbol);
}


bool Cubic_Spline_Regression::generateCurve(QwtPlotCurve* curve){
	if (!ableToGeneratedCurve) return false;

	if (!curve) return false;

	QVector<double> x, y;
	for (int i=1; i<std_number; ++i) {
		x.append(log(processedData[0].at(i)));
		y.append(processedData[1].at(i)/processedData[1].at(0));
	}

	double x_sum = 0, x2_sum = 0, x3_sum = 0, x4_sum = 0, x5_sum = 0, x6_sum = 0;
	double y_sum = 0, xy_sum = 0, x2y_sum = 0, x3y_sum = 0;
	for (int i = 0; i < (std_number-1); ++i) {
		x_sum += (x.at(i));
		x2_sum += pow(x.at(i),2);
		x3_sum += pow(x.at(i),3);
		x4_sum += pow(x.at(i),4);
		x5_sum += pow(x.at(i),5);
		x6_sum += pow(x.at(i),6);

		y_sum += y.at(i);
		xy_sum += (x.at(i)*y.at(i));
		x2y_sum += (pow(x.at(i),2)*y.at(i));
		x3y_sum += (pow(x.at(i),3)*y.at(i));
	}

	double n = std_number-1;
	double m_data[4][5];

	m_data[0][0] = n;
	m_data[0][1] = x_sum;
	m_data[0][2] = x2_sum;
	m_data[0][3] = x3_sum;
	m_data[0][4] = y_sum;

	m_data[1][0] = x_sum;
	m_data[1][1] = x2_sum;
	m_data[1][2] = x3_sum;
	m_data[1][3] = x4_sum;
	m_data[1][4] = xy_sum;

	m_data[2][0] = x2_sum;
	m_data[2][1] = x3_sum;
	m_data[2][2] = x4_sum;
	m_data[2][3] = x5_sum;
	m_data[2][4] = x2y_sum;

	m_data[3][0] = x3_sum;
	m_data[3][1] = x4_sum;
	m_data[3][2] = x5_sum;
	m_data[3][3] = x6_sum;
	m_data[3][4] = x3y_sum;

	Matrix45 matrix45;
	matrix45.Init(m_data);

	if (!coefficient) coefficient = new QVector<double>;
	if (coefficient) matrix45.Solve(coefficient);


	double residual_sum=0, total_sum = 0;
	for (int i = 0; i < (std_number-1); ++i) {
		residual_sum += pow(y.at(i)-coefficient->at(0)-coefficient->at(1)*x.at(i)-coefficient->at(2)*x.at(i)*x.at(i)-coefficient->at(3)*x.at(i)*x.at(i)*x.at(i),2);
		total_sum += (y.at(i) - (y_sum/(std_number-1))) *(y.at(i) - (y_sum/(std_number-1)));
	}

	if (total_sum !=0)
		residual_SD = 1 - (residual_sum/total_sum);


	double a = coefficient->at(3);
	double b = coefficient->at(2);
	double c = coefficient->at(1);
	double d = coefficient->at(0);
	double strPoint = (-sqrt(4*b*b-12*a*c)-2*b)/(6*a);

	this->inflection_point = exp(-b/(3*a));

	calculatedCurveData[0].append(strPoint);
	calculatedCurveData[1].append(d+c*strPoint+b*pow(strPoint,2)+a*pow(strPoint,3));

	double increment = 0.001;
	int count = 0;
	double tempConc = 0, logTempConc = 0;
	do {
		++count;
		tempConc = (count*increment)+exp(strPoint);
		logTempConc = log(tempConc);
		calculatedCurveData[0].append(logTempConc);
		calculatedCurveData[1].append(d+c*logTempConc+b*pow(logTempConc,2)+a*pow(logTempConc,3));

	} while ((calculatedCurveData[1].at(count)*processedData[1].at(0))>0.1 && 
		calculatedCurveData[1].at(count)<calculatedCurveData[1].at(count-1));

	//set the curve attributes
	QPen pen;  // creates a default pen
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(curvePenWidth);
	pen.setBrush(curvePenBrush);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	curve->setPen(pen);
	curve->setRenderHint(QwtPlotCurve::RenderAntialiased);
	curve->setSamples(calculatedCurveData[0],calculatedCurveData[1]);

	curveGenerated = true;
	return true;
}

const QVector<double> Cubic_Spline_Regression::Coefficient(){
	return *coefficient;
}

const double Cubic_Spline_Regression::Regression_Coefficient(){
	return sqrt(residual_SD);
}

const double Cubic_Spline_Regression::Standard_Deviation() {
	return residual_SD;
}

const int Cubic_Spline_Regression::Standard_Number(){
	return std_number;
}

double Cubic_Spline_Regression::BisectFunct(double guessedLogConc, double OD){
	double a = coefficient->at(3);
	double b = coefficient->at(2);
	double c = coefficient->at(1);
	double d = coefficient->at(0) - (OD/processedData[1].at(0));

	return a*pow(guessedLogConc,3)+b*pow(guessedLogConc,2)+c*guessedLogConc+d;
}

double Cubic_Spline_Regression::calculateConc(double OD, double dilutionFactor){
	if (!ableToGeneratedCurve) return 0;

	double curveBottomLimit = 0, curveUpLimit = 0;
	curveBottomLimit = calculatedCurveData[1].at(calculatedCurveData[1].size()-1)*processedData[1].at(0);
	curveUpLimit = calculatedCurveData[1].at(0)*processedData[1].at(0);

	if (OD<=curveUpLimit && OD>=curveBottomLimit) {
		double nearHighConc = 0, nearLowConc = calculatedCurveData[0].at(0);
		int i = 0;
		while (i<calculatedCurveData[0].size() && (OD/processedData[1].at(0))<calculatedCurveData[1].at(i)) {
			++i;
		}

		nearHighConc = calculatedCurveData[0].at(i);
		if (i>=1) nearLowConc = calculatedCurveData[0].at(i-1);

		double epislon = 0.01;
		double x_median = 0;

		do {
			x_median = 0.5*(nearHighConc+nearLowConc);
			if (BisectFunct(x_median,OD)*BisectFunct(nearHighConc,OD) > 0)
				nearHighConc = x_median;
			else nearLowConc = x_median;
		} while (fabs(exp(nearHighConc)-exp(nearLowConc))>epislon);

		return exp(x_median)*dilutionFactor;
	}

	if (OD>= processedData[1].at(0)) return 0;
	
	if (OD>curveUpLimit && OD<processedData[1].at(0)){
		double b = processedData[1].at(0);
		double a = (calculatedCurveData[1].at(0)-1)*b/exp(calculatedCurveData[0].at(0));
		return (OD-b)/a;
	}

	if (OD>0.01 && OD<curveBottomLimit) {
		double temp1 = calculateConc(2*curveBottomLimit-OD,1);
		double temp2 = exp(calculatedCurveData[0].at(calculatedCurveData[0].size()-1));
		return pow((16*temp2*temp2*temp2*temp2-temp1*temp1*temp1*temp1),(0.25))*dilutionFactor;
	}
	return 0;
}

double Cubic_Spline_Regression::calculateConc(double OD1, double OD2, double dilutionFactor){
	return calculateConc((OD1+OD2)*0.5,dilutionFactor);
}

double Cubic_Spline_Regression::IC50() {
	//return calculateConc(processedData[1].at(0)/2,1);
	return Inflection_Point();
}

void Cubic_Spline_Regression::Linear_Range(double range[2]) {
	if (!curveGenerated) {
		QwtPlotCurve tempCurve;
		this->generateCurve(&tempCurve);
	}

	/*
	QwtPlotCurve *linear_curve = new QwtPlotCurve();
	Linear_Regression *linearRegression = new Linear_Regression();
	if (linearRegression->setRawData(processedData)) 
		linearRegression->generateCurve(linear_curve);

	double curveBottomLimit = 0, curveUpLimit = 0;
	curveBottomLimit = calculatedCurveData[1].at(calculatedCurveData[1].size()-1)*processedData[1].at(0);
	curveUpLimit = calculatedCurveData[1].at(0)*processedData[1].at(0);

	double lowLimit = 0, highLimit = 0, linear_conc = 0, spline_conc = 0;
	double relativeDiff = 0;
	int count = 0;
	double tempOD = 0;
	do {
		++count;
		tempOD = curveUpLimit-count*0.01;
		linear_conc = linearRegression->calculateConc(tempOD,1);
		spline_conc = this->calculateConc(tempOD,1);
		double diff = fabs(spline_conc-linear_conc);
		relativeDiff = diff/linear_conc;
	} while (relativeDiff>0.03 && tempOD >= curveBottomLimit);
	lowLimit = linear_conc;

	count = 0;
	do {
		++count;
		tempOD = curveBottomLimit+count*0.01;
		linear_conc = linearRegression->calculateConc(tempOD,1);
		spline_conc = this->calculateConc(tempOD,1);
		double diff = fabs(spline_conc-linear_conc);
		relativeDiff = diff/linear_conc;
	} while (relativeDiff>0.03 && tempOD <= curveUpLimit);
	highLimit = linear_conc;

	range[0] = lowLimit;
	range[1] = highLimit;

	delete linear_curve;
	delete linearRegression;*/

	double a = this->coefficient->at(3);
	double b = this->coefficient->at(2);

	double Limit1 = 0, Limit2 = 0;
	Limit1 = exp((0.1-2*b)/(6*a));
	Limit2 = exp((-0.1-2*b)/(6*a));
	range[0] = Limit1;
	range[1] = Limit2;
}


Cubic_Spline_Regression::~Cubic_Spline_Regression(void)
{
}
