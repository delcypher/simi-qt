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
	
		//Gets the seed point for slice z
		bool getSeedPoint(int sliceNumber, int& x, int& y);


	public slots:
		//allows the seed point to manually set
		bool setSeedPoint(int x, int y, int slice);

		void setCurrentSlice(int slice);


	private:
		int numberOfSlices;
};

#endif
