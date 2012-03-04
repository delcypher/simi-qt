#ifndef SEEDPOINTMANAGER_H
#define SEEDPOINTMANAGER_H

#include <QObject>
#include <vtkEvent.h>
#include <vtkObject.h>
#include <vector>

using std::vector;

class SeedPointManager : public QObject
{
	Q_OBJECT
	public:
		SeedPointManager(int numberOfSlices);
		~SeedPointManager();
	
		//Gets the seed point for slice "sliceNumber". Reports (-1,-1) if no seed Point is currently set
		bool getSeedPoint(int sliceNumber, int& x, int& y);

	signals:
		void seedPointChanged(int sliceNumber, int x, int y);


	public slots:
		//allows the seed point to manually set
		bool setSeedPoint(int x, int y, int sliceNumber);

		void setCurrentSlice(int sliceNumber);


	private:
		int numberOfSlices;
		vector< vector<int> > seedPerSlice;
};

#endif
