#include "multiviewmanager.h"
#include <QDebug>


MultiViewManager::MultiViewManager(ViewManager *xy,
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
    qDebug() << "Seed point set to:(" << xSeed << "," << ySeed << "," << zSeed << ")";
    return true;
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


void MultiViewManager::setXYActive()
{
    activeView = vtkImageViewer2::SLICE_ORIENTATION_XY;

    xyButton->setChecked(true);
    xzButton->setChecked(false);
    yzButton->setChecked(false);
    emit viewChanged();
    qDebug() << "XY activated.";
}

void MultiViewManager::setXZActive()
{
    activeView = vtkImageViewer2::SLICE_ORIENTATION_XZ;

    xyButton->setChecked(false);
    xzButton->setChecked(true);
    yzButton->setChecked(false);
    emit viewChanged();
    qDebug() << "XZ activated.";
}

void MultiViewManager::setYZActive()
{
    activeView = vtkImageViewer2::SLICE_ORIENTATION_YZ;

    xyButton->setChecked(false);
    xzButton->setChecked(false);
    yzButton->setChecked(true);
    emit viewChanged();
    qDebug() << "YZ activated.";
}
