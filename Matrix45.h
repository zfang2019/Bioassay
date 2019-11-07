#ifndef MATRIX45_H
#define MATRIX54_H

#include <cmath>
#include <qvector.h>

class Matrix45
{
public:
	Matrix45(void);
	void Init(double data[4][5]);
	void Solve(QVector<double> *coefficient);
	~Matrix45(void);
private:
	double d[4][5];
	bool initialized;
};

#endif

