#include "viewmanager.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <QDebug>
#include <vtkCellPicker.h>
#include <QVTKInteractor.h>


ViewManager::ViewManager(ImagePairManager* imageManager, QVTKWidget* qvtkWidget) : scaleStep(10), dragOn(false)
{
    this->imageManager = imageManager;
    this->qvtkWidget = qvtkWidget;

	//setup original image
	imageViewer = vtkImageViewer2::New();
	imageViewer->SetInput(imageManager->original);
	qvtkWidget->SetRenderWindow(imageViewer->GetRenderWindow());
	imageViewer->SetupInteractor(qvtkWidget->GetRenderWindow()->GetInteractor());

    //setup zoom control
    maxScale= ( imageManager->getYDim() )*( imageManager->getYSpacing() )/2.0;
    minScale=maxScale/800;
    currentScale=maxScale;

    imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale(currentScale);


    //setup segblock image

    //setup seedpoint drawing?

    //Enable connections between VTK events and our Qt Slots
    connections = vtkEventQtSlotConnect::New();

    //Setup mousewheel connection for zoom. We use high priority so we get called before the vtkinteractorstyle
    connections->Connect(qvtkWidget->GetInteractor(),
                vtkCommand::MouseWheelForwardEvent,
                this,
                SLOT(mouseWheelForward(vtkObject*,ulong,void*,void*,vtkCommand*)),
                NULL,
                1.0);


    connections->Connect(qvtkWidget->GetInteractor(),
                vtkCommand::MouseWheelBackwardEvent,
                this,
                SLOT(mouseWheelBackward(vtkObject*,ulong,void*,void*,vtkCommand*)),
                NULL,
                1.0
                );

    //setup connection for simple left click
    connections->Connect(qvtkWidget->GetInteractor(),
			vtkCommand::LeftButtonPressEvent,
			this,
			SLOT(mouseLeftClick(vtkObject*,ulong,void*,void*,vtkCommand*)),
			NULL,
			1.0);

    //setup connections for mouse dragging
    connections->Connect(qvtkWidget->GetInteractor(),
		vtkCommand::LeftButtonPressEvent,
		this,
		SLOT(dragHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
		NULL,
		2.0);


    connections->Connect(qvtkWidget->GetInteractor(),
		vtkCommand::MouseMoveEvent,
		this,
		SLOT(dragHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
		NULL,
		2.0);

    connections->Connect(qvtkWidget->GetInteractor(),
		vtkCommand::LeftButtonReleaseEvent,
		this,
		SLOT(dragHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
		NULL,
		2.0);

}

ViewManager::~ViewManager()
{

}

void ViewManager::setConstrast(double minIntensity, double maxIntensity)
{
    if(maxIntensity >= minIntensity)
    {
        imageViewer->SetColorLevel( (minIntensity + maxIntensity)/2.0 );
        imageViewer->SetColorWindow(maxIntensity - minIntensity);

        //update
        imageViewer->GetRenderWindow()->Render();
    }
}

int ViewManager::getCurrentSlice()
{
    return imageViewer->GetSlice();
}

vtkRenderer *ViewManager::getRenderer()
{
    if(imageViewer!=0)
        return imageViewer->GetRenderer();
    else
        return NULL;

}

void ViewManager::zoomIn()
{
    double tempScale = currentScale -scaleStep;

    if(tempScale >= minScale)
    {
        currentScale=tempScale;
        qDebug() << "Zoom scale set to " << currentScale;
    }
    else
    {
        qDebug() << "Zoom In limit reached";
    }

    forceZoom();

}

void ViewManager::zoomOut()
{
    double tempScale = currentScale +scaleStep;

    if(tempScale <= maxScale)
    {
        currentScale=tempScale;
        qDebug() << "Zoom scale set to " << currentScale;
    }
    else
    {
        qDebug() << "Zoom Out limit reached.";
    }

    forceZoom();
}

void ViewManager::mouseWheelForward(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command)
{
    zoomIn();

    //Prevent vtkInteractorStyle from intercepting the event
    command->AbortFlagOn();
}

void ViewManager::mouseWheelBackward(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command)
{
    zoomOut();

    //Prevent vtkInteractorStyle from intercepting the event
    command->AbortFlagOn();
}


void ViewManager::ChangeSlice(int slice)
{
	if(slice >= imageManager->getZMin() && slice <= imageManager->getZMax())
	{
        imageViewer->SetSlice(slice);
        emit sliceChanged(slice);
	}

    //workaround bug where changing slice causes zoom to reset to something unexpected
    forceZoom();
}

void ViewManager::forceZoom()
{
    imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale(currentScale);
    imageViewer->GetRenderWindow()->Render();
}

void ViewManager::mouseLeftClick(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command)
{
	vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::SafeDownCast(caller);

	// Get the location of the click (in window coordinates)
	int* pos = iren->GetEventPosition();

	qDebug() << "ViewManager::mouseLeftClick() : Window position is:" << pos[0] << pos[1];

	vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();

	// Pick from this location.
	picker->Pick(pos[0], pos[1], 0, imageViewer->GetRenderer());

	if(picker->GetCellId() != -1)
	{
		qDebug() << "ViewManager::mouseLeftClick() : IJK:" << picker->GetCellIJK()[0] << "," << picker->GetCellIJK()[1] << "," << picker->GetCellIJK()[2];
		emit viewLeftClicked(picker->GetCellIJK()[0], picker->GetCellIJK()[1], picker->GetCellIJK()[2]);
	}
	else
		qDebug() << "ViewManager::mouseLeftClick() : Out of range!";

	//make sure vtkinteractorstyle doesn't catch event
	command->AbortFlagOn();
}

void ViewManager::dragHandler(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command)
{
	//Get event co-ordinates before processing the event type

	vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::SafeDownCast(caller);

	// Get the location of the click (in window coordinates)
	int* pos = iren->GetEventPosition();

	vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();

	// Pick from this location.
	picker->Pick(pos[0], pos[1], 0, imageViewer->GetRenderer());


	switch(vtkEvent)
	{
		case vtkCommand::LeftButtonPressEvent :
			dragOn = true;
			//qDebug() << "Drag start";
			emit dragEvent(picker->GetCellIJK()[0],picker->GetCellIJK()[1], picker->GetCellIJK()[2]);

		break;

		case vtkCommand::LeftButtonReleaseEvent :
			dragOn = false;
			//qDebug() << "Drag stop";
			emit dragEvent(picker->GetCellIJK()[0],picker->GetCellIJK()[1], picker->GetCellIJK()[2]);

		break;

		case vtkCommand::MouseMoveEvent :
			if(dragOn)
			{
				//qDebug() << "Dragging...";
				emit dragEvent(picker->GetCellIJK()[0],picker->GetCellIJK()[1], picker->GetCellIJK()[2]);
			}



		break;

		default:
			qWarning() << "ViewManager::dragHandler() unrecognised mouse event received.";

	}
}

void ViewManager::update()
{
	imageViewer->GetRenderWindow()->Render();
}
