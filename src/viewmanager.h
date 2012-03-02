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
        ViewManager(ImagePairManager* imagePairManager, QVTKWidget* vtkWidget);
		~ViewManager();
		void setConstrast(double minIntensity, double maxIntensity);
		int getCurrentSlice();
        vtkRenderer* getRenderer();

		int getLastMousePosX() { return mouseX;}
		int getLastMousePosY() { return mouseY;}
		int getLastMousePosZ() { return mouseZ;}
		int getLastMouseIntensity() { return mouseIntensity;}
		bool mouseIsOverWidget() { return mouseOverWidget;}


	
	public slots:
		void ChangeSlice(int slice);
		void zoomIn();
		void zoomOut();
		void update();
		void debugDump();

	private slots:
		void mouseLeftClick(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
		void mouseWheelForward(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
		void mouseWheelBackward(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
		void dragHandler(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
		void enterLeaveHandler(vtkObject* caller, unsigned long vtkEvent);


	signals:
		void sliceChanged(int slice);
		void viewLeftClicked(int xVoxel, int yVoxel, int zVoxel);
		void dragEvent(int xVoxel, int yVoxel, int zVoxel);
		void mouseHasMoved();
		void mouseEntersWidget();
		void mouseLeavesWidget();

	private:
        void forceZoom();


		vtkSmartPointer<vtkImageViewer2> imageViewer;
		vtkSmartPointer<vtkEventQtSlotConnect> connections;
        ImagePairManager* imagePairManager;
		QVTKWidget* qvtkWidget;

		double minScale; //for zoom
		double maxScale; //for zoom
		double currentScale;
		double scaleStep;

		//for dragging
		bool dragOn;

		//for reporting mouse position and intensity (in terms of voxels in original image)
		int mouseX;
		int mouseY;
		int mouseZ;
		int mouseIntensity;
		bool mouseOverWidget;

};
