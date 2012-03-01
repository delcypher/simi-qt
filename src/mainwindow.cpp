#include <QtGui>

#include "mainwindow.h"
#include <iostream>
#include <cstring>
#include <QString>
#include <QTextStream>
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
	viewManager=0;
	drawManager=0;
	segmenter=0;



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

	if(viewManager!=0)
        delete viewManager;

	if(drawManager!=0)
        delete drawManager;

	if(segmenter!=0)
		delete segmenter;
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
			if(viewManager!=0)
				delete viewManager;
			viewManager = new ViewManager(imageManager,ui->qvtkWidget);

			//setup drawManager
			if(drawManager!=0)
				delete drawManager;
			drawManager = new DrawManager(imageManager);

			//setup segmenter
			if(segmenter!=0)
				delete segmenter;
			segmenter = new Segmenter(seedManager,imageManager);



			sliceControlSetup();
			contrastControlSetup();
			toolbarSetup();
			segmentationControlSetup();

			//setup statusbar update from viewmanager
			connect(viewManager,SIGNAL(mouseHasMoved()), this, SLOT(updateStatusBar()));

			//setup zoom control
			connect(ui->actionZoom_in,SIGNAL(triggered()), viewManager,SLOT(zoomIn()));
			connect(ui->actionZoom_out,SIGNAL(triggered()), viewManager,SLOT(zoomOut()));

			//setup so on segmentation completion we redraw
			connect(segmenter,SIGNAL(segmentationDone(int)), viewManager, SLOT(update()));

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
    if(viewManager!=0)
        viewManager->ChangeSlice( viewManager->getCurrentSlice() +1);
}


void MainWindow::on_actionSlice_down_triggered()
{
    if(viewManager!=0)
        viewManager->ChangeSlice( viewManager->getCurrentSlice() -1);
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

	if(imageManager!=0)
	{
		ui->minIntensitySlider->setRange(static_cast<int>(imageManager->getMinimumIntensity()),static_cast<int>(imageManager->getMaximumIntensity()));
		ui->maxIntensitySlider->setRange(static_cast<int>(imageManager->getMinimumIntensity()),static_cast<int>(imageManager->getMaximumIntensity()));
		ui->minIntensitySpinBox->setRange(static_cast<int>(imageManager->getMinimumIntensity()),static_cast<int>(imageManager->getMaximumIntensity()));
		ui->maxIntensitySpinBox->setRange(static_cast<int>(imageManager->getMinimumIntensity()),static_cast<int>(imageManager->getMaximumIntensity()));
	}


	//Doing this change should cause the valueChanged() signal to be emitted by the sliders
	ui->minIntensitySlider->setValue(static_cast<int>(imageManager->getMinimumIntensity()));
	ui->maxIntensitySlider->setValue(static_cast<int>(imageManager->getMaximumIntensity()));

}

void MainWindow::changeContrast()
{
    if(viewManager!=0)
    {
        viewManager->setConstrast(static_cast<double>(ui->minIntensitySlider->value()), static_cast<double>(ui->maxIntensitySlider->value()));
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
	if(viewManager!=0)
        viewManager->ChangeSlice(value);
}

void MainWindow::on_actionHandTool_triggered()
{
    qDebug() << "Hand tool activated";

    //disable other connections
    //disable seedTool connection
    disconnect(viewManager,
		SIGNAL(viewLeftClicked(int,int,int)),
		seedManager,
		SLOT(setSeedPoint(int,int,int))
		);

    //disable penTool connection
    disconnect(viewManager,
		SIGNAL(dragEvent(int,int,int)),
		drawManager,
		SLOT(draw(int,int,int))
		);

    //disable eraseTool connection
    disconnect(viewManager,
		SIGNAL(dragEvent(int,int,int)),
		drawManager,
		SLOT(erase(int,int,int))
		);

}

void MainWindow::on_actionPenTool_triggered()
{
    qDebug() << "Pen tool activated";

    //disable other connections
    //disable seedTool connection
    disconnect(viewManager,
		SIGNAL(viewLeftClicked(int,int,int)),
		seedManager,
		SLOT(setSeedPoint(int,int,int))
		);

    //disable eraseTool connection
    disconnect(viewManager,
		SIGNAL(dragEvent(int,int,int)),
		drawManager,
		SLOT(erase(int,int,int))
		);

    //enable connection
    connect(viewManager,
		SIGNAL(dragEvent(int,int,int)),
		drawManager,
		SLOT(draw(int,int,int))
		);
}

void MainWindow::on_actionCrosshairTool_triggered()
{
    qDebug() << "Crosshair tool activated";

    //disable other connections
    //disable seedTool connection
    disconnect(viewManager,
		SIGNAL(viewLeftClicked(int,int,int)),
		seedManager,
		SLOT(setSeedPoint(int,int,int))
		);

    //disable penTool connection
    disconnect(viewManager,
		SIGNAL(dragEvent(int,int,int)),
		drawManager,
		SLOT(draw(int,int,int))
		);

    //disable eraseTool connection
    disconnect(viewManager,
		SIGNAL(dragEvent(int,int,int)),
		drawManager,
		SLOT(erase(int,int,int))
		);



    //enable connection
    connect(viewManager,
		SIGNAL(viewLeftClicked(int,int,int)),
		seedManager,
		SLOT(setSeedPoint(int,int,int))
		);

}

void MainWindow::on_actionEraseTool_triggered()
{
    qDebug() << "Erase tool activated";

    //disable seedTool connection
    disconnect(viewManager,
		SIGNAL(viewLeftClicked(int,int,int)),
		seedManager,
		SLOT(setSeedPoint(int,int,int))
		);

    //disable penTool connection
    disconnect(viewManager,
		SIGNAL(dragEvent(int,int,int)),
		drawManager,
		SLOT(draw(int,int,int))
		);

    //enable connection
    connect(viewManager,
		SIGNAL(dragEvent(int,int,int)),
		drawManager,
		SLOT(erase(int,int,int))
		);



}

void MainWindow::sliceControlSetup()
{
	if(imageManager!=0)
	{
		ui->sliceSlider->setRange(imageManager->getZMin(),imageManager->getZMax());
		ui->sliceSpinBox->setRange(imageManager->getZMin(),imageManager->getZMax());

		connect(viewManager,SIGNAL(sliceChanged(int)),ui->sliceSlider,SLOT(setValue(int)));

    }
}

void MainWindow::toolbarSetup()
{
    //enable the toolbar buttons to be used
    ui->toolBar->setEnabled(true);

    //setup mutually exclusive behaviour
    toolbarActions = new QActionGroup(this);

    toolbarActions->addAction(ui->actionHandTool);
    toolbarActions->addAction(ui->actionPenTool);
    toolbarActions->addAction(ui->actionCrosshairTool);
    toolbarActions->addAction(ui->actionEraseTool);

    //set default action
    ui->actionHandTool->setChecked(true);
}

void MainWindow::segmentationControlSetup()
{
	if(imageManager!=0)
	{
		ui->minSegIntensitySlider->setRange(static_cast<int>(imageManager->getMinimumIntensity()),static_cast<int>(imageManager->getMaximumIntensity()));
		ui->minSegIntensitySpinBox->setRange(static_cast<int>(imageManager->getMinimumIntensity()),static_cast<int>(imageManager->getMaximumIntensity()));
		ui->maxSegIntensitySlider->setRange(static_cast<int>(imageManager->getMinimumIntensity()),static_cast<int>(imageManager->getMaximumIntensity()));
		ui->maxSegIntensitySpinBox->setRange(static_cast<int>(imageManager->getMinimumIntensity()),static_cast<int>(imageManager->getMaximumIntensity()));
	}


	//Doing this change should cause the valueChanged() signal to be emitted by the sliders
	ui->minSegIntensitySlider->setValue(static_cast<int>(imageManager->getMinimumIntensity()));
	ui->maxSegIntensitySlider->setValue(static_cast<int>(imageManager->getMaximumIntensity()));
}

void MainWindow::on_minSegIntensitySlider_valueChanged(int value)
{
    //make sure min is not > max
    if(value > ui->maxSegIntensitySlider->value())
	    ui->minSegIntensitySlider->setValue( ui->maxSegIntensitySlider->value());

}

void MainWindow::on_maxSegIntensitySlider_valueChanged(int value)
{
	//make sure max is not < min
	if(value < ui->minSegIntensitySlider->value())
		ui->maxSegIntensitySlider->setValue( ui->minSegIntensitySlider->value());
}

void MainWindow::on_doSegmentation_clicked()
{
	if(segmenter!=0)
		segmenter->doSegmentation(viewManager->getCurrentSlice(), ui->minSegIntensitySlider->value(), ui->maxSegIntensitySlider->value());
}

void MainWindow::updateStatusBar()
{
	QString message("");
	QTextStream messageStream(&message);

	messageStream << "X:" << viewManager->getLastMousePosX() << " Y:"<< viewManager->getLastMousePosY() << " Z:" << viewManager->getLastMousePosZ() << " Intensity:" << viewManager->getLastMouseIntensity();

	ui->statusbar->showMessage(*(messageStream.string()),0);
}
