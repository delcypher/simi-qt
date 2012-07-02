/*! \file viewmanager.h  */
#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QObject>
#include <vtkStructuredPoints.h>
#include <vtkImageData.h>
#include <QVTKWidget.h>
#include "imagepairmanager.h"
#include "vtkImageViewer2.h"
#include "boundarymanager.h"
#include <vtkEventQtSlotConnect.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkLookupTable.h>
#include <QDoubleSpinBox>
#include <QSlider>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkLineSource.h>
#include <vtkCubeSource.h>

//Forward Declare MultiViewManager
class MultiViewManager;


//! Manages the display off and interaction with the images

/*! This class is responsible for the drawing of the original (MRI/CT) image, the segmentation/block image and the cross hair.
*   It also handle various mouse events on the QVTKWidget that it renders into.
*
*
*/
class ViewManager : public QObject
{
	Q_OBJECT
	public:
		/*! Constructor
		* \param imagePairManager is an imagePairManager that has already had its image loaded. This will used as the source for the original image and segmentation/blocking image.
		* \param seedPointManager is a seedPointManager. It is used draw the seed point crosshair into the QVTKWidget that is rendered into.
		* \param vtkWidget is the QVTKWidget to be used to render into
        * \param sliceSpinBox
        * \param sliceSlider
		* \param blockingAlphaSpinBox is the the Widget used to control the blocking Alpha. It is automatically configured by this constructor.
		* \param segmentationAlphaSpinBox is the the Widget used to control the segmentation Alpha. It is automatically configured by this constructor.
		* \param crosshairAlphaSpinBox is the the Widget used to control the crosshair Alpha. It is automatically configured by this constructor.
		*/
        ViewManager(ImagePairManager* imagePairManager, QVTKWidget* vtkWidget, QSpinBox* sliceSpinBox, QSlider* sliceSlider, QDoubleSpinBox* blockingAlphaSpinBox, QDoubleSpinBox* segmentationAlphaSpinBox, QDoubleSpinBox* crosshairAlphaSpinBox, QDoubleSpinBox* boundingBoxAlphaSpinBox, unsigned int defaultOrientation);

		//! Destructor
		~ViewManager();

		/*! Change the contrast of the displayed original image. The segmention/block image and crosshair are not affected.
		*  \param minIntensity is the minimum Intensity (in terms of raw values in original image) below which all intensities are shown as black in the view
		*  \param maxIntensity is the maximum Intensity (in terms of raw values in original image) above which all intensities are shown as white in the view
		*  \return if setting was successful.
		*/
		bool setConstrast(double minIntensity, double maxIntensity);

		/*! Get the current slice
		*  \return the current slice that is shown in the view.
		*/
		int getCurrentSlice();

		bool isFlipped();

		/*!
		*  \return The last x position that the mouse was at in terms of voxel coordinates in the original image.
		*/
		int getLastMousePosX() { return mouseX;}

		/*!
		*  \return The last y position that the mouse was at in terms of voxel coordinates in the original image.
		*/
		int getLastMousePosY() { return mouseY;}

		/*!
		*  \return The last z position that the mouse was at in terms of voxel coordinates in the original image.
		*/
		int getLastMousePosZ() { return mouseZ;}

		/*!
		*  \return The raw intensity of the voxel in the original image where the mouse was last located.
		*/
		int getLastMouseIntensity() { return mouseIntensity;}

		/*!
		* \return True if the mouse is currently over the QVTKWidget used by this class.
		*/
		bool mouseIsOverWidget() { return mouseOverWidget;}

		/*! Enable/disable panning via left-click and draw of the mouse
		*  \param enabled should be set true if you wish to enable panning. Otherwise set to false.
		*/
		void enablePanning(bool enabled);

        /*! Sets the scale used for panning. It is a scaling factor used for left-click pan behaviour.
        *   If set to 1.0 , for every voxel dragged over by the mouse the camera will move by 1 voxel.
        *   If this factor > 1.0 then the camera is moved by more voxels than the mouse does.
        *   If this factor < 1.0 then the camera is moved by less voxels than the mouse does.
        *   \param scale is the scaling factor
        *   \return True if in acceptable range else returns false
        */
        bool setPanScale(double scale);

        /*! Get the current pan scale.
        *
        *  \return The current pan scale.
        *  \sa setPanScale()
        */
        double getPanScale() { return panScale;}

        unsigned int getOrientation() { return orientation;}

        bool setOrientation(unsigned int ort);

        int getSliceMin();
        int getSliceMax();

        bool setCrosshairColour(double r, double g, double b);

	
	public slots:
		/*! Change the slice show by the QVTKWidget that this class renders in to.
		* \param slice is the slice to switch to.
		* \return True if slice changed successfully otherwise false
		*/
		bool setSlice(int slice);

		/*! This will causes the current view (in VTK's lazy pipeline style) presented by the QVTKWidget to be redrawn.
		*
		*/
		void update();

		/*! Dump debugging information to console standard error
		*/
		void debugDump();

		/*! Set the alpha (opacity) of blocking voxels when displayed in the QVTKWidget that this class renders in to.
		*  \param alpha is the alpha to use for blocking voxels. alpha should be in the range [0.0,1.0]
		*  \return true if successfully set alpha otherwise false
		*/
		bool setBlockingAlpha(double alpha);

		/*! Set the alpha (opacity) of segmentation voxels when displayed in the QVTKWidget that this class renders in to.
		*  \param alpha is the alpha to use for blocking voxels. alpha should be in the range [0.0,1.0]
		*  \return true if successfully set alpha otherwise false
		*/
		bool setSegmentationAlpha(double alpha);

		/*! Set the alpha (opacity) of the crosshair displayed in the QVTKWidget that this class renders in to.
		*  \param alpha is the alpha to use for blocking voxels. alpha should be in the range [0.0,1.0]
		*  \return true if successfully set alpha otherwise false
		*/
		bool setCrosshairAlpha(double alpha);

        bool setBoundingBoxAlpha(double alpha);

		/*! Rotate the view shown by the QVTKWidget (that this class renders in to) by 180 degrees. This is useful if the image is upside down!
		*
		*  \param flip is true if the view should be flipped. If not set to false.
		*/
		void flipView(bool flip);

		/*! Reset the zoom level in the view shown by the QVTKWidget (that this class renders in to) to the default
		*/
		void resetZoom();

		/*! Increase the zoom level in the view shown by the QVTKWidget that this class renders in to.
		*/
		void zoomIn();

		/*! Increase the zoom level in the view shown by the QVTKWidget that this class renders in to.
		*/
		void zoomOut();

		/*! Reset the pan (so that the image is now centered) in the view shown by the QVTKWidget that this class renders in to.
		*/
		void resetPan();

        /*! Enable/disable interpolation on original image. This has no affect on drawing segblock or the crosshair
        *   for which no interpolation is done. The default is enabled.
        *   \param enabled should be set to true for interpolation enabled, else set false.
        */
        void enableInterpolation(bool enabled);

	signals:
		/*! Emitted when the slice shown in the view by the QVTKWidget (that this class renders in to) changes.
		*  \param slice is set to the slice that has been changed to.
		*/
		void sliceChanged(int slice);

		/*! Emitted when the view is clicked on by the left mouse button
		*   \param xVoxel is x voxel in the original image that has been clicked on.
		*   \param yVoxel is y voxel in the original image that has been clicked on.
		*   \param zVoxel is z voxel in the original image that has been clicked on.
		*/
		void viewLeftClicked(int xVoxel, int yVoxel, int zVoxel);

		/*! Emitted when the mouse is clicked, released or moved during a drag event on the QVTKWidget that this class renders in to.
		*   During a drag event many of these signals are emitted.
		*
		*   \param xVoxel is x voxel in the original image where the event occured.
		*   \param yVoxel is y voxel in the original image where the event occured.
		*   \param zVoxel is z voxel in the original image where the event occured.
		*/
		void dragEvent(int xVoxel, int yVoxel, int zVoxel);

		/*! Emitted everytime the mouse moves within the QVTKWidget that this class renders in to.
		*/
		void mouseHasMoved(ViewManager* self);

		/*! Emitted everytime the mouse moves over the QVTKWidget that this class renders in to.
		*/
		void mouseEntersWidget(ViewManager* self);

		/*! Emitted everytime the mouse leaves the QVTKWidget that this class renders in to.
		*/
		void mouseLeavesWidget(ViewManager* self);

        /*! Emitted when slice is changed, pan, zoom or left click are used */
        void requestActive();

	private slots:
		//Rebuild the LUT. Necessary if we change one of the alphas
		void buildLookUpTable();

		//Handle VTK Interactor events
		void vtkEventHandler(vtkObject* caller, unsigned long vtkEvent, void* clientData, void* callData, vtkCommand* command);

		void redrawCrossHair();
        void redrawBoundingBox();


	private:
		void addSegblock();
		void addCrosshair();
        void addBoundingBox();
		void applyCameraFixes();
		void setManager(MultiViewManager* viewManager);


		vtkSmartPointer<vtkImageViewer2> imageViewer;
		vtkSmartPointer<vtkEventQtSlotConnect> connections;
		ImagePairManager* imagePairManager;
		QVTKWidget* qvtkWidget;
		vtkSmartPointer<vtkImageActor> segblockActor;
		vtkSmartPointer<vtkLookupTable> lut;

		vtkSmartPointer<vtkLineSource> hcrosshairSource;
		vtkSmartPointer<vtkPolyDataMapper> hcrosshairMapper;
		vtkSmartPointer<vtkActor> hcrosshairActor;

		vtkSmartPointer<vtkLineSource> vcrosshairSource;
		vtkSmartPointer<vtkPolyDataMapper> vcrosshairMapper;
        vtkSmartPointer<vtkActor> vcrosshairActor;

        vtkSmartPointer<vtkCubeSource> boundingBoxSource;
        vtkSmartPointer<vtkPolyDataMapper> boundingBoxMapper;
        vtkSmartPointer<vtkActor> boundingBoxActor;
        QDoubleSpinBox* boundingBoxAlphaSpinBox;
        double boundingBoxAlpha;

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

        MultiViewManager* myManager;

		//for look up table
		double blockingAlpha;
		double segmentationAlpha;
		QDoubleSpinBox* blockingAlphaSpinBox;
		QDoubleSpinBox* segmentationAlphaSpinBox;

		//for crosshair
		double crossHairXlength; //half the horizontal crosshair length in terms of world coordinates
		double crossHairYlength; //half the vertical crosshair length in terms of world coordinates
		QDoubleSpinBox* crosshairAlphaSpinBox;
		double crossHairAlpha;

		//for panning
		bool panEnabled;
		int originalX;
		int originalY;
        int panSign; //Used for flip/nonflip view (should be 1 or -1)
        double originalCamPos[3];
        double originalCamFocalPoint[3];
        double panScale;

        unsigned int orientation;

        //for slice control
        QSpinBox* sliceSpinBox;
        QSlider* sliceSlider;

        friend class MultiViewManager;

};

#endif
