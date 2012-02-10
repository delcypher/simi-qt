#include <QtGui>

#include "mainwindow.h"
#include <iostream>



MainWindow::MainWindow() : mFilepath(""), currentSlice(0)
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

void MainWindow::on_actionOpen_Image_triggered()
{
	QString newImagePath;

	std::cerr << "Opening image" << std::endl;
	//check okay to open image
	if(!isWindowModified())
	{

		//Should do Image clean up here!
		if(!reader == 0)
		{
			reader->CloseVTKFile();
		}


		newImagePath = QFileDialog::getOpenFileName(this,
							    tr("Open Image"),
							    tr("VTK Structured points (*.vtk)"));
		if(!newImagePath.isEmpty())
		{
			//Okay so load the image
			mFilepath = newImagePath;
			loadImage();
		}

	}
	else
	{
		// offer to save stuff
	}
}


void MainWindow::on_actionSlice_up_triggered()
{
	imageView->SetSlice(++currentSlice);

}


void MainWindow::on_actionSlice_down_triggered()
{
	imageView->SetSlice(--currentSlice);

}

bool MainWindow::loadImage()
{
	reader->SetFileName(mFilepath.toStdString().c_str());
	reader->Update();

	if(!reader->IsFileStructuredPoints())
	{
		std::cerr << "Error invalid file?" << std::endl;
	}


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
