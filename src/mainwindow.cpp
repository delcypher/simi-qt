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
	// vtkSmartPointer<vtkRenderWindowInteractor>::New();
	// image_view->SetupInteractor( iren );

	//flip view by 180 degrees so we aren't upside down
        //imageView->GetRenderer()->ResetCamera();
        //imageView->GetRenderer()->GetActiveCamera()->Roll(180);

	/* ****************************************************************************** */
	/* START ADD ACTORS EXPERIMENT */

	// Create a cube
	// vtkSmartPointer<vtkCubeSource> cubeSource = vtkSmartPointer<vtkCubeSource>::New();
	// cubeSource->SetXLength(50);
	// cubeSource->SetYLength(50);
	// cubeSource->SetZLength(60);
//	vtkSmartPointer<vtkImageData> cubeSource = vtkSmartPointer<vtkImageData>::New();
//	cubeSource->SetDimensions(50,50,60);


//	// Create a mapper and actor
//	vtkSmartPointer<vtkImageMapper> mapper = vtkSmartPointer<vtkImageMapper>::New();
//	mapper->SetInput(cubeSource);
//	vtkSmartPointer<vtkActor2D> actor = vtkSmartPointer<vtkActor2D>::New();
//	actor->SetMapper(mapper);
//	// actor->SetPosition(0.0, 0.0, 120);
//	actor->SetPosition(50, 50);

//	// Add actor to viewer
//	imageView->GetRenderer()->AddActor(actor);

	/* END EXPERIMENT */
	/* ****************************************************************************** */

	ui->qvtkWidget->SetRenderWindow(imageView->GetRenderWindow());
	imageView->SetSlice(0);

	// Setup custom interactor style
	customStyle = new CustomInteractorStyle(reader->GetOutput(), imageView->GetRenderer());

	vtkSmartPointer<vtkRenderWindowInteractor> renwin = vtkRenderWindowInteractor::New();
	//customStyle->SetDefaultRenderer(imageView->GetRenderer());

	renwin = ui->qvtkWidget->GetRenderWindow()->GetInteractor();
	imageView->SetupInteractor(renwin);

	renwin->SetInteractorStyle(customStyle);

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
	{
		imageView->SetSlice(value);

		//work around bug where zoom level changes on calling SetSlice()
		customStyle->forceZoom();
		ui->qvtkWidget->update();
	}
}

void MainWindow::sliceControlSetup()
{
	ui->sliceSlider->setRange(imageView->GetSliceMin(), imageView->GetSliceMax() );
	ui->sliceSpinBox->setRange(imageView->GetSliceMin(), imageView->GetSliceMax() );
}

void MainWindow::on_runAlgorithm_clicked()
{
        vtkSmartPointer<vtkImageData> original = this->imageView->GetInput();        
        int pos_x = this->ui->pos_x_line->text().toInt();
        int pos_y = this->ui->pos_y_line->text().toInt();
        int pos_z = imageView->GetSlice();
        int range_from = this->ui->from_line->text().toInt();
        int range_to = this->ui->to_line->text().toInt();
        int* extent = original->GetExtent();
        int* dims = original->GetDimensions();
        double* origin = original->GetOrigin();
        cout << origin[0] << " " << origin[1] << " " << origin[2] << endl;
        double* spacings = original->GetSpacing();
        //cout << dims[0] << " " << dims[1] << " " << dims[2] << " " << pos_z << endl;
        //cout << spacings[0] << " " << spacings[1] << " " << spacings[2] << endl;
        //cout << original->GetScalarTypeAsString() << endl;
        //cout << original->GetScalarTypeMax() << endl;
        //cout << original->GetScalarTypeMin() << endl;
        //cout << original->GetNumberOfScalarComponents() << endl;

        vtkSmartPointer<vtkImageData> visited = vtkSmartPointer<vtkImageData>::New();
        visited->SetDimensions(dims[0],dims[1],1);
        visited->SetSpacing(spacings);
        //visited->SetOrigin(origin[0], origin[1], origin[2] - 1);
        //origin = visited->GetOrigin();
        //cout << origin[0] << " " << origin[1] << " " << origin[2] << endl;
        visited->SetExtent(extent);
        visited->SetNumberOfScalarComponents(4);
        visited->SetScalarTypeToUnsignedChar();
        visited->AllocateScalars();
        spacings = visited->GetSpacing();
        //cout << spacings[0] << " " << spacings[1] << " " << spacings[2] << endl;

        //fill in image
        for (int y=0; y<512; y++)
        {
                for (int x=0; x<512; x++)
                {
                        unsigned char* pixel = static_cast<unsigned char*>(visited->GetScalarPointer(x,y,0));
                        pixel[0] = 0;
                        pixel[1] = 0;
                        pixel[2] = 0;
                        pixel[3] = 0;
                        //cout << y << " " << x << " " << (int)pixel[0] << " " << (int)pixel[1] << " " << (int)pixel[2] << endl;
                }
        }

        // run algorithm
        //cout << pos_x << " " << pos_y << " " << range_from << " " << range_to << endl;
        flood_fill(original, visited, pos_x, pos_y, pos_z, range_from, range_to, predicate1);

        //display in front

//        // Create a mapper and actor
//        vtkSmartPointer<vtkImageMapper> mapper = vtkSmartPointer<vtkImageMapper>::New();
//        mapper->SetInput(visited);
//        vtkSmartPointer<vtkActor2D> actor = vtkSmartPointer<vtkActor2D>::New();
//        actor->SetMapper(mapper);


//        actor->SetPosition(0, 0);

//        // Add actor to viewer
//        imageView->GetRenderer()->AddActor(actor);

        //vtkSmartPointer<vtkImageActor> actor = vtkSmartPointer<vtkImageActor>::New();
        //vtkSmartPointer<vtkMapper2D> mapper = vtkSmartPointer<vtkMapper2D>::New();
        vtkSmartPointer<vtkImageMapper> mapper = vtkSmartPointer<vtkImageMapper>::New();
        vtkSmartPointer<vtkActor2D> actor = vtkSmartPointer<vtkActor2D>::New();

        mapper->SetInput(visited);
        actor->SetMapper(mapper);
        actor->SetPosition(origin[0],origin[1]);
        actor->SetVisibility();

        imageView->GetRenderer()->AddActor(actor);



}
