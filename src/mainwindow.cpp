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

	seedPointManager=0;
	imagePairManager=0;
	viewManager=0;
	drawManager=0;
	segmenter=0;
	volumeRenderManager=0;
	progressDialog=0;

	//Setup About Qt Dialog
	connect(ui->actionAboutQt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));


	setWindowTitle(PROGRAM_NAME);
}

MainWindow::~MainWindow()
{
	delete ui;

	if(imagePairManager!=0)
        delete imagePairManager;

    if(seedPointManager!=0)
        delete seedPointManager;

	if(viewManager!=0)
        delete viewManager;

	if(drawManager!=0)
        delete drawManager;

	if(segmenter!=0)
	delete segmenter;

	if(volumeRenderManager!=0)
	delete volumeRenderManager;
}

void MainWindow::on_actionOpen_Image_triggered()
{
	QString newImagePath;

	std::cerr << "Opening image" << std::endl;

	//check okay to open image
	if(imagePairManager==NULL || okToContinue())
	{
		newImagePath = QFileDialog::getOpenFileName(this,
							tr("Open Image"),
							workPath.absolutePath(),
							tr("VTK Structured points (*.vtk)"));

		if(!newImagePath.isEmpty())
		{
            loadOriginalImage(newImagePath);
		}

	}

}


void MainWindow::on_actionSlice_up_triggered()
{
    if(viewManager!=0)
	viewManager->setSlice( viewManager->getCurrentSlice() +1);
}


void MainWindow::on_actionSlice_down_triggered()
{
    if(viewManager!=0)
	viewManager->setSlice( viewManager->getCurrentSlice() -1);
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

	if(imagePairManager!=0)
	{
		ui->minIntensitySlider->setRange(static_cast<int>(imagePairManager->getMinimumIntensity()),static_cast<int>(imagePairManager->getMaximumIntensity()));
		ui->maxIntensitySlider->setRange(static_cast<int>(imagePairManager->getMinimumIntensity()),static_cast<int>(imagePairManager->getMaximumIntensity()));
		ui->minIntensitySpinBox->setRange(static_cast<int>(imagePairManager->getMinimumIntensity()),static_cast<int>(imagePairManager->getMaximumIntensity()));
		ui->maxIntensitySpinBox->setRange(static_cast<int>(imagePairManager->getMinimumIntensity()),static_cast<int>(imagePairManager->getMaximumIntensity()));
	}

	//Disable keyboard tracking so users can type values in before emit valueChanged() happens. They confirm by pressing enter or loosing focus
	ui->minIntensitySpinBox->setKeyboardTracking(false);
	ui->maxIntensitySpinBox->setKeyboardTracking(false);

	//Doing this change should cause the valueChanged() signal to be emitted by the sliders
	ui->minIntensitySlider->setValue(static_cast<int>(imagePairManager->getMinimumIntensity()));
	ui->maxIntensitySlider->setValue(static_cast<int>(imagePairManager->getMaximumIntensity()));

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
	viewManager->setSlice(value);
}

void MainWindow::on_actionHandTool_triggered()
{
    qDebug() << "Hand tool activated";

    //safety check
    if(viewManager==0 || seedPointManager ==0 || drawManager==0)
        return;

    //disable other connections
    //disable seedTool connection
    disconnect(viewManager,
		SIGNAL(viewLeftClicked(int,int,int)),
        seedPointManager,
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

    //enable panning
    viewManager->enablePanning(true);

}

void MainWindow::on_actionPenTool_triggered()
{
    qDebug() << "Pen tool activated";

    //safety check
    if(viewManager==0 || seedPointManager ==0 || drawManager==0)
        return;

    //disable other connections
    //disable seedTool connection
    disconnect(viewManager,
		SIGNAL(viewLeftClicked(int,int,int)),
        seedPointManager,
		SLOT(setSeedPoint(int,int,int))
		);

    //disable eraseTool connection
    disconnect(viewManager,
		SIGNAL(dragEvent(int,int,int)),
		drawManager,
		SLOT(erase(int,int,int))
		);

    //disable panning
    viewManager->enablePanning(false);

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

    //safety check
    if(viewManager==0 || seedPointManager ==0 || drawManager==0)
        return;

    //disable other connections
    //disable seedTool connection
    disconnect(viewManager,
		SIGNAL(viewLeftClicked(int,int,int)),
        seedPointManager,
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

    //disable panning
    viewManager->enablePanning(false);


    //enable connection
    connect(viewManager,
		SIGNAL(viewLeftClicked(int,int,int)),
        seedPointManager,
		SLOT(setSeedPoint(int,int,int))
		);

}

void MainWindow::on_actionEraseTool_triggered()
{
    qDebug() << "Erase tool activated";

    //safety check
    if(viewManager==0 || seedPointManager ==0 || drawManager==0)
        return;

    //disable seedTool connection
    disconnect(viewManager,
		SIGNAL(viewLeftClicked(int,int,int)),
        seedPointManager,
		SLOT(setSeedPoint(int,int,int))
		);

    //disable penTool connection
    disconnect(viewManager,
		SIGNAL(dragEvent(int,int,int)),
		drawManager,
		SLOT(draw(int,int,int))
		);

    //disable panning
    viewManager->enablePanning(false);

    //enable connection
    connect(viewManager,
		SIGNAL(dragEvent(int,int,int)),
		drawManager,
		SLOT(erase(int,int,int))
		);



}

void MainWindow::sliceControlSetup()
{
	if(imagePairManager!=0)
	{
        ui->sliceSlider->setRange(viewManager->getSliceMin(),viewManager->getSliceMax());
        ui->sliceSpinBox->setRange(viewManager->getSliceMin(),viewManager->getSliceMax());

        ui->sliceSlider->setValue(0);
        ui->sliceSlider->setValue(0);
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
    viewManager->enablePanning(true);
}

void MainWindow::segmentationControlSetup()
{
	if(imagePairManager!=0)
	{
		ui->minSegIntensitySlider->setRange(static_cast<int>(imagePairManager->getMinimumIntensity()),static_cast<int>(imagePairManager->getMaximumIntensity()));
		ui->minSegIntensitySpinBox->setRange(static_cast<int>(imagePairManager->getMinimumIntensity()),static_cast<int>(imagePairManager->getMaximumIntensity()));
		ui->maxSegIntensitySlider->setRange(static_cast<int>(imagePairManager->getMinimumIntensity()),static_cast<int>(imagePairManager->getMaximumIntensity()));
		ui->maxSegIntensitySpinBox->setRange(static_cast<int>(imagePairManager->getMinimumIntensity()),static_cast<int>(imagePairManager->getMaximumIntensity()));
	}

	//Disable keyboard tracking so users can type values in before emit valueChanged() happens. They confirm by pressing enter or loosing focus
	ui->minSegIntensitySpinBox->setKeyboardTracking(false);
	ui->maxSegIntensitySpinBox->setKeyboardTracking(false);

	//Doing this change should cause the valueChanged() signal to be emitted by the sliders
	ui->minSegIntensitySlider->setValue(static_cast<int>(imagePairManager->getMinimumIntensity()));
	ui->maxSegIntensitySlider->setValue(static_cast<int>(imagePairManager->getMaximumIntensity()));
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

void MainWindow::on_doSegmentation2D_clicked()
{
	if(segmenter!=0)
	{
                //disable segmentation widgets whilst segmenting
		disableSegmentationWidgets();
                int pos_z = viewManager->getCurrentSlice();
                int pos_x, pos_y;

                if(!seedPointManager->getSeedPoint(pos_z,pos_x,pos_y))
                {
                    qWarning() << "Can't do segmentation. The seed point isn't set!";
                }

                segmenter->doSegmentation2D(pos_x, pos_y, pos_z, ui->minSegIntensitySlider->value(), ui->maxSegIntensitySlider->value());
		enableSegmentationWidgets();
	}
}

void MainWindow::updateStatusBar()
{
	if(viewManager!=0 && viewManager->mouseIsOverWidget())
	{
		QString message("");
		QTextStream messageStream(&message);
		messageStream << "X:" << viewManager->getLastMousePosX() << " Y:"<< viewManager->getLastMousePosY() << " Z:" << viewManager->getLastMousePosZ() << " Intensity:" << viewManager->getLastMouseIntensity();

		ui->statusbar->showMessage(*(messageStream.string()),0);
	}
	else
		ui->statusbar->showMessage("");
}

void MainWindow::seedPointChanged()
{
	int x=0;
	int y=0;

	//small safety check
    if(seedPointManager!=0 && viewManager!=0)
    {
        seedPointManager->getSeedPoint(viewManager->getCurrentSlice(),x,y);

        QString seedPoint("(");
        seedPoint += QString::number(x);
        seedPoint +=",";
        seedPoint += QString::number(y);
        seedPoint +=")";
        ui->seedPointLineEdit->setText(seedPoint);

        //enable the segmentation widgets
        tryEnableSegmentationWidgets();
	}
}

void MainWindow::on_doSegmentation3D_clicked()
{
        if(segmenter!=0)
        {
                //disable segmentation widgets whilst segmenting
		disableSegmentationWidgets();

                //segmentation parameters
                int pos_z = viewManager->getCurrentSlice();
                int pos_x, pos_y;

                if(!seedPointManager->getSeedPoint(pos_z,pos_x,pos_y))
                {
                    qWarning() << "Can't do segmentation. The seed point isn't set!";
                    return;
                }

                if(segmenter->isWorking())
                {
                    qWarning() << "Can't do segmentation as it is already running";
                    return;
                }

                //Modify the dialog to have a cancel button
                showWaitDialog();
                progressDialog->setCancelButtonText(QString("Stop"));
                connect(progressDialog,SIGNAL(canceled()), segmenter, SLOT(cancel3D()));

                segmenter->doSegmentation3D(pos_x, pos_y, pos_z, ui->minSegIntensitySlider->value(), ui->maxSegIntensitySlider->value(), ui->minZSliceSpinBox->value(), ui->maxZSliceSpinBox->value());
                hideWaitDialog();

		//reenable segmentation widgets
		enableSegmentationWidgets();

                //remove cancel button and connection for other dialogs
                progressDialog->setCancelButtonText(QString());
                disconnect(progressDialog,SIGNAL(canceled()), segmenter, SLOT(cancel3D()));

                qDebug() << "3D segmentation complete.";
        }
}

void MainWindow::on_actionClear_Drawing_triggered()
{
    if(imagePairManager!=0 && viewManager!=0)
    {
        imagePairManager->setAll(viewManager->getCurrentSlice(), ImagePairManager::BLOCKING, ImagePairManager::BACKGROUND);
        viewManager->update();
    }
}

void MainWindow::on_actionClear_Segmentation_triggered()
{
    if(imagePairManager!=0 && viewManager!=0)
    {
        imagePairManager->setAll(viewManager->getCurrentSlice(), ImagePairManager::SEGMENTATION, ImagePairManager::BACKGROUND);
	volumeRenderManager->render3D();
        viewManager->update();
    }
}

void MainWindow::on_actionLoad_Segmentation_triggered()
{

    if(imagePairManager==0)
        return;

    //Check if we need to save our current segblock first
    if(!okToContinue())
	    return;

    //show load dialog
    QString loadFile = QFileDialog::getOpenFileName(this,
				    tr("Load Segmentation/block image"),
				    workPath.absolutePath(),
				    tr("VTK Structured points (*.vtk)"));


    imagePairManager->loadSegblock(loadFile);
    viewManager->update(); //update
}

bool MainWindow::on_actionSave_Segmentation_triggered()
{

    if(imagePairManager!=0)
    {
	//Show save dialog
	QString saveFile = QFileDialog::getSaveFileName(this,
						tr("Save Segmentation/block image"),
						workPath.absolutePath(),
						tr("VTK Structured points (*.vtk)")
						);


	if(saveFile.isEmpty())
		return false; //the user cancelled on us!

	return imagePairManager->saveSegblock(saveFile);
    }

    return false;
}

void MainWindow::on_actionInterpolate_Image_toggled(bool enable)
{
    if(imagePairManager!=0 && viewManager!=0)
    {
        viewManager->enableInterpolation(enable);
    }
}

void MainWindow::on_actionXY_View_toggled(bool enable)
{
    if(enable && viewManager!=0)
    {
        //Set orientation
        qDebug() << "Set XY";
        viewManager->setOrientation(vtkImageViewer2::SLICE_ORIENTATION_XY);
        viewManager->flipView(ui->actionRotate_view_by_180->isChecked()); //make sure the checkbox behaviour remains consistent when switching orientation
        sliceControlSetup(); //Adjust Widgets
    }
}

void MainWindow::on_actionXZ_View_toggled(bool enable)
{
    if(enable && viewManager!=0)
    {
        //set orientation
        qDebug() << "Set XZ";
        viewManager->setOrientation(vtkImageViewer2::SLICE_ORIENTATION_XZ);
        viewManager->flipView(ui->actionRotate_view_by_180->isChecked()); //make sure the checkbox behaviour remains consistent when switching orientation
        sliceControlSetup(); //Adjust Widgets
    }
}

void MainWindow::on_actionYZ_View_toggled(bool enable)
{
    if(enable && viewManager!=0)
    {
        //set orientation
        qDebug() << "Set YZ";
        viewManager->setOrientation(vtkImageViewer2::SLICE_ORIENTATION_YZ);
        viewManager->flipView(ui->actionRotate_view_by_180->isChecked()); //make sure the checkbox behaviour remains consistent when switching orientation
        sliceControlSetup(); //Adjust Widgets
    }
}

void MainWindow::tryEnableSegmentationWidgets()
{
    int dummy;

    if(seedPointManager!=0 && viewManager!=0 && seedPointManager->getSeedPoint(viewManager->getCurrentSlice(),dummy,dummy))
    {
        qDebug() << "Enable segmentation widgets!";
	enableSegmentationWidgets();
    }
    else
    {
        qDebug() << "Cannot enable segmentation widgets!";
	disableSegmentationWidgets();
    }
}


void MainWindow::closeEvent(QCloseEvent *close)
{
	if(okToContinue())
		close->accept();
	else
		close->ignore();
}

bool MainWindow::okToContinue()
{
        if(imagePairManager==NULL)
		return true; //segblock was never loaded so it's fine

	if(!(imagePairManager->segblockModified()))
		return true; //okay to continue as segblock hasn't been modified

	//segblock must of been modified. Ask user if they'd like to save
	int result = QMessageBox::warning(this,this->windowTitle(), "Segmentation/blocking data has been changed.\nDo you want to save?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

	switch(result)
	{
		case QMessageBox::Yes :
			//show save dialog
			return on_actionSave_Segmentation_triggered();

		break;

		case QMessageBox::No :
			return true; //user doesn't want to save so okay to proceed
		break;

		case QMessageBox::Cancel :
			return false;
		default:
			qWarning() << "MainWindow::okToContinue() : The QMessageBox gave us an unexpected answer!";
			return false;
	}

}




void MainWindow::on_do3Drender_clicked()
{
	qDebug() << "3D draw button is clicked" ;
	//setup volumeRenderManager
	if(imagePairManager != NULL)
		volumeRenderManager->render3D();
}

void MainWindow::on_actionClear_Segmentation_on_All_Slices_triggered()
{
	qDebug() << "Clearing all simblock voxels!";

    if(imagePairManager!=0 && viewManager!=0 && volumeRenderManager!=0)
    {
        showWaitDialog();
        imagePairManager->setAllSimBlockVoxels(ImagePairManager::SEGMENTATION, ImagePairManager::BACKGROUND);
        viewManager->update();
        volumeRenderManager->render3D();
        hideWaitDialog();
	}

}

void MainWindow::on_actionClear_Blocking_on_All_Slices_triggered()
{
	qDebug() << "Clearing all blocking voxels!";

	showWaitDialog();

    if(imagePairManager!=0 && viewManager!=0 && volumeRenderManager!=0)
    {
        imagePairManager->setAllSimBlockVoxels(ImagePairManager::BLOCKING, ImagePairManager::BACKGROUND);
        viewManager->update();
        volumeRenderManager->render3D();
        hideWaitDialog();
	}



}

void MainWindow::on_doDilate2D_clicked()
{
        if(segmenter!=0)
        {
                int pos_z = viewManager->getCurrentSlice();
                segmenter->doDilate(pos_z);
        }
}

void MainWindow::on_doErode2D_clicked()
{
        if(segmenter!=0)
        {
                int pos_z = viewManager->getCurrentSlice();
                segmenter->doErode(pos_z);
        }
}

void MainWindow::on_doClose2D_clicked()
{
        if(segmenter!=0)
        {
                int pos_z = viewManager->getCurrentSlice();
                segmenter->doMorphClose(pos_z);
        }
}

void MainWindow::on_doOpen2D_clicked()
{
        if(segmenter!=0)
        {
                int pos_z = viewManager->getCurrentSlice();
                segmenter->doMorphOpen(pos_z);
        }
}

void MainWindow::showWaitDialog()
{
	if(progressDialog==NULL)
	{
		//Setup
		progressDialog = new QProgressDialog(this);
		progressDialog->setLabelText("Please wait...");

		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setRange(0,0); // always show the dialog

		progressDialog->setMinimumDuration(0);// do no assumed time calculation

		progressDialog->setCancelButton(0); // disable the cancel button

		progressDialog->setValue(1);

		progressDialog->show();

	}
	else
		progressDialog->show();
}

void MainWindow::hideWaitDialog()
{
    progressDialog->hide();
}

void MainWindow::viewOrientationSetup()
{
    //Enable disabled menu options
    ui->actionXY_View->setEnabled(true);
    ui->actionXZ_View->setEnabled(true);
    ui->actionYZ_View->setEnabled(true);

    QActionGroup* orientationActions = new QActionGroup(this);

    orientationActions->addAction(ui->actionXY_View);
    orientationActions->addAction(ui->actionXZ_View);
    orientationActions->addAction(ui->actionYZ_View);

    //Set the default
    ui->actionXY_View->setChecked(true);

}

void MainWindow::loadOriginalImage(QString file)
{
    imageInfo.setFile(file);

    //Setup the ImagePairManager
    if(imagePairManager!=0)
        delete imagePairManager;

    imagePairManager = new ImagePairManager();

    if(!(imagePairManager->loadImage(imageInfo)))
    {
        qDebug() << "Failed to open" << imageInfo.absoluteFilePath();
        return;
    }

    //setup SeedPointManager
    if(seedPointManager!=0)
        delete seedPointManager;
    seedPointManager = new SeedPointManager(imagePairManager->getZDim());

    //setup LayoutManager
    if(viewManager!=0)
        delete viewManager;
    viewManager = new ViewManager(imagePairManager, seedPointManager, ui->qvtkWidget,ui->blockingAlphadoubleSpinBox, ui->segmentationAlphadoubleSpinBox, ui->crosshairAlphadoubleSpinBox);

    //setup drawManager
    if(drawManager!=0)
        delete drawManager;
	drawManager = new DrawManager(imagePairManager,ui->drawSizeSpinBox,ui->drawOnComboBox, ui->minZSliceSpinBox, ui->maxZSliceSpinBox, ui->segReadOnly);

    //setup segmenter
    if(segmenter!=0)
        delete segmenter;
    segmenter = new Segmenter(seedPointManager,imagePairManager,ui->kernelComboBox);

    //setup volumeRenderManager
    if(volumeRenderManager!=0)
        delete volumeRenderManager;
    volumeRenderManager = new VolumeRenderManager(imagePairManager,ui->qvtk3Ddisplayer);

    sliceControlSetup();
    contrastControlSetup();
    toolbarSetup();
    segmentationControlSetup();
    viewOrientationSetup();

    //Set the default checked state for interpolation of original image
    ui->actionInterpolate_Image->setChecked(true);

    //setup statusbar update from viewmanager
    connect(viewManager,SIGNAL(mouseHasMoved()), this, SLOT(updateStatusBar()));
    connect(viewManager,SIGNAL(mouseEntersWidget()), this, SLOT(updateStatusBar()));
    connect(viewManager,SIGNAL(mouseLeavesWidget()), this, SLOT(updateStatusBar()));

    //setup seedPointLine view being told about the seed point being change
    connect(seedPointManager,SIGNAL(seedPointChanged(int,int,int)), this, SLOT(seedPointChanged()));
    connect(viewManager,SIGNAL(sliceChanged(int)), this,SLOT(seedPointChanged()));

    //setup zoom control
    connect(ui->actionZoom_in,SIGNAL(triggered()), viewManager,SLOT(zoomIn()));
    connect(ui->actionZoom_out,SIGNAL(triggered()), viewManager,SLOT(zoomOut()));

    //setup so on segmentation completion we redraw
    connect(segmenter,SIGNAL(segmentationDone(int)), viewManager, SLOT(update()));

    //setup on drawing complete we redraw
    connect(drawManager,SIGNAL(drawingDone()),viewManager,SLOT(update()));

    //Update the work path to the location of the new image
    workPath.setPath(imageInfo.absolutePath());

    //Setup when using scroll wheel to change slice, inform widgets
    connect(viewManager,SIGNAL(sliceChanged(int)),ui->sliceSlider,SLOT(setValue(int)));


    //allow debug information to be shown from menu
    connect(ui->actionDump_debug,SIGNAL(triggered()),viewManager,SLOT(debugDump()));

    //allow load/save segblock menus now
    ui->actionLoad_Segmentation->setEnabled(true);
    ui->actionSave_Segmentation->setEnabled(true);

    //At start up no seed point will be set so disable segmentation widgets
    disableSegmentationWidgets();


    //When the user changes slice we may need to disable/enabled the segmentation widgets
    connect(viewManager,SIGNAL(sliceChanged(int)), this, SLOT(tryEnableSegmentationWidgets()));

    /*When segmentation is done force redraw for volumeRenderManager
            * Note if we do 3D segmentation it seems to update itself... not sure why
            */
    connect(segmenter,SIGNAL(segmentationDone(int)),volumeRenderManager,SLOT(render3D()));


    //set window title
    QString newWindowTitle(PROGRAM_NAME);
    newWindowTitle.append(" - ").append(imageInfo.fileName());
    setWindowTitle(newWindowTitle);
}

void MainWindow::on_actionRotate_view_by_180_toggled(bool flip)
{
	if(viewManager!=0 && volumeRenderManager!=0)
	{
		viewManager->flipView(flip);
		volumeRenderManager->flipView(flip);
	}
}

void MainWindow::on_actionResetView_triggered()
{
	if(viewManager!=0)
	{
		viewManager->resetPan();
		viewManager->resetZoom();
	}
}


void MainWindow::enableSegmentationWidgets()
{
	ui->doSegmentation2D->setEnabled(true);
	ui->doSegmentation3D->setEnabled(true);
}

void MainWindow::disableSegmentationWidgets()
{
	ui->doSegmentation2D->setEnabled(false);
	ui->doSegmentation3D->setEnabled(false);
}
