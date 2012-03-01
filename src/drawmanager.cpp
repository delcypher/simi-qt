#include "drawmanager.h"
#include <QDebug>

DrawManager::DrawManager(ImagePairManager* imageManager, QSpinBox* drawSize, QComboBox* drawType)
{
	this->imageManager = imageManager;
    this->drawSize=drawSize;
    this->drawType=drawType;
}


DrawManager::~DrawManager()
{

}

void DrawManager::draw(int xVoxel, int yVoxel, int zVoxel)
{
	qDebug() << "DrawManager::draw(" << xVoxel << "," << yVoxel << "," << zVoxel << ")";
}

void DrawManager::erase(int xVoxel, int yVoxel, int zVoxel)
{
	qDebug() << "DrawManager::erase(" << xVoxel << "," << yVoxel << "," << zVoxel << ")";
}
