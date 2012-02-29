#include "layoutmanager.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

LayoutManager::LayoutManager(ImagePairManager* imageManager, QVTKWidget* vtkWidget) : scaleStep(10)
{
	//setup original image
	imageViewer = vtkImageViewer2::New();
	imageViewer->SetInput(imageManager->original);
	vtkWidget->SetRenderWindow(imageViewer->GetRenderWindow());
	imageViewer->SetupInteractor(vtkWidget->GetRenderWindow()->GetInteractor());

    //setup zoom control
    maxScale= ( imageManager->getYDim() )*( imageManager->getYSpacing() )/2.0;
    minScale=maxScale/500;
    currentScale=maxScale;

    imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale(currentScale);


	//setup segblock image

	//setup seedpoint drawing?

}

LayoutManager::~LayoutManager()
{

}

void LayoutManager::setConstrast(double minIntensity, double maxIntensity)
{
    if(maxIntensity >= minIntensity)
    {
        imageViewer->SetColorLevel( (minIntensity + maxIntensity)/2.0 );
        imageViewer->SetColorWindow(maxIntensity - minIntensity);

        //update
        imageViewer->GetRenderWindow()->Render();
    }
}

void LayoutManager::zoomIn()
{
    double tempScale = currentScale -scaleStep;

    if(tempScale >= minScale)
        currentScale=tempScale;

    forceZoom();

}

void LayoutManager::zoomOut()
{
    double tempScale = currentScale +scaleStep;

    if(tempScale <= maxScale)
        currentScale=tempScale;

    forceZoom();
}

void LayoutManager::ChangeSlice(int slice)
{
	imageViewer->SetSlice(slice);
    emit sliceChanged(slice);

    //workaround bug where changing slice causes zoom to reset to something unexpected
    forceZoom();
}

void LayoutManager::forceZoom()
{
    imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale(currentScale);
    imageViewer->GetRenderWindow()->Render();
}
