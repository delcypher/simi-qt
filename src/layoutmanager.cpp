#include "layoutmanager.h"
#include <vtkRenderWindow.h>

LayoutManager::LayoutManager(ImagePairManager* imageManager, QVTKWidget* vtkWidget)
{
	imageViewer = vtkImageViewer2::New();
	imageViewer->SetInput(imageManager->original);
	vtkWidget->SetRenderWindow(imageViewer->GetRenderWindow());
	imageViewer->SetupInteractor(vtkWidget->GetRenderWindow()->GetInteractor());
}

LayoutManager::~LayoutManager()
{

}

void LayoutManager::ChangeSlice(int slice)
{
	imageViewer->SetSlice(slice);
	emit sliceChanged(slice);
}
