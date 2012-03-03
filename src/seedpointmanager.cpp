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


//Gets the seed point for slice "sliceNumber". Reports (-1,-1) if no seed Point is currently set
bool SeedPointManager::getSeedPoint(int sliceNumber, int& x, int& y)
{
	//TODO
	x=0;
	y=0;
	return true;
}

//allows the seed point to manually set
bool SeedPointManager::setSeedPoint(int x, int y, int slice)
{
	//TODO
	qDebug() << "Set seed point for slice " << slice << " to (" << x << "," << y << ")";

	//inform other classes that something has changed
	emit seedPointChanged(slice, x, y);

	return false;
}


void SeedPointManager::setCurrentSlice(int slice)
{
	//TODO
}

