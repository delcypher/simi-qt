#include "viewmanager.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <QDebug>
#include <vtkCellPicker.h>
#include <QVTKInteractor.h>
#include <vtkImageActor.h>
#include <vtkImageMapToColors.h>


ViewManager::ViewManager(ImagePairManager* imagePairManager, SeedPointManager* seedPointManager, QVTKWidget* qvtkWidget,  QDoubleSpinBox* blockingAlphaSpinBox, QDoubleSpinBox* segmentationAlphaSpinBox) :
dragOn(false),
mouseX(0),
mouseY(0),
mouseZ(0),
mouseIntensity(0),
mouseOverWidget(false),
blockingAlpha(0.5),
segmentationAlpha(0.5)
{
    this->imagePairManager = imagePairManager;
    this->qvtkWidget = qvtkWidget;
    this->blockingAlphaSpinBox=blockingAlphaSpinBox;
    this->segmentationAlphaSpinBox=segmentationAlphaSpinBox;
    this->seedPointManager=seedPointManager;

	//setup original image
	imageViewer = vtkImageViewer2::New();
	imageViewer->SetInput(imagePairManager->original);
	qvtkWidget->SetRenderWindow(imageViewer->GetRenderWindow());
	imageViewer->SetupInteractor(qvtkWidget->GetRenderWindow()->GetInteractor());

    //setup zoom control
    maxScale= ( imagePairManager->getYDim() )*( imagePairManager->getYSpacing() )/2.0;

    imageViewer->GetRenderer()->GetActiveCamera()->SetParallelScale(maxScale);


    //setup segblock image
    addSegblock();


    //setup seedpoint drawing?

    //Enable connections between VTK events and our Qt Slots
    connections = vtkEventQtSlotConnect::New();

    //Setup mousewheel connection for zoom. We use high priority so we get called before the vtkinteractorstyle
    connections->Connect(qvtkWidget->GetInteractor(),
                vtkCommand::MouseWheelForwardEvent,
                this,
                SLOT(mouseWheelForward(vtkObject*,ulong,void*,void*,vtkCommand*)),
                NULL,
                1.0);


    connections->Connect(qvtkWidget->GetInteractor(),
                vtkCommand::MouseWheelBackwardEvent,
                this,
                SLOT(mouseWheelBackward(vtkObject*,ulong,void*,void*,vtkCommand*)),
                NULL,
                1.0
                );

    //setup connection for simple left click
    connections->Connect(qvtkWidget->GetInteractor(),
			vtkCommand::LeftButtonPressEvent,
			this,
			SLOT(mouseLeftClick(vtkObject*,ulong,void*,void*,vtkCommand*)),
			NULL,
			1.0);

    //setup connections for mouse dragging
    connections->Connect(qvtkWidget->GetInteractor(),
		vtkCommand::LeftButtonPressEvent,
		this,
		SLOT(dragHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
		NULL,
		2.0);


    connections->Connect(qvtkWidget->GetInteractor(),
		vtkCommand::MouseMoveEvent,
		this,
		SLOT(dragHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
		NULL,
		2.0);

    connections->Connect(qvtkWidget->GetInteractor(),
		vtkCommand::LeftButtonReleaseEvent,
		this,
		SLOT(dragHandler(vtkObject*,ulong,void*,void*,vtkCommand*)),
		NULL,
		2.0);

    //setup connections for mouseInWidget
    connections->Connect(qvtkWidget->GetInteractor(),
			vtkCommand::EnterEvent,
			this,
			SLOT(enterLeaveHandler(vtkObject*,ulong)),
			NULL,
			1.0
			);

    connections->Connect(qvtkWidget->GetInteractor(),
			vtkCommand::LeaveEvent,
			this,
			SLOT(enterLeaveHandler(vtkObject*,ulong)),
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

vtkRenderer *ViewManager::getRenderer()
{
    if(imageViewer!=0)
        return imageViewer->GetRenderer();
    else
        return NULL;

}

void ViewManager::mouseWheelForward(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command)
{

    ChangeSlice( getCurrentSlice() -1);

    //Prevent vtkInteractorStyle from intercepting the event
    command->AbortFlagOn();
}

void ViewManager::mouseWheelBackward(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command)
{
    ChangeSlice( getCurrentSlice() +1);

    //Prevent vtkInteractorStyle from intercepting the event
    command->AbortFlagOn();
}


void ViewManager::ChangeSlice(int slice)
{
	if(slice >= imagePairManager->getZMin() && slice <= imagePairManager->getZMax())
	{
        imageViewer->SetSlice(slice);

        //try to duplicate display extent so correct segblock slice is shown.
        int displayExtent[6];
        imageViewer->GetImageActor()->GetDisplayExtent(displayExtent);
        segblockActor->SetDisplayExtent(displayExtent);

        //segblockActor->SetPropertyKeys(imageViewer->GetImageActor()->GetPropertyKeys());//try duplicate props


        emit sliceChanged(slice);
	}

    //workaround bug where changing slice causes zoom to reset to something unexpected
    resetZoom();
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


void ViewManager::mouseLeftClick(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command)
{
	vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::SafeDownCast(caller);

	// Get the location of the click (in window coordinates)
	int* pos = iren->GetEventPosition();

	qDebug() << "ViewManager::mouseLeftClick() : Window position is:" << pos[0] << pos[1];

	vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();

	// Pick from this location.
	picker->Pick(pos[0], pos[1], 0, imageViewer->GetRenderer());

	if(picker->GetCellId() != -1)
	{
		qDebug() << "ViewManager::mouseLeftClick() : IJK:" << picker->GetCellIJK()[0] << "," << picker->GetCellIJK()[1] << "," << picker->GetCellIJK()[2] << " intensity:" <<
		getLastMouseIntensity();
		;


		emit viewLeftClicked(picker->GetCellIJK()[0], picker->GetCellIJK()[1], picker->GetCellIJK()[2]);
	}
	else
		qDebug() << "ViewManager::mouseLeftClick() : Out of range!";

	//make sure vtkinteractorstyle doesn't catch event
	command->AbortFlagOn();
}

void ViewManager::dragHandler(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command)
{
	//Get event co-ordinates before processing the event type

	vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::SafeDownCast(caller);

	// Get the location of the click (in window coordinates)
	int* pos = iren->GetEventPosition();

	vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();

	// Pick from this location.
	picker->Pick(pos[0], pos[1], 0, imageViewer->GetRenderer());

	//check we are in the image
	if(picker->GetCellId() == -1)
		return; //not in the image!

	mouseX= picker->GetCellIJK()[0];
	mouseY= picker->GetCellIJK()[1];
	mouseZ= picker->GetCellIJK()[2];
	//This is VERY dirty we should work out the cast at run time!
	mouseIntensity = *(static_cast<short*>(imagePairManager->original->GetScalarPointer(mouseX, mouseY, mouseZ)));

	//Inform other classes that the mouse has moved.
	emit mouseHasMoved();

	switch(vtkEvent)
	{
		case vtkCommand::LeftButtonPressEvent :
			dragOn = true;
			//qDebug() << "Drag start";
			emit dragEvent(picker->GetCellIJK()[0],picker->GetCellIJK()[1], picker->GetCellIJK()[2]);

		break;

		case vtkCommand::LeftButtonReleaseEvent :
			dragOn = false;
			//qDebug() << "Drag stop";
			emit dragEvent(picker->GetCellIJK()[0],picker->GetCellIJK()[1], picker->GetCellIJK()[2]);

		break;

		case vtkCommand::MouseMoveEvent :
			if(dragOn)
			{
				//qDebug() << "Dragging...";
				emit dragEvent(picker->GetCellIJK()[0],picker->GetCellIJK()[1], picker->GetCellIJK()[2]);
			}



		break;

		default:
			qWarning() << "ViewManager::dragHandler() unrecognised mouse event received.";

	}
}

void ViewManager::update()
{
	qDebug() << "ViewManager::update() requested";

	/* Rebuilding the LUT is a HACK.
	*  When using a VtkStructuredPoints from a VtkStructuredPointsReader using
	*  segblock->Modified() does not seem to make VTK realise that the pipeline needs updating.
	*  Forcing the LUT to be regenerated is a WORKAROUND!
	*/
	buildLookUpTable();

	imageViewer->GetRenderWindow()->Render();
	qvtkWidget->update();


}

void ViewManager::enterLeaveHandler(vtkObject *caller, unsigned long vtkEvent)
{

	switch(vtkEvent)
	{
		case vtkCommand::EnterEvent :
			//qDebug() << "enterLeaveHandler() : Enter";
			mouseOverWidget=true;
			emit mouseIsOverWidget();
			break;
		case vtkCommand::LeaveEvent :
			//qDebug() << "enterLeaveHandler() : Leave";
			mouseOverWidget=false;
			dragOn=false; //disable dragging so that when cursor goes back over widget we don't continue to drag
			emit mouseLeavesWidget();
			break;
		default:
			qWarning() << "Unexpected event received in ViewManager::enterLeaveHandler()";

	}
}

void ViewManager::reBuildPipeline()
{
	imageViewer->GetRenderer()->RemoveActor(segblockActor);
	addSegblock();
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



