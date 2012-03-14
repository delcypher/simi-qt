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


ViewManager::ViewManager(ImagePairManager* imagePairManager, SeedPointManager* seedPointManager, QVTKWidget* qvtkWidget,  QDoubleSpinBox* blockingAlphaSpinBox, QDoubleSpinBox* segmentationAlphaSpinBox, QDoubleSpinBox* crosshairAlphaSpinBox) :
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
originalY(0)
{
    this->imagePairManager = imagePairManager;
    this->qvtkWidget = qvtkWidget;
    this->blockingAlphaSpinBox=blockingAlphaSpinBox;
    this->segmentationAlphaSpinBox=segmentationAlphaSpinBox;
    this->seedPointManager=seedPointManager;
    this->crosshairAlphaSpinBox=crosshairAlphaSpinBox;

	//setup original image
	imageViewer = vtkImageViewer2::New();
	imageViewer->SetInput(imagePairManager->original);
	qvtkWidget->SetRenderWindow(imageViewer->GetRenderWindow());
	imageViewer->SetupInteractor(qvtkWidget->GetRenderWindow()->GetInteractor());

    //setup zoom control
    maxScale= ( imagePairManager->getYDim() )*( imagePairManager->getYSpacing() )/2.0;
    minScale=1;
    zoomSteps=20;
    currentStep=0;

    //Set the default zoom at start up
    imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale(maxScale);

    //setup segblock image
    addSegblock();


    //setup crosshiar

    //Calculate half the length of the crosshair lines
    crossHairXlength = imagePairManager->getXDim()*imagePairManager->getXSpacing()/2.0;
    crossHairYlength = imagePairManager->getYDim()*imagePairManager->getYSpacing()/2.0;


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

    //If the seed point is changed by the user we should redraw the crosshair
    connect(seedPointManager, SIGNAL(seedPointChanged(int,int,int)),
            this,
            SLOT(redrawCrossHair()));



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


}

ViewManager::~ViewManager()
{

}

void ViewManager::setConstrast(double minIntensity, double maxIntensity)
{
    if(maxIntensity >= minIntensity)
    {
        imageViewer->SetColorLevel( (minIntensity + maxIntensity)/2.0 );
        imageViewer->SetColorWindow(maxIntensity - minIntensity);

        //update
        imageViewer->GetRenderWindow()->Render();
    }
}

int ViewManager::getCurrentSlice()
{
    return imageViewer->GetSlice();
}





void ViewManager::setSlice(int slice)
{


	if(slice >= imagePairManager->getZMin() && slice <= imagePairManager->getZMax())
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

		//Redraw crosshair if necessary
		redrawCrossHair();

		/* We need to make the view update so that the segblock data
		*  is drawn correctly on top of the new slice
		*/
		update();

		emit sliceChanged(slice);
	}


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
        emit mouseHasMoved();
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

            if(inImage)
                emit dragEvent(picker->GetCellIJK()[0],picker->GetCellIJK()[1], picker->GetCellIJK()[2]);

            if(panEnabled)
            {
                //record click down position
                originalX=pos[0];
                originalY=pos[1];
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

                    double currentPos[3];
                    double currentFocalPoint[3];
                    cam->GetPosition(currentPos);
                    cam->GetFocalPoint(currentFocalPoint);
                    //Reposition the camera
                    double xOffset = 0.1*(originalX - pos[0])*(imagePairManager->getXSpacing());
                    double yOffset = 0.1*(originalY - pos[1])*(imagePairManager->getYSpacing());

                    qDebug() << "Pan Xoffset: " << xOffset << " Yoffset" << yOffset;
                    cam->SetPosition(currentPos[0] + xOffset,
                                    currentPos[1] +yOffset,
                                    currentPos[2]);
                    cam->SetFocalPoint(currentFocalPoint[0] +xOffset,
                                        currentFocalPoint[1] + yOffset,
                                        currentFocalPoint[2]);
                    update();
                }
            }



		break;

        case vtkCommand::EnterEvent :
            mouseOverWidget=true;
            emit mouseIsOverWidget();
        break;

        case vtkCommand::LeaveEvent :
            mouseOverWidget=false;
            dragOn=false; //disable dragging so that when cursor goes back over widget we don't continue to drag
            emit mouseLeavesWidget();
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
    if(!seedPointManager->getSeedPoint(getCurrentSlice(),seedX,seedY))
    {
        //We shouldn't show the crosshair
        hcrosshairActor->GetProperty()->SetOpacity(0.0);
        vcrosshairActor->GetProperty()->SetOpacity(0.0);
    }
    else
    {

	double zPosition;
	vtkCamera* cam = imageViewer->GetRenderer()->GetActiveCamera();
	double projectDirection[3];
	cam->GetDirectionOfProjection(projectDirection);

	/* Use the Z-range of of the original image to tell help tell us where to place the line.
	*  We need to direction of projection so that we always place the line infront of the original image
	*  from the camera's perspective
	*/
	double* imageActorRange;
	imageActorRange = imageViewer->GetImageActor()->GetZRange();

	if(projectDirection[2] < 0)
	{
		//camera is looking down the z-axis (decreasing z)
		zPosition = imageActorRange[1] +1;
	}
	else
	{
		//camera is looking up the z-axis (increasing z)
		zPosition = imageActorRange[1] -1;
	}

	qDebug() << "guess Z pos:" << zPosition;

	hcrosshairActor->GetProperty()->SetOpacity(crossHairAlpha);
	vcrosshairActor->GetProperty()->SetOpacity(crossHairAlpha);
        double Yoffset= imagePairManager->getYSpacing()*imagePairManager->getYDim()/2.0;
        double Yposition = seedY*imagePairManager->getYSpacing();

        double Xoffset = imagePairManager->getXSpacing()*imagePairManager->getXDim()/2.0;
        double Xposition = seedX*imagePairManager->getXSpacing();

	hcrosshairSource->SetPoint1(-crossHairXlength, Yposition - Yoffset, zPosition);
	hcrosshairSource->SetPoint2( crossHairXlength, Yposition -Yoffset , zPosition);
        hcrosshairSource->Update();

	vcrosshairSource->SetPoint1(Xposition - Xoffset, -crossHairYlength, zPosition);
	vcrosshairSource->SetPoint2(Xposition - Xoffset,  crossHairYlength, zPosition);
        vcrosshairSource->Update();

        update();
        qDebug() << "Redraw cross hair at " << seedX << "," << seedY << "," << getCurrentSlice();
    }

}


void ViewManager::panning(bool enabled)
{
    panEnabled=enabled;
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
	if(flip)
		imageViewer->GetRenderer()->GetActiveCamera()->SetRoll(180.0);
	else
		imageViewer->GetRenderer()->GetActiveCamera()->SetRoll(0.0);

	update();

}

void ViewManager::zoomIn()
{
	if(currentStep  < zoomSteps)
		currentStep++;

	imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale( maxScale - currentStep*(maxScale - minScale)/( static_cast<double>(zoomSteps))  );
	imageViewer->GetRenderWindow()->Render();
}

void ViewManager::zoomOut()
{
	//make currentScale bigger
	if(currentStep > 0)
		currentStep--;


	imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale( maxScale - currentStep*(maxScale - minScale)/( static_cast<double>(zoomSteps))  );
	imageViewer->GetRenderWindow()->Render();
}

void ViewManager::resetPan()
{
	imageViewer->GetRenderer()->ResetCamera();
	imageViewer->UpdateDisplayExtent();
	imageViewer->GetRenderWindow()->Render();

}

bool ViewManager::setCrosshairAlpha(double alpha)
{
	if(alpha > 1 || alpha < 0)
		return false;

	crossHairAlpha=alpha;
	redrawCrossHair();
	return true;
}




