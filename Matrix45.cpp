#include "Matrix45.h"


Matrix45::Matrix45(void)
{
	for (int row = 0; row <4; ++row)
		for (int column = 0; column<5; ++column)
			d[row][column] = 0;

	initialized = false;
}

void Matrix45::Init(double data[4][5]){
	for (int row = 0; row <4; ++row)
		for (int column = 0; column<5; ++column)
			d[row][column] = data[row][column];

	initialized = true;
}

void Matrix45::Solve(QVector<double> *coefficient){
	if (!initialized) return;

	//step 1: set column  1 in row 2,3,4 to 0
	double factor = d[1][0]/d[0][0];
	for (int i=0; i<5; ++i) d[1][i] -= (factor*d[0][i]);
	factor = d[2][0]/d[0][0];
	for (int i=0; i<5; ++i) d[2][i] -= (factor*d[0][i]);
	factor = d[3][0]/d[0][0];
	for (int i=0; i<5; ++i) d[3][i] -= (factor*d[0][i]);
		
	//step 2: set column 2 in row 3,4 to 0
	factor = d[2][1]/d[1][1];
	for (int i=0; i<5; ++i) d[2][i] -= (factor*d[1][i]);
	factor = d[3][1]/d[1][1];
	for (int i=0; i<5; ++i) d[3][i] -= (factor*d[1][i]);

	//step 3: set column 3 in row 4 to 0
	//        and column 4 in row 4 to 1
	factor = d[3][2]/d[2][2];
	for (int i=0; i<5; ++i) d[3][i] -= (factor*d[2][i]);
	d[3][4] /= d[3][3];
	d[3][3] = 1.00;

	//step 4: set column 4 in row 3 to 0 and column 3 in row 3 to 1
	factor = d[2][3];
	for (int i=0; i<5; ++i) d[2][i] -= (factor*d[3][i]);
	d[2][4] /= d[2][2];
	d[2][2] = 1.00;
	
	//step 5: set column 3,4 in row 2 to 0
	//        and column 2 in row 2 to 1
	factor = d[1][3];
	for (int i=0; i<5; ++i) d[1][i] -= (factor*d[3][i]);
	factor = d[1][2];
	for (int i=0; i<5; ++i) d[1][i] -= (factor*d[2][i]);
	d[1][4] /= d[1][1];
	d[1][1] = 1.00;

	//step 6: set column 4,3,2 in row 1 to 0 and column 1 in row 1 to 1
	factor = d[0][3];
	for (int i=0; i<5; ++i) d[0][i] -= (factor*d[3][i]);
	factor = d[0][2];
	for (int i=0; i<5; ++i) d[0][i] -= (factor*d[2][i]);
	factor = d[0][1];
	for (int i=0; i<5; ++i) d[0][i] -= (factor*d[1][i]);
	d[0][4] /= d[0][0];
	d[0][0] = 1.00;

	//return the solution
	coefficient->append(d[0][4]);
	coefficient->append(d[1][4]);
	coefficient->append(d[2][4]);
	coefficient->append(d[3][4]);

	return;
}

Matrix45::~Matrix45(void)
{
}
