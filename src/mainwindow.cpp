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

	connect(this,SIGNAL(sliceChanged(int)), ui->sliceSpinBox, SLOT(setValue(int)));
	connect(this,SIGNAL(sliceChanged(int)), ui->sliceSlider, SLOT(setValue(int)));


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
	{
		imageView->SetSlice(imageView->GetSlice() +1);
		emit sliceChanged(imageView->GetSlice());
	}
}


void MainWindow::on_actionSlice_down_triggered()
{
	if(imageInfo.exists() && imageView != 0 && imageView->GetSliceMin() != imageView->GetSlice())
	{
		imageView->SetSlice(imageView->GetSlice() -1);
		emit sliceChanged(imageView->GetSlice());
	}

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


	// vtkSmartPointer<vtkRenderWindowInteractor> iren =
	//   vtkSmartPointer<vtkRenderWindowInteractor>::New();
	// image_view->SetupInteractor( iren );
	imageView->GetRenderer()->ResetCamera();


	ui->qvtkWidget->SetRenderWindow(imageView->GetRenderWindow());
	imageView->SetSlice(0);
	imageView->SetupInteractor(ui->qvtkWidget->GetRenderWindow()->GetInteractor());


	contrastControlSetup();
	sliceControlSetup();

	return true;
}



void MainWindow::contrastControlSetup()
{
	double range[2]; //[0]:min, [1]:max
	reader->GetOutput()->GetScalarRange(range);
	//setup the ranges on the contrast sliders appropriately
	qDebug() << "Min intensity range:" <<  range[0] << ", Max intensity:" << range[1] ;
	ui->minIntensitySlider->setRange(static_cast<int>(range[0]),static_cast<int>(range[1]));
	ui->maxIntensitySlider->setRange(static_cast<int>(range[0]),static_cast<int>(range[1]));
	ui->minIntensitySpinBox->setRange(static_cast<int>(range[0]),static_cast<int>(range[1]));
	ui->maxIntensitySpinBox->setRange(static_cast<int>(range[0]),static_cast<int>(range[1]));

	imageView->SetColorLevel((range[0]+range[1])/2.0); //half way point in data set
	imageView->SetColorWindow(range[1]-range[0]); //width in dataset to use

	ui->minIntensitySlider->setValue(static_cast<int>(range[0]));
	ui->maxIntensitySlider->setValue(static_cast<int>(range[1]));

}

void MainWindow::changeContrast()
{
	imageView->SetColorLevel( (static_cast<double>(ui->maxIntensitySpinBox->value()) + static_cast<double>(ui->minIntensitySpinBox->value()) )/2.0 );
	imageView->SetColorWindow(  static_cast<double>(ui->maxIntensitySpinBox->value()) - static_cast<double>(ui->minIntensitySpinBox->value()) );
	ui->qvtkWidget->update();
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
	if(imageView != 0)
		imageView->SetSlice(value);
}

void MainWindow::sliceControlSetup()
{
	ui->sliceSlider->setRange(imageView->GetSliceMin(), imageView->GetSliceMax() );
	ui->sliceSpinBox->setRange(imageView->GetSliceMin(), imageView->GetSliceMax() );
}
