#include "seedpointmanager.h"
#include <QDebug>


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
	return false;
}


void SeedPointManager::pointPicked(vtkObject* caller, unsigned long vtkEvent, void* clientData)
{
	//TODO
}

void SeedPointManager::setCurrentSlice(int slice)
{
	//TODO
}

