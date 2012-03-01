#include <QObject>
#include <vtkStructuredPoints.h>
#include <vtkImageData.h>
#include <QVTKWidget.h>
#include "imagepairmanager.h"
#include "vtkImageViewer2.h"
#include <vtkEventQtSlotConnect.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>

class ViewManager : public QObject
{
	Q_OBJECT
	public:
		ViewManager(ImagePairManager* imageManager, QVTKWidget* vtkWidget);
		~ViewManager();
		void setConstrast(double minIntensity, double maxIntensity);
		int getCurrentSlice();
        vtkRenderer* getRenderer();

	
	public slots:
		void ChangeSlice(int slice);
		void zoomIn();
		void zoomOut();
		void update();

	private slots:
		void mouseLeftClick(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
		void mouseWheelForward(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
		void mouseWheelBackward(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
		void dragHandler(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
		void mouseMove(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);


	signals:
		void sliceChanged(int slice);
		void viewLeftClicked(int xVoxel, int yVoxel, int zVoxel);
		void dragEvent(int xVoxel, int yVoxel, int zVoxel);
		void mouseIsAt(int xVoxel, int yVoxel, int zVoxel, short intensity);

	private:
        void forceZoom();

		vtkSmartPointer<vtkImageViewer2> imageViewer;
		vtkSmartPointer<vtkEventQtSlotConnect> connections;
		ImagePairManager* imageManager;
		QVTKWidget* qvtkWidget;

		double minScale; //for zoom
		double maxScale; //for zoom
		double currentScale;
		double scaleStep;

		//for dragging
		bool dragOn;

};
