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

	// Set draw mode and call draw algorithm function
	int mode = DrawManager::DRAW;
	drawAlgorithm(xVoxel, yVoxel, zVoxel, mode);

	//Inform other classes that we're done (so they can update)
	imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
	emit drawingDone();
}

void DrawManager::erase(int xVoxel, int yVoxel, int zVoxel)
{
	qDebug() << "DrawManager::erase(" << xVoxel << "," << yVoxel << "," << zVoxel << ")";

	// Set draw mode and call draw algorithm function
	int mode = DrawManager::ERASE;
	drawAlgorithm(xVoxel, yVoxel, zVoxel, mode);

	//Inform other classes that we're done (so they can update)
	imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
	emit drawingDone();
}

void DrawManager::drawAlgorithm(int &xVoxel, int &yVoxel, int &zVoxel, int &mode)
{
	//Check for draw type and mode
	int blockType;
	if (mode == DrawManager::DRAW)
	{
		if (drawType->currentText() == "Blocking") { blockType = ImagePairManager::BLOCKING; }
		else if (drawType->currentText() == "Segmentation") { blockType = ImagePairManager::SEGMENTATION; }
	}
	else if (mode == DrawManager::ERASE) { blockType = ImagePairManager::BACKGROUND; }

	//Get the boundary of the segblock
	int* boundary = imagePairManager->segblock->GetExtent();

	qDebug() << "DrawManager::drawAlgorithm->boundary(" << boundary[0] << "," << boundary[1] << "," << boundary[2] << ","
			 << boundary[3] << "," << boundary[4] << "," << boundary[5] << ")";

	//Set corresponding pixels to the corresponding mode
	for (int y = -(drawSize->value()); y <= drawSize->value(); y++)
	{
		for (int x = -(drawSize->value()); x <= drawSize->value(); x++)
		{
			//Check boundary condition
			if (xVoxel + x < boundary[0] || xVoxel + x > boundary[1] || yVoxel + y < boundary[2] || yVoxel + y > boundary[3])
			{
				qDebug() << "DrawManager::drawAlgorithm::REPORT->OUT_OF_BOUND_AT(" << xVoxel + x << "," << yVoxel + y << ","
						 << zVoxel << ")";
			}
			else
			{
				short* voxel = static_cast<short*>(imagePairManager->segblock->GetScalarPointer(xVoxel + x, yVoxel + y, zVoxel));
				*voxel = blockType;

				qDebug() << "DrawManager::drawAlgorithm::REPORT->DRAWN_AT(" << xVoxel + x << "," << yVoxel + y << ","
						 << zVoxel << ")";
			}
		}
	}
}
