#include <QtGui>

#include "mainwindow.h"
#include <iostream>



MainWindow::MainWindow(QString& filepath) : mFilepath(filepath), currentSlice(0)
{
	ui = new Ui::MainWindow;
	ui->setupUi(this); //set up user interface

	//Setup About Qt Dialog
	connect(ui->actionAboutQt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));


	reader = vtkStructuredPointsReader::New();
	imageView = vtkImageViewer2::New();







}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_doSomething_clicked()
{
	reader->SetFileName(mFilepath.toStdString().c_str());
	reader->Update();
	imageView->SetInputConnection(reader->GetOutputPort());



	double range[2];
	reader->GetOutput()->GetScalarRange(range);

       // vtkSmartPointer<vtkRenderWindowInteractor> iren =
    //   vtkSmartPointer<vtkRenderWindowInteractor>::New();
       // image_view->SetupInteractor( iren );
	imageView->GetRenderer()->ResetCamera();

	imageView->SetColorLevel((range[0]+range[1])/2.0); //half way point in data set
	imageView->SetColorWindow(range[0]-range[1]); //width in dataset to use

	ui->qvtkWidget->SetRenderWindow(imageView->GetRenderWindow());
	imageView->SetSlice(currentSlice);
	imageView->SetupInteractor(ui->qvtkWidget->GetRenderWindow()->GetInteractor());


}


void MainWindow::on_actionSlice_up_triggered()
{
	std::cerr << "hello";
	imageView->SetSlice(++currentSlice);

}


void MainWindow::on_actionSlice_down_triggered()
{
	std::cerr << "hello222";
	imageView->SetSlice(--currentSlice);

}
