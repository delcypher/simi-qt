#include <QtGui>

#include "mainwindow.h"

MainWindow::MainWindow(QString& filepath) : mFilepath(filepath)
{
	ui = new Ui::MainWindow;
	ui->setupUi(this); //set up user interface

}

MainWindow::~MainWindow()
{
	delete ui;
}
