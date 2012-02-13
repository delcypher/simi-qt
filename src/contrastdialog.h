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

	private:
		Ui::ContrastDialog* ui;
};

#endif
