#include <QObject>
#include <vtkStructuredPoints.h>
#include <vtkImageData.h>
#include <QVTKWidget.h>
#include "imagepairmanager.h"
#include "vtkImageViewer2.h"
#include <vtkEventQtSlotConnect.h>
#include <vtkCommand.h>

class LayoutManager : public QObject
{
	Q_OBJECT
	public:
		LayoutManager(ImagePairManager* imageManager, QVTKWidget* vtkWidget);
		~LayoutManager();
		void setConstrast(double minIntensity, double maxIntensity);

	
	public slots:
		void ChangeSlice(int slice);
		void zoomIn();
		void zoomOut();

	private slots:
        void mouseWheelForward(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
        void mouseWheelBackward(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);


	signals:
		void sliceChanged(int slice);

	private:
        void forceZoom();

		vtkSmartPointer<vtkImageViewer2> imageViewer;
		vtkSmartPointer<vtkEventQtSlotConnect> connections;

		double minScale; //for zoom
		double maxScale; //for zoom
		double currentScale;
		double scaleStep;

};
