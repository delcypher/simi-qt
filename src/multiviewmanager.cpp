#include "multiviewmanager.h"
#include <QDebug>


MultiViewManager::MultiViewManager(BoundaryManager* boundaryManager,
ViewManager *xy,
ViewManager *xz,
ViewManager *yz,
QPushButton *xyButton,
QPushButton *xzButton,
QPushButton *yzButton,
unsigned int defaultView=vtkImageViewer2::SLICE_ORIENTATION_XY) :
xSeed(-1),
ySeed(-1),
zSeed(-1)
{
    this->boundaryManager = boundaryManager;

    //if the boundary changes we may need to change the seed point colour (informs user seed point out of boundary)
    connect(boundaryManager,SIGNAL(boundaryChanged()),this,SLOT(checkCrosshairInBoundary()));

    //add view managers
    xyView = xy;
    xzView = xz;
    yzView = yz;

    //A view may request to become the active view
    connect(xy,SIGNAL(requestActive()),this,SLOT(setXYActive()));
    connect(xz,SIGNAL(requestActive()),this,SLOT(setXZActive()));
    connect(yz,SIGNAL(requestActive()),this,SLOT(setYZActive()));

    //setup buttons
    this->xyButton = xyButton;
    this->xzButton = xzButton;
    this->yzButton = yzButton;

    //connect buttons
    connect(xyButton,SIGNAL(clicked()),this,SLOT(setXYActive()));
    connect(xzButton,SIGNAL(clicked()), this, SLOT(setXZActive()));
    connect(yzButton,SIGNAL(clicked()), this, SLOT(setYZActive()));

    //setup default view
    switch(defaultView)
    {
        case vtkImageViewer2::SLICE_ORIENTATION_XY:
            setXYActive();
            break;
        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
            setXZActive();
            break;
        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
            setYZActive();
            break;
        default:
            qWarning() << "Unable to set default view!";

    }

    //Let the views have a pointer to us
    xy->setManager(this);
    xz->setManager(this);
    yz->setManager(this);
}

unsigned int MultiViewManager::getActiveView()
{
    return activeView;
}

ViewManager *MultiViewManager::getActiveViewPointer()
{
    switch(activeView)
    {
        case vtkImageViewer2::SLICE_ORIENTATION_XY:
            return xyView;
        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
            return xzView;
        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
            return yzView;
        default:
            qWarning() << "Unable to get active view pointer!";
            return NULL;

    }
}

bool MultiViewManager::getSeedPoint(int &x, int &y, int &z)
{
    x= xSeed;
    y= ySeed;
    z= zSeed;
    if(x==-1 && y==-1 && z==-1)
        return false;//no seed point set
    else
        return true;
}

bool MultiViewManager::setSeedPoint(int x, int y, int z)
{
    //check seed point is in image range
    //TODO

    xSeed=x;
    ySeed=y;
    zSeed=z;
    emit seedPointChanged();
    qDebug() << "Seed point set to:(" << xSeed << "," << ySeed << "," << zSeed << ")";

    checkCrosshairInBoundary();

    return true;
}

void MultiViewManager::redrawCrosshair()
{
    //make views redraw the crosshair
    xyView->redrawCrossHair();
    xzView->redrawCrossHair();
    yzView->redrawCrossHair();

}

void MultiViewManager::checkCrosshairInBoundary()
{
    //Note no need to call redrawCrosshair() as these calls do it automatically
    if(isCrosshairInBoundary())
    {
        xyView->setCrosshairColour(1.0,1.0,0.0); // Yellow in boundary colour
        xzView->setCrosshairColour(1.0,1.0,0.0); // Yellow in boundary colour
        yzView->setCrosshairColour(1.0,1.0,0.0); // Yellow in boundary colour
    }
    else
    {
        xyView->setCrosshairColour(1.0,0.0,1.0); // Fuschia in boundary colour
        xzView->setCrosshairColour(1.0,0.0,1.0); // Fuschia in boundary colour
        yzView->setCrosshairColour(1.0,0.0,1.0); // Fuschia in boundary colour
    }


}

void MultiViewManager::update()
{
    if(xyView != 0 && xzView !=0 && yzView!=0)
    {
        xyView->update();
        xzView->update();
        yzView->update();
    }
}

void MultiViewManager::enableInterpolation(bool enable)
{
    if(xyView != 0 && xzView !=0 && yzView!=0)
    {
        xyView->enableInterpolation(enable);
        xzView->enableInterpolation(enable);
        yzView->enableInterpolation(enable);
    }
}

void MultiViewManager::enablePanning(bool e)
{
    if(xyView!=0 && xzView !=0 && yzView!=0)
    {
        xyView->enablePanning(e);
        xzView->enablePanning(e);
        yzView->enablePanning(e);
        qDebug() << (e?"Enabling":"Disabling") << " panning";
    }
    else
        qWarning() << "Failed to enable/disable panning";
}

bool MultiViewManager::isCrosshairInBoundary()
{
    //Note no need to call redrawCrosshair() as these calls do it automatically
    if(boundaryManager->isInBoundary(xSeed,ySeed,zSeed))
        return true;
    else
        return false;
}


void MultiViewManager::setXYActive()
{
    activeView = vtkImageViewer2::SLICE_ORIENTATION_XY;

    xyButton->setChecked(true);
    xzButton->setChecked(false);
    yzButton->setChecked(false);
    emit viewChanged(activeView);
    qDebug() << "XY activated.";
}

void MultiViewManager::setXZActive()
{
    activeView = vtkImageViewer2::SLICE_ORIENTATION_XZ;

    xyButton->setChecked(false);
    xzButton->setChecked(true);
    yzButton->setChecked(false);
    emit viewChanged(activeView);
    qDebug() << "XZ activated.";
}

void MultiViewManager::setYZActive()
{
    activeView = vtkImageViewer2::SLICE_ORIENTATION_YZ;

    xyButton->setChecked(false);
    xzButton->setChecked(false);
    yzButton->setChecked(true);
    emit viewChanged(activeView);
    qDebug() << "YZ activated.";
}
