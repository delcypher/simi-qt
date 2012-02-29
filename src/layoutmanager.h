#include <QObject>
#include <vtkStructuredPoints.h>
#include <vtkImageData.h>
#include <QVTKWidget.h>
#include "imagepairmanager.h"
#include "vtkImageViewer2.h"

class LayoutManager : public QObject
{
	Q_OBJECT
	public:
		LayoutManager(ImagePairManager* imageManager, QVTKWidget* vtkWidget);
		~LayoutManager();
		void setConstrast(double minIntensity, double maxIntensity);
	
	public slots:
		void ChangeSlice(int slice);

	signals:
		void sliceChanged(int slice);

	private:
		vtkSmartPointer<vtkImageViewer2> imageViewer;

};
