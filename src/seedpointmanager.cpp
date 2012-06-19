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

	// initializes the array seedPerSlice to have the correct number of rows
	seedPerSlice.resize(numberOfSlices);

	// sets all values of seedPerSlice (x and y coordinated per slice) to -1
	for (int i = 0; i < numberOfSlices; ++i)
	{
		seedPerSlice[i].assign(2,-1);
	}
}

SeedPointManager::~SeedPointManager()
{

}


//Gets the seed point for slice "sliceNumber". Reports (-1,-1) if no seed Point is currently set
bool SeedPointManager::getSeedPoint(int sliceNumber, int& x, int& y)
{
	if (sliceNumber >= numberOfSlices || sliceNumber < 0)
	{
		qWarning() << "SeedPointManager::getSeedPoint() : Invalid slice number:" << sliceNumber << " requested!";
		return false;
	}

	x = seedPerSlice[sliceNumber][0];
	y = seedPerSlice[sliceNumber][1];

	// checks if the seed point for the given slice has been set
	if (x != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//allows the seed point to manually set
bool SeedPointManager::setSeedPoint(int x, int y, int sliceNumber)
{
	if (sliceNumber > numberOfSlices || sliceNumber < 0)
	{
		return false;
	}

	seedPerSlice[sliceNumber][0] = x;
	seedPerSlice[sliceNumber][1] = y;

	qDebug() << "Set seed point for slice " << sliceNumber << " to (" << x << "," << y << ")";

	//inform other classes that something has changed
	emit seedPointChanged(sliceNumber, x, y);

	return true;
}

