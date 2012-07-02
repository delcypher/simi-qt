#include "viewmanager.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <QDebug>
#include <vtkCellPicker.h>
#include <QVTKInteractor.h>
#include <vtkImageActor.h>
#include <vtkImageMapToColors.h>
#include <vtkInteractorStyle.h>
#include <vtkProperty.h>
#include "multiviewmanager.h"


ViewManager::ViewManager(ImagePairManager* imagePairManager, QVTKWidget* qvtkWidget, QSpinBox* sliceSpinBox, QSlider* sliceSlider, QDoubleSpinBox* blockingAlphaSpinBox, QDoubleSpinBox* segmentationAlphaSpinBox, QDoubleSpinBox* crosshairAlphaSpinBox, QDoubleSpinBox* boundingBoxAlphaSpinBox, unsigned int defaultOrientation) :
boundingBoxAlpha(0.5),
dragOn(false),
mouseX(0),
mouseY(0),
mouseZ(0),
mouseIntensity(0),
mouseOverWidget(false),
blockingAlpha(0.5),
segmentationAlpha(0.5),
crossHairAlpha(0.5),
panEnabled(false),
originalX(0),
originalY(0),
panSign(1),
panScale(1.0)
{
    //Initialise the camera position+ focal point arrays
    for(int i=0; i < 3; i++)
    {
        originalCamPos[i]=0;
        originalCamFocalPoint[i]=0;
    }

    //Setup pointers to widgets and the imagePairManager
    this->imagePairManager = imagePairManager;
    this->qvtkWidget = qvtkWidget;
    this->blockingAlphaSpinBox=blockingAlphaSpinBox;
    this->segmentationAlphaSpinBox=segmentationAlphaSpinBox;
    this->crosshairAlphaSpinBox=crosshairAlphaSpinBox;
    this->sliceSlider = sliceSlider;
    this->sliceSpinBox = sliceSpinBox;
    this->boundingBoxAlphaSpinBox=boundingBoxAlphaSpinBox;

    this->myManager=0;

	//setup original image
	imageViewer = vtkImageViewer2::New();
	imageViewer->SetInput(imagePairManager->original);
	qvtkWidget->SetRenderWindow(imageViewer->GetRenderWindow());
	imageViewer->SetupInteractor(qvtkWidget->GetRenderWindow()->GetInteractor());

    //setup zoom control defaults
    minScale=1;
    zoomSteps=20;
    currentStep=0;

    //setup segblock image
    addSegblock();

    //setup crosshair
    addCrosshair();

    //add bounding box
    addBoundingBox();

    enableInterpolation(true); //enable interpolation on original image by default

    //setup default orientation (this does zoom default too)
    setOrientation(defaultOrientation);

    //set default flip
    flipView(false);


    //If the seed point is changed by the user we should redraw the crosshair
    /* connect(seedPointManager, SIGNAL(seedPointChanged(int,int,int)),
            this,
            SLOT(redrawCrossHair())); */



    //Enable connections between VTK events and our Qt Slots
    connections = vtkEventQtSlotConnect::New();

    //Setup mousewheel connection for zoom. We use high priority so we get called before the vtkinteractorstyle
    connections->Connect(qvtkWidget->GetInteractor(),
                vtkCommand::MouseWheelForwardEvent,
                this,
                SLOT(vtkEventHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
                NULL,
                1.0);


    connections->Connect(qvtkWidget->GetInteractor(),
                vtkCommand::MouseWheelBackwardEvent,
                this,
                SLOT(vtkEventHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
                NULL,
                1.0
                );


    //setup connections for mouse dragging and left click
    connections->Connect(qvtkWidget->GetInteractor(),
		vtkCommand::LeftButtonPressEvent,
		this,
        SLOT(vtkEventHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
		NULL,
		2.0);


    connections->Connect(qvtkWidget->GetInteractor(),
		vtkCommand::MouseMoveEvent,
		this,
        SLOT(vtkEventHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
		NULL,
		2.0);

    connections->Connect(qvtkWidget->GetInteractor(),
		vtkCommand::LeftButtonReleaseEvent,
		this,
        SLOT(vtkEventHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
		NULL,
		2.0);

    //setup connections for mouseInWidget
    connections->Connect(qvtkWidget->GetInteractor(),
			vtkCommand::EnterEvent,
			this,
            SLOT(vtkEventHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
			NULL,
			1.0
			);

    connections->Connect(qvtkWidget->GetInteractor(),
			vtkCommand::LeaveEvent,
			this,
            SLOT(vtkEventHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
			NULL,
			1.0
			);

    //initialise blockingAlphaSpinBox widget
    blockingAlphaSpinBox->setRange(0.0,1.0);
    blockingAlphaSpinBox->setSingleStep(0.05);
    blockingAlphaSpinBox->setValue(blockingAlpha);
    connect(blockingAlphaSpinBox,SIGNAL(valueChanged(double)),this, SLOT(setBlockingAlpha(double)));

    //initialise segmentationAlphaSpinBox widget
    segmentationAlphaSpinBox->setRange(0.0,1.0);
    segmentationAlphaSpinBox->setSingleStep(0.05);
    segmentationAlphaSpinBox->setValue(segmentationAlpha);
    connect(segmentationAlphaSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setSegmentationAlpha(double)));

    //intialise crosshairAlphaSpinBox
    crosshairAlphaSpinBox->setRange(0.0,1.0);
    crosshairAlphaSpinBox->setSingleStep(0.05);
    crosshairAlphaSpinBox->setValue(crossHairAlpha);
    connect(crosshairAlphaSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setCrosshairAlpha(double)));

    //initialise boundingBoxAlphaSpinBox
    boundingBoxAlphaSpinBox->setRange(0.0,1.0);
    boundingBoxAlphaSpinBox->setSingleStep(0.05);
    boundingBoxAlphaSpinBox->setValue(crossHairAlpha);
    connect(boundingBoxAlphaSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setBoundingBoxAlpha(double)));




    //assume sliceSlider and sliceSpinBox already connected together
    //connect so that the user can change slice
    connect(sliceSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setSlice(int)));

    //if the slice is changed by interaction with this view inform the other widgets
    connect(this,SIGNAL(sliceChanged(int)),sliceSpinBox,SLOT(setValue(int)));
}

ViewManager::~ViewManager()
{

}

bool ViewManager::setConstrast(double minIntensity, double maxIntensity)
{
	if(minIntensity < imagePairManager->getMinimumIntensity() || maxIntensity > imagePairManager->getMaximumIntensity())
	{
		qWarning() << "ViewManager::setConstrast() : Invalid parameters passed";
		return false;
	}

	if(maxIntensity >= minIntensity)
	{
		imageViewer->SetColorLevel( (minIntensity + maxIntensity)/2.0 );
		imageViewer->SetColorWindow(maxIntensity - minIntensity);

		//update
		imageViewer->GetRenderWindow()->Render();
		return true;
	}
	else
		return false;

}

int ViewManager::getCurrentSlice()
{
    return imageViewer->GetSlice();
}

bool ViewManager::isFlipped()
{
    return (panSign==1)?false:true;
}





bool ViewManager::setSlice(int slice)
{


    if(slice >= imageViewer->GetSliceMin() && slice <= imageViewer->GetSliceMax())
	{		
		/* WORKAROUND:
		*  We record the zoom level before changing slice so we can force that zoom level after calling
		*  SetSlice() on the VtkImageViewer2 which has a bug that the zoom level changes in an undesirable way
		*  after calling SetSlice()
		*/
		double recordedZoomLevel= imageViewer->GetRenderer()->GetActiveCamera()->GetParallelScale();

		imageViewer->SetSlice(slice);
		imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale(recordedZoomLevel); //Force Zoom!

		//try to duplicate display extent so correct segblock slice is shown.
		int displayExtent[6];
		imageViewer->GetImageActor()->GetDisplayExtent(displayExtent);
		segblockActor->SetDisplayExtent(displayExtent);

		//segblockActor->SetPropertyKeys(imageViewer->GetImageActor()->GetPropertyKeys());//try duplicate props

        /* We allow changing the slice to move the crosshair */
        if(myManager!=0)
        {
            int seedX,seedY,seedZ;

            //check a seed point is set first
            if(myManager->getSeedPoint(seedX,seedY,seedZ))
            {
                switch(orientation)
                {
                    case vtkImageViewer2::SLICE_ORIENTATION_XY:
                        seedZ=getCurrentSlice();
                    break;

                    case vtkImageViewer2::SLICE_ORIENTATION_XZ:
                        seedY=getCurrentSlice();
                    break;

                    case vtkImageViewer2::SLICE_ORIENTATION_YZ:
                        seedX=getCurrentSlice();
                }

                myManager->setSeedPoint(seedX,seedY,seedZ);
            }

        }



		//Redraw crosshair if necessary
		redrawCrossHair();
        redrawBoundingBox();

		/* We need to make the view update so that the segblock data
		*  is drawn correctly on top of the new slice
		*/
		update();

		emit sliceChanged(slice);
        emit requestActive();// Request that this view be made active
		return true;
	}
	else
		return false;


}

void ViewManager::resetZoom()
{
    imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale(maxScale);
    imageViewer->GetRenderWindow()->Render();
}

void ViewManager::debugDump()
{
    /*  ImageViewer2 probe
    *
    *
    */
    double position[3]; //position in world co-ordinates
    double origin[3];
    double scale[3];
    double* xrange;
    double* yrange;
    double* zrange;
    int displayExtent[6];
    double* centre; //centre of bounding box in world co-ordinates


    vtkImageActor* actor=NULL;
    for(int i=0 ; i <2; i++)
    {
        actor=(i==0)?(imageViewer->GetImageActor()):(segblockActor.GetPointer());

        actor->GetPosition(position);
        qDebug() <<  (i==0?"Original":"segblock") << "'s ImageActor position:" << position[0] << "," << position[1] << "," << position[2];

        actor->GetOrigin(origin);
        qDebug() <<  (i==0?"Original":"segblock") << "'s ImageActor origin:" << origin[0] << "," << origin[1] << "," << origin[2];

        actor->GetScale(scale);
        qDebug() <<  (i==0?"Original":"segblock") << "'s ImageActor scale:" << scale[0] << "," << scale[1] << "," << scale[2];

        xrange=actor->GetXRange();
        yrange=actor->GetYRange();
        zrange=actor->GetZRange();
        qDebug() << (i==0?"Original":"segblock") << "'s Image Actor X range [" << xrange[0] << "," << xrange[1] << "]";
        qDebug() << (i==0?"Original":"segblock") << "'s Image Actor Y range [" << yrange[0] << "," << yrange[1] << "]";
        qDebug() << (i==0?"Original":"segblock") <<"'s Image Actor Z range [" << zrange[0] << "," << zrange[1] << "]";

        actor->GetDisplayExtent(displayExtent);
        qDebug() << (i==0?"Original":"segblock") << "'s Image Actor X display extent" << displayExtent[0] << "," << displayExtent[1];
        qDebug() << (i==0?"Original":"segblock") << "'s Image Actor Y display extent " << displayExtent[2] << "," << displayExtent[3];
        qDebug() << (i==0?"Original":"segblock") << "'s Image Actor Z display extent " << displayExtent[4] << "," << displayExtent[5];

        centre = actor->GetCenter();
        qDebug() << (i==0?"Original":"segblock") << "'s Image Actor's centre of bounding box:" << centre[0] << "," << centre[1] << "," << centre[2];

    }

    /* Camera information
    *
    */
    vtkCamera* camera = imageViewer->GetRenderer()->GetActiveCamera();
    camera->GetPosition(position);
    qDebug() << "Camera position:" << position[0] << "," << position[1] << "," << position[2];

    double viewUp[3];
    camera->GetViewUp(viewUp);
    qDebug() << "Camera view up:" << viewUp[0] << "," << viewUp[1] << "," << viewUp[2];

    double focalPoint[3];
    camera->GetFocalPoint(focalPoint);
    qDebug() << "Camera focal point:" <<  focalPoint[0] << "," << focalPoint[1] << "," << focalPoint[2];

    double projectionVector[3];
    camera->GetDirectionOfProjection(projectionVector);
    qDebug() << "Camera projection direction [from camera to focal point]:" << projectionVector[0] << "," << projectionVector[1] << "," << projectionVector[2];

    qDebug() << "Camera roll:" << camera->GetRoll();

    double clippingRange[2];
    qDebug() << "Camera cliping range:- near:" << clippingRange[0] << ", far:" << clippingRange[1];

    qDebug() << "Camera parallel scale (for zoom):" << camera->GetParallelScale();

    /*Bounding box info
     */
     double point[3];
     boundingBoxActor->GetOrigin(point);
     qDebug() << "Bounding box origin:" << point[0] << "," << point[1] << "," << point[2];

     boundingBoxSource->GetCenter(point);
     qDebug() << "Bounding box centre:" << point[0] << "," << point[1] << "," << point[2];
     qDebug() << "Lengths :" << boundingBoxSource->GetXLength() << "," << boundingBoxSource->GetYLength() << "," << boundingBoxSource->GetZLength();

     qDebug() << "xMin" << point[0] - boundingBoxSource->GetXLength();
     qDebug() << "xMax" << point[0] + boundingBoxSource->GetXLength();
     qDebug() << "yMin" << point[1] - boundingBoxSource->GetYLength();
     qDebug() << "yMax" << point[1] + boundingBoxSource->GetYLength();
     qDebug() << "zMin" << point[2] - boundingBoxSource->GetZLength();
     qDebug() << "zMax" << point[2] + boundingBoxSource->GetZLength();



}

void ViewManager::buildLookUpTable()
{
    if(lut!=0)
    {
        qDebug() << "Rebuilding look up table";
        lut->SetNumberOfTableValues(3);
        lut->SetRange(0.0,2.0);
        lut->SetTableValue(ImagePairManager::BACKGROUND,0.0,0.0,0.0,0.0); //set background
        lut->SetTableValue(ImagePairManager::BLOCKING,0.0,0.0,1.0,blockingAlpha);
        lut->SetTableValue(ImagePairManager::SEGMENTATION,1.0,0.0,0.0,segmentationAlpha);
        lut->Build();
    }
}

bool ViewManager::setBlockingAlpha(double alpha)
{
    if(alpha < 0 || alpha >1)
    {
        qWarning() << "ViewManager::setBlockingAlpha() : Cannot set alpha not in range [0,1]";
        return false;
    }

    blockingAlpha=alpha;
    qDebug() << "ViewManager::setBlockingAlpha(" << alpha << ")";
    buildLookUpTable();
    lut->Modified();
    update();
    return true;
}

bool ViewManager::setSegmentationAlpha(double alpha)
{
    if(alpha < 0 || alpha >1)
    {
        qWarning() << "ViewManager::setSegmentationAlpha() : Cannot set alpha not in range [0,1]";
        return false;
    }

    segmentationAlpha=alpha;
    qDebug() << "ViewManager::setSegmentationAlpha(Alpha(" << alpha << ")";
    buildLookUpTable();
    lut->Modified();
    update();
    return true;
}


void ViewManager::vtkEventHandler(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command)
{
    bool inImage=true; //Tells us if mouse is over the image

    //Get event co-ordinates before processing the event type

	vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::SafeDownCast(caller);

	// Get the location of the click (in window coordinates)
	int* pos = iren->GetEventPosition();

	vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();

	// Pick from this location.
	picker->Pick(pos[0], pos[1], 0, imageViewer->GetRenderer());

    //check we are in the image
    if(picker->GetCellId() != -1)
    {


        mouseX= picker->GetCellIJK()[0];
        mouseY= picker->GetCellIJK()[1];
        mouseZ= picker->GetCellIJK()[2];
        //This is VERY dirty we should work out the cast at run time!
        mouseIntensity = *(static_cast<short*>(imagePairManager->original->GetScalarPointer(mouseX, mouseY, mouseZ)));


        //Inform other classes that the mouse has moved.
        emit mouseHasMoved(this);
    }
    else
        inImage=false; //not in the image!

	switch(vtkEvent)
	{
        case vtkCommand::MouseWheelForwardEvent :
	    setSlice( getCurrentSlice() +1);

        break;

        case vtkCommand::MouseWheelBackwardEvent :
	    setSlice( getCurrentSlice() -1);

        break;

		case vtkCommand::LeftButtonPressEvent :
			dragOn = true;
			//qDebug() << "Drag start";

            //inform other classes that the view has been left clicked
            emit viewLeftClicked(mouseX, mouseY, mouseZ);
            emit requestActive();// Request that this view be made active

            if(inImage)
                emit dragEvent(picker->GetCellIJK()[0],picker->GetCellIJK()[1], picker->GetCellIJK()[2]);

            if(panEnabled)
            {
                //record click down position
                originalX=pos[0];
                originalY=pos[1];

                //record the original camera position
                double* opCam=imageViewer->GetRenderer()->GetActiveCamera()->GetPosition();
                double* ofpCam=imageViewer->GetRenderer()->GetActiveCamera()->GetFocalPoint();

                //copy into our variables ( can't use opCam or ofpCam because they change)
                for(int i=0; i < 3; i++)
                {
                    originalCamPos[i]=opCam[i];
                    originalCamFocalPoint[i]=ofpCam[i];
                }

            }

		break;

		case vtkCommand::LeftButtonReleaseEvent :
			dragOn = false;
			//qDebug() << "Drag stop";
            if(inImage)
                emit dragEvent(picker->GetCellIJK()[0],picker->GetCellIJK()[1], picker->GetCellIJK()[2]);

		break;

		case vtkCommand::MouseMoveEvent :
			if(dragOn)
			{
				//qDebug() << "Dragging...";
                if(inImage)
                    emit dragEvent(picker->GetCellIJK()[0],picker->GetCellIJK()[1], picker->GetCellIJK()[2]);

                if(panEnabled)
                {
                    //Get current camera position
                    vtkCamera* cam = imageViewer->GetRenderer()->GetActiveCamera();
                    int* widgetSize = imageViewer->GetRenderWindow()->GetSize();

                    //Reposition the camera
                    double xOffset = 2*panScale*((originalX - pos[0])/( (double) widgetSize[0]) )*(cam->GetParallelScale());
                    double yOffset = 2*panScale*((originalY - pos[1])/( (double) widgetSize[1]) ) *(cam->GetParallelScale());

                    qDebug() << "Pan Xoffset: " << xOffset << " Yoffset" << yOffset;

                    switch(orientation)
                    {
                        case vtkImageViewer2::SLICE_ORIENTATION_XY:
                                cam->SetPosition(originalCamPos[0] + panSign*xOffset,
                                    originalCamPos[1] +panSign*yOffset,
                                    originalCamPos[2]);
                                cam->SetFocalPoint(originalCamFocalPoint[0] + panSign*xOffset,
                                        originalCamFocalPoint[1] + panSign*yOffset,
                                        originalCamFocalPoint[2]);
                                break;
                        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
                                cam->SetPosition(originalCamPos[0] + panSign*xOffset,
                                    originalCamPos[1] ,
                                    originalCamPos[2] - panSign*yOffset);
                                cam->SetFocalPoint(originalCamFocalPoint[0] + panSign*xOffset,
                                        originalCamFocalPoint[1] ,
                                        originalCamFocalPoint[2] - panSign*yOffset);
                                break;

                        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
                                cam->SetPosition(originalCamPos[0] ,
                                    originalCamPos[1] + panSign*xOffset ,
                                    originalCamPos[2] +panSign*yOffset);
                                cam->SetFocalPoint(originalCamFocalPoint[0] ,
                                        originalCamFocalPoint[1] + panSign*xOffset ,
                                        originalCamFocalPoint[2] + panSign*yOffset);
                                break;

                        default:
                            qWarning() << "Orientation not supported!";
                    }


                    update();
                }
            }



		break;

        case vtkCommand::EnterEvent :
            mouseOverWidget=true;
            emit mouseEntersWidget(this);
        break;

        case vtkCommand::LeaveEvent :
            mouseOverWidget=false;
            dragOn=false; //disable dragging so that when cursor goes back over widget we don't continue to drag
            emit mouseLeavesWidget(this);
        break;

		default:
			qWarning() << "ViewManager::dragHandler() unrecognised mouse event received.";

	}

    //make sure vtkinteractorstyle doesn't catch event
    command->AbortFlagOn();
}

void ViewManager::update()
{
    qDebug() << "ViewManager::update() requested";
	imageViewer->GetRenderWindow()->Render();
	qvtkWidget->update();

}


void ViewManager::redrawCrossHair()
{
    //check if we show display the crosshair
    int seedX=0;
    int seedY=0;
    int seedZ=0;

    //check if the seed point is known
    if(myManager==0 || !(myManager->getSeedPoint(seedX,seedY,seedZ)) )
    {
        //We shouldn't show the crosshair
        hcrosshairActor->GetProperty()->SetOpacity(0.0);
        vcrosshairActor->GetProperty()->SetOpacity(0.0);
    }
    else
    {
        hcrosshairActor->GetProperty()->SetOpacity(crossHairAlpha);
        vcrosshairActor->GetProperty()->SetOpacity(crossHairAlpha);


        /* We calculate the positions of the seed point in the world co-ordinate system.
         * Unfortunately we have to take the camera into account later so we may write over some over
         * some of these values so the crosshair is shown correctly.
         */
        double xPosition, xOffset, yPosition, yOffset, zPosition;
        zPosition = imagePairManager->getZSpacing()*seedZ;

        yOffset= imagePairManager->getYSpacing()*imagePairManager->getYDim()/2.0;
        yPosition = seedY*imagePairManager->getYSpacing();

        xOffset = imagePairManager->getXSpacing()*imagePairManager->getXDim()/2.0;
        xPosition = seedX*imagePairManager->getXSpacing();

        //Get a vector describing the projection of direction
        double* projectionDir = imageViewer->GetRenderer()->GetActiveCamera()->GetDirectionOfProjection();

        switch(orientation)
        {
            case vtkImageViewer2::SLICE_ORIENTATION_XY:

                //The camera will be looking down/up the z-axis position cross hair in front of slice
                zPosition = (imageViewer->GetImageActor()->GetZRange() )[0] + ((projectionDir[2] < 0)?1:-1);

                hcrosshairSource->SetPoint1(-crossHairXlength, yPosition - yOffset, zPosition);
                hcrosshairSource->SetPoint2( crossHairXlength, yPosition -yOffset , zPosition);
                vcrosshairSource->SetPoint1(xPosition - xOffset, -crossHairYlength, zPosition);
                vcrosshairSource->SetPoint2(xPosition - xOffset,  crossHairYlength, zPosition);

            break;

            case vtkImageViewer2::SLICE_ORIENTATION_XZ:


                //The camera will be looking down/up the y-axis. Position crosshair in front of slice
                yPosition= (imageViewer->GetImageActor()->GetYRange() )[0] + ((projectionDir[1] < 0)?1:-1);

                hcrosshairSource->SetPoint1(-crossHairXlength, yPosition ,zPosition);
                hcrosshairSource->SetPoint2( crossHairXlength, yPosition ,zPosition);
                vcrosshairSource->SetPoint1(xPosition - xOffset, yPosition, 0);
                vcrosshairSource->SetPoint2(xPosition - xOffset, yPosition, 2*crossHairYlength);

            break;

            case vtkImageViewer2::SLICE_ORIENTATION_YZ:

                //The camera will looking down/up the x-axis. Position crosshair in front of slice
                xPosition = (imageViewer->GetImageActor()->GetXRange() )[0] + ((projectionDir[0] < 0)?1:-1);

                hcrosshairSource->SetPoint1(xPosition, -crossHairXlength ,zPosition);
                hcrosshairSource->SetPoint2(xPosition, crossHairXlength,zPosition);

                vcrosshairSource->SetPoint1(xPosition, yPosition - yOffset , 0);
                vcrosshairSource->SetPoint2(xPosition, yPosition - yOffset , 2*crossHairYlength);


            break;

            default:
                qWarning() << "Orientation not supported when trying to draw crosshair";

        }


        hcrosshairSource->Update();
        vcrosshairSource->Update();

        update();
        qDebug() << "Redraw cross hair at " << seedX << "," << seedY << "," << seedZ;
    }

}

void ViewManager::redrawBoundingBox()
{
    if(myManager!=0)
    {
        //calculate the dimensions of the bounding box
        double minX= (myManager->boundaryManager->getXMin() - (imagePairManager->getXDim()/2.0 ))*imagePairManager->getXSpacing();
        double maxX=(myManager->boundaryManager->getXMax() - (imagePairManager->getXDim()/2.0 ))*imagePairManager->getXSpacing();
        double minY=(myManager->boundaryManager->getYMin() - (imagePairManager->getYDim()/2.0 ))*imagePairManager->getYSpacing();
        double maxY=(myManager->boundaryManager->getYMax() - (imagePairManager->getYDim()/2.0 ))*imagePairManager->getYSpacing();
        double minZ=(myManager->boundaryManager->getZMin() )*imagePairManager->getZSpacing();
        double maxZ=(myManager->boundaryManager->getZMax() )*imagePairManager->getZSpacing();

        boundingBoxActor->GetProperty()->SetOpacity(boundingBoxAlpha);

        /*  We now tweak the bounding values depending on orientation so that the box is placed in front of the camera.
         *  The depth length is adjusted to be some small value (it doesn't matter as using orthogonal projection).
         *  If for the depth value we are outside bounding box we will hide it as we are out of boundary.
         *
         *  In doing this we are actually using the cube more as a square as we are not using the depth component.
         */
        //Get a vector describing the projection of direction
        double* projectionDir = imageViewer->GetRenderer()->GetActiveCamera()->GetDirectionOfProjection();
        switch(orientation)
        {
            case vtkImageViewer2::SLICE_ORIENTATION_XY:
                minZ = (imageViewer->GetImageActor()->GetZRange() )[0] + ((projectionDir[2] < 0)?2:-2);
                maxZ = (imageViewer->GetImageActor()->GetZRange() )[0] + ((projectionDir[2] < 0)?2:-2);

                if(!myManager->boundaryManager->isInZRange(getCurrentSlice()))
                    boundingBoxActor->VisibilityOff();
                else
                    boundingBoxActor->VisibilityOn();
            break;

            case vtkImageViewer2::SLICE_ORIENTATION_XZ:
                minY = (imageViewer->GetImageActor()->GetYRange() )[0] + ((projectionDir[1] < 0)?1:-1);
                maxY = (imageViewer->GetImageActor()->GetYRange() )[0] + ((projectionDir[1] < 0)?1:-1);

                if(!myManager->boundaryManager->isInYRange(getCurrentSlice()))
                    boundingBoxActor->VisibilityOff();
                else
                    boundingBoxActor->VisibilityOn();
            break;

            case vtkImageViewer2::SLICE_ORIENTATION_YZ:
                minX = (imageViewer->GetImageActor()->GetXRange() )[0] + ((projectionDir[0] < 0)?1:-1);
                maxX = (imageViewer->GetImageActor()->GetXRange() )[0] + ((projectionDir[0] < 0)?1:-1);


                if(!myManager->boundaryManager->isInXRange(getCurrentSlice()))
                    boundingBoxActor->VisibilityOff();
                else
                    boundingBoxActor->VisibilityOn();
            break;

        }


        boundingBoxSource->SetBounds(minX,maxX,minY,maxY,minZ,maxZ);

        boundingBoxSource->Update();

        update();
    }

}


void ViewManager::enablePanning(bool enabled)
{
    panEnabled=enabled;
}

bool ViewManager::setPanScale(double scale)
{
    //these limits are sensible guesses
    if(scale > 0.5 && scale < 3)
    {
        panScale = scale;
        return true;
    }
    else
        return false;
}

bool ViewManager::setOrientation(unsigned int ort)
{
    switch(ort)
    {
        case vtkImageViewer2::SLICE_ORIENTATION_XY :
            orientation=vtkImageViewer2::SLICE_ORIENTATION_XY;
            maxScale= ( imagePairManager->getYDim() )*( imagePairManager->getYSpacing() )/2.0;
            qDebug() << "Setting Orientation to XY";

            //calculate half crosshair lengths
            crossHairXlength = imagePairManager->getXDim()*imagePairManager->getXSpacing()/2.0;
            crossHairYlength = imagePairManager->getYDim()*imagePairManager->getYSpacing()/2.0;

        break;

        case vtkImageViewer2::SLICE_ORIENTATION_XZ :
            orientation=vtkImageViewer2::SLICE_ORIENTATION_XZ;
            maxScale= ( imagePairManager->getZDim() )*( imagePairManager->getZSpacing() )/2.0;
            qDebug() << "Setting Orientation to XZ";

            //calculate half crosshair lengths
            crossHairXlength = imagePairManager->getXDim()*imagePairManager->getXSpacing()/2.0;
            crossHairYlength = imagePairManager->getZDim()*imagePairManager->getZSpacing()/2.0;
        break;

        case vtkImageViewer2::SLICE_ORIENTATION_YZ :
            orientation=vtkImageViewer2::SLICE_ORIENTATION_YZ;
            maxScale= ( imagePairManager->getZDim() )*( imagePairManager->getZSpacing() )/2.0;
            qDebug() << "Setting Orientation to YZ";

            //calculate half crosshair lengths
            crossHairXlength = imagePairManager->getYDim()*imagePairManager->getYSpacing()/2.0;
            crossHairYlength = imagePairManager->getZDim()*imagePairManager->getZSpacing()/2.0;
        break;

        default :
            qWarning() << "setOrientation() failed. Orientation not supported!";
            return false;
    }

    imageViewer->SetSliceOrientation(orientation);

    //set default slice
    if(!setSlice(getSliceMin()) )
        qWarning() << "Failed to set default slice!";

    applyCameraFixes();// correct camera positions if necessary
    imageViewer->UpdateDisplayExtent();

    //Set the default zoom
    imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale(maxScale);


    //the widgets need to be told the valid range
    sliceSpinBox->setRange(getSliceMin(),getSliceMax());
    sliceSlider->setRange(getSliceMin(),getSliceMax());

    return true;
}

int ViewManager::getSliceMin()
{
    return imageViewer->GetSliceMin();
}

int ViewManager::getSliceMax()
{
    return imageViewer->GetSliceMax();
}

bool ViewManager::setCrosshairColour(double r, double g, double b)
{
    if( r >= 0.0 && r <= 1.0 && g >= 0.0 && g <= 1.0 && b >= 0.0 && b <= 1.0)
    {
        hcrosshairActor->GetProperty()->SetColor(r,g,b);
        vcrosshairActor->GetProperty()->SetColor(r,g,b);
        qDebug() << "Crosshair Colour set to r:" << r << " g:" << g << " b:" << b;

        redrawCrossHair();

        return true;
    }
    else
        return false;
}

void ViewManager::addSegblock()
{
	lut= vtkLookupTable::New();
	buildLookUpTable();

	vtkSmartPointer<vtkImageMapToColors> segblockMapper = vtkImageMapToColors::New();
	segblockMapper->SetLookupTable(lut);
	segblockMapper->PassAlphaToOutputOn(); //do I need this?
	segblockMapper->SetInput(imagePairManager->segblock);

	segblockActor = vtkImageActor::New();
	segblockActor->SetInput(segblockMapper->GetOutput());
	segblockActor->InterpolateOff(); //No interpolation so our voxels don't get blurred when we zoom in.
	imageViewer->GetRenderer()->AddViewProp(segblockActor);

	//try to duplicate display bounds
	int displayExtent[6];
	imageViewer->GetImageActor()->GetDisplayExtent(displayExtent);
	segblockActor->SetDisplayExtent(displayExtent);
	qvtkWidget->update();
}

void ViewManager::flipView(bool flip)
{
	double angle=0.0;

	if(flip)
    {
		panSign=-1;
		switch(orientation)
		{
            case vtkImageViewer2::SLICE_ORIENTATION_XY: angle=180.0; break;
            case vtkImageViewer2::SLICE_ORIENTATION_XZ: angle=180.0; break;
            case vtkImageViewer2::SLICE_ORIENTATION_YZ: angle=90.0; break;
            default: qWarning() << "Orientation not supported!";
		}
    }
    else
    {
        panSign=1;
        switch(orientation)
		{
            case vtkImageViewer2::SLICE_ORIENTATION_XY: angle=0.0; break;
            case vtkImageViewer2::SLICE_ORIENTATION_XZ: angle=0.0; break;
            case vtkImageViewer2::SLICE_ORIENTATION_YZ: angle=-90.0; break;
            default: qWarning() << "Orientation not supported!";
		}

    }

    imageViewer->GetRenderer()->GetActiveCamera()->SetRoll(angle);

	update();

}

void ViewManager::zoomIn()
{
	if(currentStep  < zoomSteps)
		currentStep++;

	imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale( maxScale - currentStep*(maxScale - minScale)/( static_cast<double>(zoomSteps))  );
	imageViewer->GetRenderWindow()->Render();

    emit requestActive();// Request that this view be made active
}

void ViewManager::zoomOut()
{
	//make currentScale bigger
	if(currentStep > 0)
		currentStep--;


	imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale( maxScale - currentStep*(maxScale - minScale)/( static_cast<double>(zoomSteps))  );
	imageViewer->GetRenderWindow()->Render();

    emit requestActive();// Request that this view be made active
}

void ViewManager::resetPan()
{
	imageViewer->GetRenderer()->ResetCamera();
	applyCameraFixes();
	imageViewer->UpdateDisplayExtent();
    imageViewer->GetRenderWindow()->Render();

}

void ViewManager::enableInterpolation(bool enabled)
{
    if(enabled)
    {
        imageViewer->GetImageActor()->InterpolateOn();
        qDebug() << "Original image interpolation on.";
    }
    else
    {
        imageViewer->GetImageActor()->InterpolateOff();
        qDebug() << "Original image interpolation off.";
    }

    update();

}

bool ViewManager::setCrosshairAlpha(double alpha)
{
	if(alpha > 1 || alpha < 0)
		return false;

	crossHairAlpha=alpha;
	redrawCrossHair();
    return true;
}

bool ViewManager::setBoundingBoxAlpha(double alpha)
{
    if(alpha >1 || alpha <0)
        return false;

    boundingBoxAlpha=alpha;
    redrawBoundingBox();
    return true;
}

void ViewManager::addCrosshair()
{
    /*the co-ordinates used for the crosshair are of no real consequence as they
     * are overwriten in redrawCrossHair();
     */

	hcrosshairSource = vtkLineSource::New();
	hcrosshairSource->SetPoint1(-256,0,1000);
	hcrosshairSource->SetPoint2(256,0,1000);
	hcrosshairSource->Update();

	hcrosshairMapper = vtkPolyDataMapper::New();
	hcrosshairMapper->SetInput(hcrosshairSource->GetOutput());

	hcrosshairActor = vtkActor::New();
	hcrosshairActor->SetMapper(hcrosshairMapper);
	hcrosshairActor->GetProperty()->SetOpacity(0.0); //hide by default
	hcrosshairActor->GetProperty()->SetLineWidth(2.0);
	hcrosshairActor->GetProperty()->SetColor(1.0,1.0,0.0);

	vcrosshairSource = vtkLineSource::New();
	vcrosshairSource->SetPoint1(0,-256.0,1000);
	vcrosshairSource->SetPoint2(0,256.0,1000);
	vcrosshairSource->Update();

	vcrosshairMapper = vtkPolyDataMapper::New();
	vcrosshairMapper->SetInput(vcrosshairSource->GetOutput());


	vcrosshairActor = vtkActor::New();
	vcrosshairActor->SetMapper(vcrosshairMapper);
	vcrosshairActor->GetProperty()->SetOpacity(0.0); //hide by default
	vcrosshairActor->GetProperty()->SetLineWidth(2.0);
	vcrosshairActor->GetProperty()->SetColor(1.0,1.0,0.0);

	imageViewer->GetRenderer()->AddActor(hcrosshairActor);
    imageViewer->GetRenderer()->AddActor(vcrosshairActor);
}

void ViewManager::addBoundingBox()
{
    boundingBoxSource = vtkCubeSource::New();
    boundingBoxSource->SetXLength(10);
    boundingBoxSource->SetYLength(10);
    boundingBoxSource->SetZLength(10);
    boundingBoxSource->SetCenter(0,0,0);

    boundingBoxMapper = vtkPolyDataMapper::New();
    boundingBoxMapper->SetInput(boundingBoxSource->GetOutput());

    boundingBoxActor = vtkActor::New();
    boundingBoxActor->SetMapper(boundingBoxMapper);
    boundingBoxActor->GetProperty()->SetColor(0.0,1.0,0.0);
    boundingBoxActor->GetProperty()->SetLineWidth(1.0);
    boundingBoxActor->SetOrigin(0.0,0.0,0.0);
    boundingBoxActor->GetProperty()->SetRepresentationToWireframe();
    boundingBoxActor->SetPickable(false);
    boundingBoxActor->GetProperty()->SetOpacity(boundingBoxAlpha);

    /* These fix looking at the wireframe cube from other perspectives.
     * Without it we don't see the proper colour and see nothing or black lines!
     * I have no idea why this works. This is copied from a mailing list post
     *
     * http://vtk.1045678.n5.nabble.com/two-sided-rendering-of-wireframe-td1232972l.html
     */
    boundingBoxActor->GetProperty()->SetAmbient(1.0);
    boundingBoxActor->GetProperty()->SetDiffuse(0.0);
    boundingBoxActor->GetProperty()->SetSpecular(0.0);


    imageViewer->GetRenderer()->AddActor(boundingBoxActor);

}

void ViewManager::applyCameraFixes()
{
    vtkCamera* camera = imageViewer->GetRenderer()->GetActiveCamera();
    if(camera==NULL) return;


    switch(orientation)
    {
        case vtkImageViewer2::SLICE_ORIENTATION_XY:
            camera->SetPosition(0,0,imagePairManager->getZDim()*imagePairManager->getZSpacing()*2.0);
            camera->SetPosition(0,0,imagePairManager->getZDim()*imagePairManager->getZSpacing()/2.0);

        break;

        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
            /* Workaround:
             * The camera is incorrectly positioned along the z-axis initially so image cannot be seen.
             * We are also looking up the positive y-axis (0,1,0) which I don't want so we make it so we are
             * looking down the negative y-axis instead (0,-1,0)
             */
            camera->SetPosition(0,
                                imagePairManager->getYDim()*imagePairManager->getYSpacing()*2.0,
                                (imagePairManager->getZDim()*imagePairManager->getZSpacing())/2.0
                                );
            camera->SetFocalPoint(0,
                                0,
                                (imagePairManager->getZDim()*imagePairManager->getZSpacing())/2.0
                                );
        break;

        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
            /* Workaround:
             * The camera is incorrectly positioned along the z-axis initially so image
             * cannot be seen. Fix this
             */
             camera->SetPosition(imagePairManager->getXDim()*imagePairManager->getXSpacing()*2.0,
                                0,
                                (imagePairManager->getZDim()*imagePairManager->getZSpacing())/2.0
                                );
             camera->SetFocalPoint(0,0,(imagePairManager->getZDim()*imagePairManager->getZSpacing())/2.0);
        break;
    }

    /* Workaround:
     * When we manually adjust the camera position the clipping planes appear (as far as I cant tell) to prevent
     * the image from being displayed. We workaround this by setting an excessively large distance between clipping planes so that
     * the entire image is guaranteed to be between the planes (assuming the previous code positioning the camrea is correct).
     * The VtkImageViewer2 widget will later correct this clipping distance to something more sensible.
     */

    camera->SetClippingRange(0.1,camera->GetDistance());
}

void ViewManager::setManager(MultiViewManager* viewManager)
{
    if(viewManager!=0)
    {
        this->myManager=viewManager;
        //It is now safe/possible to draw the bounding box directly
        redrawBoundingBox();
    }
}
