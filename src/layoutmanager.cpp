#include "layoutmanager.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

LayoutManager::LayoutManager(ImagePairManager* imageManager, QVTKWidget* vtkWidget)
{
	//setup original image
	imageViewer = vtkImageViewer2::New();
	imageViewer->SetInput(imageManager->original);
	vtkWidget->SetRenderWindow(imageViewer->GetRenderWindow());
	imageViewer->SetupInteractor(vtkWidget->GetRenderWindow()->GetInteractor());

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

void LayoutManager::ChangeSlice(int slice)
{
	imageViewer->SetSlice(slice);
	emit sliceChanged(slice);
}
