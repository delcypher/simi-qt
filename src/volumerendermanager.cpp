#include "volumerendermanager.h"
#include <QVTKWidget.h>
#include <vtkCamera.h>
#include "imagepairmanager.h"
#include "vtkMarchingCubes.h"
#include "vtkSmartPointer.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include <QDebug>
#include "vtkImageMaskBits.h"
#include "vtkViewport.h"

VolumeRenderManager::VolumeRenderManager(ImagePairManager* imagePairManager, QVTKWidget* qvtk3Ddisplayer)
{
	this->imagePairManager = imagePairManager;
	this->qvtk3Ddisplayer = qvtk3Ddisplayer;

	surface = vtkMarchingCubes::New();
	renderer = vtkRenderer::New();
	renderWindow = vtkRenderWindow::New();
	mapper = vtkPolyDataMapper::New();
	actor = vtkActor::New();
	mask = vtkImageMaskBits::New();

	//filter segblock, only SEGMENTATION will be presented
	mask->AddInput(imagePairManager->segblock);
	mask->SetMask( static_cast<unsigned int>(ImagePairManager::SEGMENTATION));
	mask->SetOperationToAnd();

	double bounds[6];
	imagePairManager->segblock->GetBounds(bounds);
	for(int i = 0 ; i < 6 ; i++)
	{
		double range = bounds[i+1]-bounds[i];
		bounds[i] = bounds[i] - 0.1*range ;
		bounds[i+1] = bounds[i+1] + 0.1*range ;
	}


	surface->SetInputConnection(mask->GetOutputPort());
	surface->ComputeNormalsOn();
	surface->SetValue(0, 0.1);
	renderer->SetBackground(0.0,0.0,0.0);
	renderWindow->AddRenderer(renderer);

	renderer->ResetCamera(bounds);

	qvtk3Ddisplayer->SetRenderWindow(renderWindow);
	mapper->SetInputConnection(surface->GetOutputPort());
	actor->SetMapper(mapper);
	renderer->AddActor(actor);
}

VolumeRenderManager::~VolumeRenderManager()
{

}

void VolumeRenderManager::render3D()
{
	qDebug() << "This is renderManager function" ;
	//qDebug() << "Currently, the viewport is " << renderer->GetCenter()[0] << " " << renderer->GetCenter()[1] << " " << renderer->GetCenter()[2];

	renderWindow->Render();
	qvtk3Ddisplayer->update();

}

void VolumeRenderManager::flipView(bool flip)
{
	if(flip)
		renderer->GetActiveCamera()->SetRoll(180.0);
	else
		renderer->GetActiveCamera()->SetRoll(0.0);

	renderWindow->Render();
}

void VolumeRenderManager::rebuildPipeline()
{
	/* It is assumed that segblock has been changed so we disconnect all inputs from the mask and
	*  then reconnect
	*/
	mask->RemoveAllInputs();
	mask->AddInput(imagePairManager->segblock);
	mask->SetMask( static_cast<unsigned int>(ImagePairManager::SEGMENTATION));
	mask->SetOperationToAnd();
	qDebug() << "VolumeRenderManager::rebuildPipeline()";
}
