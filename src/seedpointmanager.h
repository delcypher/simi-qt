#ifndef SEEDPOINTMANAGER_H
#define SEEDPOINTMANAGER_H

#include <QObject>
#include <vtkEvent.h>
#include <vtkObject.h>

class SeedPointManager : public QObject
{
	Q_OBJECT
	public:
		SeedPointManager(int numberOfSlices);
		~SeedPointManager();
	
		//Gets the seed point for the current slice
		bool getSeedPoint(int& x, int& y);


	public slots:
		//allows the seed point to manually set
		bool setSeedPoint(int x, int y, int slice);

		void setCurrentSlice(int slice);


	private:
		int currentSlice;
		int numberOfSlices;
};

#endif
