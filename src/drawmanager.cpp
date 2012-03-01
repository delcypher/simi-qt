#include "drawmanager.h"
#include <QDebug>

DrawManager::DrawManager(ImagePairManager* imageManager)
{
	this->imageManager = imageManager;
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
