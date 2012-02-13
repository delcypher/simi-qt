#ifndef CONTRAST_DIALOG_H
#define CONTRAST_DIALOG_H

#include <QDialog>
#include "ui_contrastdialog.h"


class ContrastDialog : public QDialog
{
	Q_OBJECT

	public:
		ContrastDialog(QWidget* parent=0);
		~ContrastDialog();
		bool setIntensityRange(double minI,double maxI);


	signals:
		void contrastChanged(double colourWindow, double colourLevel);


private slots:
		void on_minIntensitySpinBox_valueChanged(int minI );

		void on_maxIntensitySpinBox_valueChanged(int maxI );

private:
		Ui::ContrastDialog* ui;
		double minimumIntensity;
		double maximumIntensity;
		double calculateColourWindow();
		double calculateColourLevel();


};

#endif
