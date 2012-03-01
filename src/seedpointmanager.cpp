#include "seedpointmanager.h"
#include <QDebug>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkCellPicker.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>


SeedPointManager::SeedPointManager(int numberOfSlices)
{
	this->numberOfSlices = numberOfSlices;
	qDebug() << "SeedPointManager setup with " << numberOfSlices << " slices to handle.";
}

SeedPointManager::~SeedPointManager()
{

}


//Gets the seed point for the current slice
bool SeedPointManager::getSeedPoint(int& x, int& y)
{
	//TODO
	x=0;
	y=0;
	return true;
}

//allows the seed point to manually set
bool SeedPointManager::setSeedPoint(int slice, int x, int y)
{
	//TODO
    qDebug() << "Set seed point for slice " << slice << " to (" << x << "," << y << ")";
	return false;
}


void SeedPointManager::pointPicked(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command)
{
	//TODO
    qDebug() << "SeedManager::pointPicked()";

    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::SafeDownCast(caller);

    // Get the location of the click (in window coordinates)
    int* pos = iren->GetEventPosition();

    qDebug() << "Window position is:" << pos[0] << pos[1] << endl;


    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();

    vtkRenderer* renderer = static_cast<vtkRenderer*>(clientData);

    // Pick from this location.
    picker->Pick(pos[0], pos[1], 0, renderer);

    qDebug() << "IJK:" << picker->GetCellIJK()[0] << "," << picker->GetCellIJK()[1] << "," << picker->GetCellIJK()[2];


}

void SeedPointManager::setCurrentSlice(int slice)
{
	//TODO
}

