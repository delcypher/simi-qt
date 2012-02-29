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

		//allows the seed point to manually set
		bool setSeedPoint(int slice, int x, int y);

	public slots:
		void pointPicked(vtkObject* caller, unsigned long vtkEvent, void* clientData);
		void setCurrentSlice(int slice);


	private:
		int currentSlice;
		int numberOfSlices;
};
