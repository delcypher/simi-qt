#include "contrastdialog.h"

ContrastDialog::ContrastDialog(QWidget* parent) : QDialog(parent)
{
	ui = new Ui::ContrastDialog;
	ui->setupUi(this);

	//make sliderbars and spin boxes insync

}

ContrastDialog::~ContrastDialog()
{
	delete ui;
}
