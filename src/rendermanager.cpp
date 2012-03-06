#include "rendermanager.h"
#include <QVTKWidget.h>
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

RenderManager::RenderManager(ImagePairManager* imagePairManager, QVTKWidget* qvtk3Ddisplayer)
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

	surface->SetInputConnection(mask->GetOutputPort());
	surface->ComputeNormalsOn();
	surface->SetValue(0, 0.1);
	renderer->SetBackground(0.0,0.0,0.0);
	renderWindow->AddRenderer(renderer);
	qvtk3Ddisplayer->SetRenderWindow(renderWindow);
	mapper->SetInputConnection(surface->GetOutputPort());
	actor->SetMapper(mapper);
	renderer->AddActor(actor);
}

RenderManager::~RenderManager()
{

}

void RenderManager::render3D()
{
	qDebug() << "This is renderManager function" ;

	renderWindow->Render();
	qvtk3Ddisplayer->update();

}
