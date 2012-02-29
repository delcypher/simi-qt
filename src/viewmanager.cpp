#include "viewmanager.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <QDebug>


ViewManager::ViewManager(ImagePairManager* imageManager, QVTKWidget* vtkWidget) : scaleStep(10)
{
	//setup original image
	imageViewer = vtkImageViewer2::New();
	imageViewer->SetInput(imageManager->original);
	vtkWidget->SetRenderWindow(imageViewer->GetRenderWindow());
	imageViewer->SetupInteractor(vtkWidget->GetRenderWindow()->GetInteractor());

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
    connections->Connect(vtkWidget->GetInteractor(),
                vtkCommand::MouseWheelForwardEvent,
                this,
                SLOT(mouseWheelForward(vtkObject*,ulong,void*,void*,vtkCommand*)),
                NULL,
                1.0);


    connections->Connect(vtkWidget->GetInteractor(),
                vtkCommand::MouseWheelBackwardEvent,
                this,
                SLOT(mouseWheelBackward(vtkObject*,ulong,void*,void*,vtkCommand*)),
                NULL,
                1.0
                );

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
	imageViewer->SetSlice(slice);
    emit sliceChanged(slice);

    //workaround bug where changing slice causes zoom to reset to something unexpected
    forceZoom();
}

void ViewManager::forceZoom()
{
    imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale(currentScale);
    imageViewer->GetRenderWindow()->Render();
}
