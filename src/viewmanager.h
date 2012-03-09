#include <QObject>
#include <vtkStructuredPoints.h>
#include <vtkImageData.h>
#include <QVTKWidget.h>
#include "imagepairmanager.h"
#include "vtkImageViewer2.h"
#include <vtkEventQtSlotConnect.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkLookupTable.h>
#include <QDoubleSpinBox>
#include "seedpointmanager.h"
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkLineSource.h>

class ViewManager : public QObject
{
	Q_OBJECT
	public:
	  ViewManager(ImagePairManager* imagePairManager, SeedPointManager* seedPointManager, QVTKWidget* vtkWidget, QDoubleSpinBox* blockingAlphaSpinBox, QDoubleSpinBox* segmentationAlphaSpinBox);
		~ViewManager();
		void setConstrast(double minIntensity, double maxIntensity);
		int getCurrentSlice();
        vtkRenderer* getRenderer();

		int getLastMousePosX() { return mouseX;}
		int getLastMousePosY() { return mouseY;}
		int getLastMousePosZ() { return mouseZ;}
		int getLastMouseIntensity() { return mouseIntensity;}
		bool mouseIsOverWidget() { return mouseOverWidget;}

		void reBuildPipeline();

	
	public slots:
		void ChangeSlice(int slice);
		void update();
		void debugDump();
		void buildLookUpTable();
		bool setBlockingAlpha(double alpha);
		bool setSegmentationAlpha(double alpha);
		void flipView(bool flip);
		void resetZoom();
		void zoomIn();
		void zoomOut();
		void resetPan();

	private slots:
		void mouseLeftClick(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
		void mouseWheelForward(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
		void mouseWheelBackward(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
		void dragHandler(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);
		void enterLeaveHandler(vtkObject* caller, unsigned long vtkEvent);

        void redrawCrossHair();


	signals:
		void sliceChanged(int slice);
		void viewLeftClicked(int xVoxel, int yVoxel, int zVoxel);
		void dragEvent(int xVoxel, int yVoxel, int zVoxel);
		void mouseHasMoved();
		void mouseEntersWidget();
		void mouseLeavesWidget();

	private:
		void addSegblock();


		vtkSmartPointer<vtkImageViewer2> imageViewer;
		vtkSmartPointer<vtkEventQtSlotConnect> connections;
		ImagePairManager* imagePairManager;
		SeedPointManager* seedPointManager;
		QVTKWidget* qvtkWidget;
		vtkSmartPointer<vtkImageActor> segblockActor;
		vtkSmartPointer<vtkLookupTable> lut;

        vtkSmartPointer<vtkLineSource> hcrosshairSource;
        vtkSmartPointer<vtkPolyDataMapper> hcrosshairMapper;
        vtkSmartPointer<vtkActor> hcrosshairActor;

        vtkSmartPointer<vtkLineSource> vcrosshairSource;
        vtkSmartPointer<vtkPolyDataMapper> vcrosshairMapper;
        vtkSmartPointer<vtkActor> vcrosshairActor;

		double minScale; //for Zoom
		double maxScale; //for zoom
		int currentStep;//for zoom
		int zoomSteps;

		//for dragging
		bool dragOn;

		//for reporting mouse position and intensity (in terms of voxels in original image)
		int mouseX;
		int mouseY;
		int mouseZ;
		int mouseIntensity;
		bool mouseOverWidget;

		//for pan
		double defaultViewUp[3];
		double defaultFocalPoint[3];
		double defaultPosition[3];

		//for look up table
		double blockingAlpha;
		double segmentationAlpha;
		QDoubleSpinBox* blockingAlphaSpinBox;
		QDoubleSpinBox* segmentationAlphaSpinBox;

		double crossHairXlength;
		double crossHairYlength;



};
