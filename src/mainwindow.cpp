#include <QtGui>

#include "mainwindow.h"

MainWindow::MainWindow(QString& filepath) : mFilepath(filepath)
{
	ui = new Ui::MainWindow;
	ui->setupUi(this); //set up user interface

	//Setup About Qt Dialog
	connect(ui->actionAboutQt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));

}

MainWindow::~MainWindow()
{
	delete ui;
}
