#include "boundarymanager.h"
#include <QDebug>

BoundaryManager::BoundaryManager(ImagePairManager* imagePairManager,
                                QSpinBox* xMin,
                                QSpinBox* xMax,
                                QSpinBox* yMin,
                                QSpinBox* yMax,
                                QSpinBox* zMin,
                                QSpinBox* zMax,
                                QPushButton* resetButton,
                                QObject *parent = 0) :
                                QObject(parent),
                                xMinSpinBox(xMin),
                                xMaxSpinBox(xMax),
                                yMinSpinBox(yMin),
                                yMaxSpinBox(yMax),
                                zMinSpinBox(zMin),
                                zMaxSpinBox(zMax),
                                resetPushButton(resetButton)
{
    this->imagePairManager = imagePairManager;

    //set widget limits
    xMinSpinBox->setMinimum(imagePairManager->getXMin());
    xMinSpinBox->setMaximum(imagePairManager->getXMax());
    xMaxSpinBox->setMinimum(imagePairManager->getXMin());
    xMaxSpinBox->setMaximum(imagePairManager->getXMax());

    yMinSpinBox->setMinimum(imagePairManager->getYMin());
    yMinSpinBox->setMaximum(imagePairManager->getYMax());
    yMaxSpinBox->setMinimum(imagePairManager->getYMin());
    yMaxSpinBox->setMaximum(imagePairManager->getYMax());

    zMinSpinBox->setMinimum(imagePairManager->getZMin());
    zMinSpinBox->setMaximum(imagePairManager->getZMax());
    zMaxSpinBox->setMinimum(imagePairManager->getZMin());
    zMaxSpinBox->setMaximum(imagePairManager->getZMax());

    //connect signals and slots
    connect(xMinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(on_xMin_valueChanged(int)));
    connect(xMaxSpinBox,SIGNAL(valueChanged(int)),this,SLOT(on_xMax_valueChanged(int)));
    connect(yMinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(on_yMin_valueChanged(int)));
    connect(yMaxSpinBox,SIGNAL(valueChanged(int)),this,SLOT(on_yMax_valueChanged(int)));
    connect(zMinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(on_zMin_valueChanged(int)));
    connect(zMaxSpinBox,SIGNAL(valueChanged(int)),this,SLOT(on_zMax_valueChanged(int)));

    connect(resetPushButton,SIGNAL(clicked()),this,SLOT(reset()));


    //set to default
    reset();
}

int BoundaryManager::getXMin()
{
    return xMinSpinBox->value();
}

int BoundaryManager::getXMax()
{
    return xMaxSpinBox->value();
}

int BoundaryManager::getYMin()
{
    return yMinSpinBox->value();
}

int BoundaryManager::getYMax()
{
    return yMaxSpinBox->value();
}

int BoundaryManager::getZMin()
{
    return zMinSpinBox->value();
}

int BoundaryManager::getZMax()
{
    return zMaxSpinBox->value();
}

bool BoundaryManager::isInBoundary(int x, int y, int z)
{
    qDebug() << "Is " << x << "," << y << "," << z << " is boundary?";

    if(x < xMinSpinBox->value() || x > xMaxSpinBox->value() || y < yMinSpinBox->value() || y > yMaxSpinBox->value() || z < zMinSpinBox->value() || z > zMaxSpinBox->value())
        return false; // not inside boundary
    else
        return true; // point is inside boundary
}

bool BoundaryManager::isInXRange(int x)
{
    return (x < xMinSpinBox->value() || x > xMaxSpinBox->value())?false:true;
}

bool BoundaryManager::isInYRange(int y)
{
    return (y < yMinSpinBox->value() || y > yMaxSpinBox->value())?false:true;
}

bool BoundaryManager::isInZRange(int z)
{
    return (z < zMinSpinBox->value() || z > zMaxSpinBox->value())?false:true;
}

bool BoundaryManager::reset()
{
    if(imagePairManager!=0)
    {
        xMinSpinBox->setValue(imagePairManager->getXMin());
        xMaxSpinBox->setValue(imagePairManager->getXMax());
        yMinSpinBox->setValue(imagePairManager->getYMin());
        yMaxSpinBox->setValue(imagePairManager->getYMax());
        zMinSpinBox->setValue(imagePairManager->getZMin());
        zMaxSpinBox->setValue(imagePairManager->getZMax());

        return true;
    }
    else
        return false;
}

void BoundaryManager::on_xMin_valueChanged(int value)
{
    //prevent min > max
    if(value > xMaxSpinBox->value())
        xMinSpinBox->setValue(xMaxSpinBox->value());

    emit boundaryChanged();
}

void BoundaryManager::on_xMax_valueChanged(int value)
{
    //prevent max < min
    if(value < xMinSpinBox->value())
        xMaxSpinBox->setValue(xMinSpinBox->value());

    emit boundaryChanged();
}

void BoundaryManager::on_yMin_valueChanged(int value)
{
    //prevent min > max
    if(value > yMaxSpinBox->value())
        yMinSpinBox->setValue(yMaxSpinBox->value());

    emit boundaryChanged();
}

void BoundaryManager::on_yMax_valueChanged(int value)
{
    //prevent max < min
    if(value < yMinSpinBox->value())
        yMaxSpinBox->setValue(yMinSpinBox->value());

    emit boundaryChanged();
}

void BoundaryManager::on_zMin_valueChanged(int value)
{
    //prevent min > max
    if(value > zMaxSpinBox->value())
        zMinSpinBox->setValue(zMaxSpinBox->value());

    emit boundaryChanged();
}

void BoundaryManager::on_zMax_valueChanged(int value)
{
    //prevent max < min
    if(value < zMinSpinBox->value())
        zMaxSpinBox->setValue(zMinSpinBox->value());

    emit boundaryChanged();
}
