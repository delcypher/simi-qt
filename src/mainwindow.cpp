#include <QtGui>

#include "mainwindow.h"
#include <iostream>
#include <cstring>
#include <QDebug>
#include <QTextStream>
#include "compiletimeconstants.h"


MainWindow::MainWindow() : imageInfo(""), workPath(QDir::home())
{
	ui = new Ui::MainWindow;
	ui->setupUi(this); //set up user interface


	//Setup About Qt Dialog
	connect(ui->actionAboutQt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));


	reader = vtkStructuredPointsReader::New();
	imageView = vtkImageViewer2::New();

    setWindowTitle(PROGRAM_NAME);

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
		newImagePath = QFileDialog::getOpenFileName(this,
							    tr("Open Image"),
                                workPath.absolutePath(),
							    tr("VTK Structured points (*.vtk)"));

		if(!newImagePath.isEmpty())
		{
            imageInfo.setFile(newImagePath);

            //Load the image
            if(!loadImage())
            {
                qDebug() << "Loading image failed!";
                return ;
            }

            //Update the work path to the location of the new image
            workPath.setPath(imageInfo.absolutePath());


			setWindowModified(false); //because we've loaded new image nothing can be modified

            //set window title
            QString newWindowTitle(PROGRAM_NAME);
            newWindowTitle.append(" - ").append(imageInfo.fileName());
            setWindowTitle(newWindowTitle);

		}

	}
	else
	{
		// offer to save stuff
	}
}


void MainWindow::on_actionSlice_up_triggered()
{
    if(imageInfo.exists() && imageView != 0 && imageView->GetSliceMax() != imageView->GetSlice())
        imageView->SetSlice(imageView->GetSlice() +1);



}


void MainWindow::on_actionSlice_down_triggered()
{
    if(imageInfo.exists() && imageView != 0 && imageView->GetSliceMin() != imageView->GetSlice())
        imageView->SetSlice(imageView->GetSlice() -1);

}


void MainWindow::on_actionAbout_triggered()
{
    QString about;
    QTextStream aboutTS(&about);

    aboutTS << "<h2>" << PROGRAM_NAME << "</h2>" <<
             "<p><b>Version:</b> " << PROGRAM_VERSION << "</p>" <<
             "<p>" << PROGRAM_NAME << " is a program for structure identification in medical images.</p>" <<
             "<p><b>Authors</b>:<br>" << PROGRAM_AUTHORS << "</p>";

    QMessageBox::about(this,tr("About ") + PROGRAM_NAME, about);
}

bool MainWindow::loadImage()
{

    //If We had image open before we should clean up
    if(!reader == 0)
    {
        qDebug() << "Attempting Image clean up";
        reader->CloseVTKFile();
    }



    reader->SetFileName(imageInfo.absoluteFilePath().toAscii());
	reader->Update();

	if(!reader->IsFileStructuredPoints())
	{
        qWarning() << "Error invalid file";
        return false;
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
    imageView->SetSlice(0);
	imageView->SetupInteractor(ui->qvtkWidget->GetRenderWindow()->GetInteractor());

    return true;
}
