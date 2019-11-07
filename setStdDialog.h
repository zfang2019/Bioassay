#ifndef SETSTDDIALOG_H
#define SETSTDDIALOG_H

#include <qdialog.h>
#include <qvector.h>

class QLabel;
class QLineEdit;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

class setStdDialog : public QDialog
{
	Q_OBJECT
public:
	setStdDialog(QStringList stdPosition,QVector<double> ODValues,QWidget *parent = 0);
	~setStdDialog(void);
//private:
	QLabel *cellHeader,*ODHeader,*concHeader;
	QVector<QLabel*> position,ODValue;
	QVector<QLineEdit*> lineEdits;
	QPushButton *OKButton;
	QPushButton *cancelButton;
	QGridLayout *gridLayout;
	QHBoxLayout *bottomLayout;
	QVBoxLayout *mainDialogLayout;
	
protected:
	void keyPressEvent(QKeyEvent *event);
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
};
#endif
