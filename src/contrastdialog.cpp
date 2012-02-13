#include "contrastdialog.h"

ContrastDialog::ContrastDialog(QWidget* parent): QDialog(parent)
{
	ui = new Ui::ContrastDialog;
	ui->setupUi(this);

	setWindowTitle(tr("Image Contrast"));
}

ContrastDialog::~ContrastDialog()
{
	delete ui;
}


bool ContrastDialog::setIntensityRange(double minI, double maxI)
{
	if(minI >= maxI)
		return false;

	ui->maxIntensitySlider->setRange(minI,maxI);
	ui->minIntensitySlider->setRange(minI,maxI);
	ui->maxIntensitySpinBox->setRange(minI,maxI);
	ui->minIntensitySpinBox->setRange(minI,maxI);
	return true;
}

double ContrastDialog::calculateColourWindow()
{
	return  static_cast<double>(ui->maxIntensitySpinBox->value()) - static_cast<double>(ui->minIntensitySpinBox->value());
}

double ContrastDialog::calculateColourLevel()
{
	return  ( static_cast<double>(ui->maxIntensitySpinBox->value()) + static_cast<double>(ui->minIntensitySpinBox->value()) )/2.0 ;
}



void ContrastDialog::on_minIntensitySpinBox_valueChanged(int minI)
{
    //prevent minimum being > maximum
    if(minI > ui->maxIntensitySpinBox->value())
	    ui->minIntensitySpinBox->setValue(ui->maxIntensitySpinBox->value());

    emit contrastChanged(calculateColourWindow(),calculateColourLevel());
}

void ContrastDialog::on_maxIntensitySpinBox_valueChanged(int maxI)
{
	//prevent maximum being < minimum
	if(maxI < ui->minIntensitySpinBox->value())
		ui->maxIntensitySpinBox->setValue(ui->minIntensitySpinBox->value());

	emit contrastChanged(calculateColourWindow(),calculateColourLevel());
}
