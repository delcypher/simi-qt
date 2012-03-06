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
//#include "vtkVoxelModeller.h"

RenderManager::RenderManager(ImagePairManager* imagePairManager, QVTKWidget* qvtk3Ddisplayer)
{
	this->imagePairManager = imagePairManager;
	this->qvtk3Ddisplayer = qvtk3Ddisplayer;

}

RenderManager::~RenderManager()
{

}

void RenderManager::render3D()
{
	qDebug() << "This is renderManager function" ;

	//segblock is a vtkStructuredPoints
	//vtkMarchingCubes is a filter taking a volume as input (e.g. 3D structured point set)

	/*
	double bounds[6];
	imagePairManager->segblock->GetBounds(bounds);
	for(int i = 0 ; i < 6 ; i++)
	{
		double range = bounds[i+1]-bounds[i];
		bounds[i] = bounds[i] - 0.1*range ;
		bounds[i+1] = bounds[i+1] + 0.1*range ;
	}
	vtkSmartPointer<vtkVoxelModeller> voxelModeller = vtkSmartPointer<vtkVoxelModeller>::New();
	voxelModeller->SetSampleDimensions(50,50,50);
	voxelModeller->SetModelBounds(bounds);
	voxelModeller->SetScalarTypeToFloat();
	voxelModeller->SetMaximumDistance(0.1);
	voxelModeller->SetInputConnection(imagePairManager->segblock->GetProducerPort());
	*/
	vtkSmartPointer<vtkMarchingCubes> surface = vtkSmartPointer<vtkMarchingCubes>::New();

	surface->SetInputConnection(imagePairManager->segblock->GetProducerPort());
	surface->ComputeNormalsOn();
	surface->SetValue(0, 0.5);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(.1, .2, .3);

	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	qvtk3Ddisplayer->SetRenderWindow(renderWindow);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(surface->GetOutputPort());

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	renderer->AddActor(actor);

	renderWindow->Render();
	qvtk3Ddisplayer->GetInteractor()->Start();
	qvtk3Ddisplayer->update();

}
