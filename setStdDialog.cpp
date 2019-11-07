#include "setStdDialog.h"
#include <QtGui>


setStdDialog::setStdDialog(QStringList stdPosition,QVector<double> ODValues,QWidget *parent):QDialog(parent)
{
	mainDialogLayout = new QVBoxLayout(this);
	gridLayout = new QGridLayout(this);
	cellHeader = new QLabel("Cell",this);
	ODHeader = new QLabel("OD Value",this);
	concHeader = new QLabel("Concentration",this);
	gridLayout->addWidget(cellHeader,0,0,1,2,Qt::AlignCenter);
	gridLayout->addWidget(ODHeader,0,2,1,1,Qt::AlignCenter);
	gridLayout->addWidget(concHeader,0,3,1,1,Qt::AlignCenter);
	for (int i=0; i< stdPosition.count();++i) {
		position.append(new QLabel(stdPosition.at(i),this));
		position.at(i)->setVisible(true);
		QString ODString;
		ODValue.append(new QLabel(ODString.sprintf("%.4f",ODValues.at(i)),this));
		ODValue.at(i)->setVisible(true);
		lineEdits.append(new QLineEdit(this));
		lineEdits.at(i)->setVisible(true);
		lineEdits.at(i)->setAcceptDrops(true);
		lineEdits.at(i)->setDragEnabled(true);
		lineEdits.at(i)->setFocusPolicy(Qt::WheelFocus);
		if (i>0) lineEdits.at(i)->setTabOrder(lineEdits.at(i-1),lineEdits.at(i));
		position.at(i)->setBuddy(lineEdits.at(i));
		gridLayout->addWidget(position.at(i),i+1,0,1,2,Qt::AlignCenter);
		gridLayout->addWidget(ODValue.at(i),i+1,2,1,1,Qt::AlignCenter);
		gridLayout->addWidget(lineEdits.at(i),i+1,3,1,1,Qt::AlignCenter);
	}	
	if (lineEdits.count()>0) this->setFocusProxy(lineEdits.at(0));
	gridLayout->setSpacing(10);


	bottomLayout = new QHBoxLayout(this);
	OKButton = new QPushButton(tr("&OK"),this);
	OKButton->setVisible(true);
	lineEdits.at(lineEdits.count()-1)->setTabOrder(lineEdits.at(lineEdits.count()-1),OKButton);
	cancelButton = new QPushButton(tr("Cancel"),this);
	cancelButton->setVisible(true);
	bottomLayout->addWidget(OKButton);
	bottomLayout->addWidget(cancelButton);

	mainDialogLayout->addLayout(gridLayout);
	mainDialogLayout->addLayout(bottomLayout);
	this->setLayout(mainDialogLayout);
	//this->setMinimumSize(700,700);

	connect(OKButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	//this->update();
}

void setStdDialog::keyPressEvent(QKeyEvent *event)
{
	QLineEdit *temp = qobject_cast<QLineEdit*>(this->focusWidget());
	if (!temp) {
		QWidget::keyPressEvent(event);
		return;
	}
	int index = lineEdits.indexOf(temp);
	switch (event->key()) {
	case Qt::Key_Up:
		if (index>0) lineEdits.at(index-1)->setFocus();
		break;
	case Qt::Key_Down:
		if (index < (lineEdits.count()-1)) lineEdits.at(index+1)->setFocus();
		break;
	}
}

void setStdDialog::paintEvent(QPaintEvent *event)
{
	//mainDialogLayout->setGeometry(this->geometry());
	mainDialogLayout->setSpacing(50);
	mainDialogLayout->setContentsMargins(20,20,20,20);
	mainDialogLayout->setAlignment(Qt::AlignLeft);
	for (int i=0; i< position.count();++i) {
		position.at(i)->show();
		ODValue.at(i)->show();
		lineEdits.at(i)->show();
	}
	OKButton->show();
	cancelButton->show();	
}

void setStdDialog::resizeEvent(QResizeEvent *event)
{
	//mainDialogLayout->setGeometry(this->geometry());
	mainDialogLayout->setSpacing(50);
	mainDialogLayout->setContentsMargins(20,20,20,20);
	mainDialogLayout->setAlignment(Qt::AlignLeft);
	for (int i=0; i< position.count();++i) {
		position.at(i)->show();
		ODValue.at(i)->show();
		lineEdits.at(i)->show();
	}
	OKButton->show();
	cancelButton->show();	
}


setStdDialog::~setStdDialog(void)
{
}
