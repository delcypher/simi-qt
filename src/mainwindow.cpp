#include <QtGui>

#include "mainwindow.h"
#include <iostream>
#include <cstring>
#include <QDebug>
#include <QTextStream>
#include "compiletimeconstants.h"
#include "vtkCamera.h"


MainWindow::MainWindow() : imageInfo(""), workPath(QDir::home())
{
	ui = new Ui::MainWindow;
	ui->setupUi(this); //set up user interface

	seedManager=0;
	imageManager=0;
	layoutManager=0;

	//Setup About Qt Dialog
	connect(ui->actionAboutQt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));


	setWindowTitle(PROGRAM_NAME);

}

MainWindow::~MainWindow()
{
	delete ui;

	if(imageManager!=0)
        delete imageManager;

	if(seedManager!=0)
        delete seedManager;

	if(layoutManager!=0)
        delete layoutManager;
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

			//Setup the ImagePairManager
			if(imageManager!=0)
				delete imageManager;

			imageManager = new ImagePairManager();

			if(!(imageManager->loadImage(imageInfo)))
			{
				qDebug() << "Failed to open" << imageInfo.absoluteFilePath();
				return;
			}

			//setup SeedPointManager
			if(seedManager!=0)
				delete seedManager;
			seedManager = new SeedPointManager(imageManager->getZDim());

			//setup LayoutManager
			if(layoutManager!=0)
				delete layoutManager;
			layoutManager = new LayoutManager(imageManager,ui->qvtkWidget);

			sliceControlSetup();
			contrastControlSetup();

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

}


void MainWindow::on_actionSlice_down_triggered()
{


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




void MainWindow::contrastControlSetup()
{
//	double range[2]; //[0]:min, [1]:max
//	reader->GetOutput()->GetScalarRange(range);
//	//setup the ranges on the contrast sliders appropriately
//	qDebug() << "Min intensity range:" <<  range[0] << ", Max intensity:" << range[1] ;
	if(imageManager!=0)
	{
        ui->minIntensitySlider->setRange(static_cast<int>(imageManager->getMinimumIntensity()),static_cast<int>(imageManager->getMaximumIntensity()));
        ui->maxIntensitySlider->setRange(static_cast<int>(imageManager->getMinimumIntensity()),static_cast<int>(imageManager->getMaximumIntensity()));
        ui->minIntensitySpinBox->setRange(static_cast<int>(imageManager->getMinimumIntensity()),static_cast<int>(imageManager->getMaximumIntensity()));
        ui->maxIntensitySpinBox->setRange(static_cast<int>(imageManager->getMinimumIntensity()),static_cast<int>(imageManager->getMaximumIntensity()));
    }
//	imageView->SetColorLevel((range[0]+range[1])/2.0); //half way point in data set
//	imageView->SetColorWindow(range[1]-range[0]); //width in dataset to use

    //Doing this change should cause the valueChanged() signal to be emitted by the sliders
    ui->minIntensitySlider->setValue(static_cast<int>(imageManager->getMinimumIntensity()));
    ui->maxIntensitySlider->setValue(static_cast<int>(imageManager->getMaximumIntensity()));

}

void MainWindow::changeContrast()
{
    if(layoutManager!=0)
    {
        layoutManager->setConstrast(static_cast<double>(ui->minIntensitySlider->value()), static_cast<double>(ui->maxIntensitySlider->value()));
    }
}



void MainWindow::on_minIntensitySlider_valueChanged(int value)
{
	//prevent minimum being > maximum
	if(value > ui->maxIntensitySpinBox->value())
		ui->minIntensitySpinBox->setValue(ui->maxIntensitySpinBox->value());

   changeContrast();
}

void MainWindow::on_maxIntensitySlider_valueChanged(int value)
{
	//prevent maximum being < minimum
	if(value < ui->minIntensitySpinBox->value())
		ui->maxIntensitySpinBox->setValue(ui->minIntensitySpinBox->value());

	changeContrast();
}


void MainWindow::on_sliceSlider_valueChanged(int value)
{
	if(layoutManager!=0)
		layoutManager->ChangeSlice(value);
}

void MainWindow::sliceControlSetup()
{
	if(imageManager!=0)
	{
		ui->sliceSlider->setRange(imageManager->getZMin(),imageManager->getZMax());
		ui->sliceSpinBox->setRange(imageManager->getZMin(),imageManager->getZMax());

	}
}
