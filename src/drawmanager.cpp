#include "drawmanager.h"
#include <QDebug>

DrawManager::DrawManager(ImagePairManager* imagePairManager, QSpinBox* drawSize, QComboBox* drawType)
{
    this->imagePairManager = imagePairManager;
    this->drawSize=drawSize;
    this->drawType=drawType;
}


DrawManager::~DrawManager()
{

}

void DrawManager::draw(int xVoxel, int yVoxel, int zVoxel)
{
	qDebug() << "DrawManager::draw(" << xVoxel << "," << yVoxel << "," << zVoxel << ")";

	//Inform other classes that we're done (so they can update)
	emit drawingDone();
}

void DrawManager::erase(int xVoxel, int yVoxel, int zVoxel)
{
	qDebug() << "DrawManager::erase(" << xVoxel << "," << yVoxel << "," << zVoxel << ")";

	//Inform other classes that we're done (so they can update)
	emit drawingDone();
}
