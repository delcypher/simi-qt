#include "drawmanager.h"
#include <QDebug>

DrawManager::DrawManager(ImagePairManager* imagePairManager, QSpinBox* drawSize, QComboBox* drawType, QSpinBox* minZSlice, QSpinBox* maxZSlice, QCheckBox* segmentation)
{
    this->imagePairManager = imagePairManager;
    this->drawSize=drawSize;
    this->drawType=drawType;
	this->minZSlice=minZSlice;
	this->maxZSlice=maxZSlice;
	this->segmentation=segmentation;
	drawSize->setValue(5);
}


DrawManager::~DrawManager()
{

}

void DrawManager::draw(int xVoxel, int yVoxel, int zVoxel)
{
	// Set draw mode and call draw algorithm function
	int mode = DrawManager::DRAW;
	drawAlgorithm(xVoxel, yVoxel, zVoxel, mode);

	//Inform other classes that we're done (so they can update)
	imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
	emit drawingDone();
}

void DrawManager::erase(int xVoxel, int yVoxel, int zVoxel)
{
	// Set draw mode and call draw algorithm function
	int mode = DrawManager::ERASE;
	drawAlgorithm(xVoxel, yVoxel, zVoxel, mode);

	//Inform other classes that we're done (so they can update)
	imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
	emit drawingDone();
}

void DrawManager::drawAlgorithm(int &xVoxel, int &yVoxel, int &zVoxel, int &mode)
{
	//Get the boundary of the segblock
	int* boundary = imagePairManager->segblock->GetExtent();

	qDebug() << "DrawManager::drawAlgorithm->boundary(" << boundary[0] << "," << boundary[1] << "," << boundary[2] << ","
			 << boundary[3] << "," << boundary[4] << "," << boundary[5] << ")";

	//Check for draw type and mode
	int blockType;
	if (mode == DrawManager::DRAW)
	{
		//Set blocking according to the drawType spinbox
		if (drawType->currentText() == "Blocking (Current Slice)" || drawType->currentText() == "Blocking (Z Slices Range)"
				|| drawType->currentText() == "Blocking (All Slices)")
		{
			blockType = ImagePairManager::BLOCKING;
		}
		else if (drawType->currentText() == "Segmentation")
		{
			blockType = ImagePairManager::SEGMENTATION;
		}


		//Set corresponding pixels to the corresponding blocking type
		for (int y = -(drawSize->value()); y <= drawSize->value(); y++)
		{
			for (int x = -(drawSize->value()); x <= drawSize->value(); x++)
			{
				//Check boundary condition
				if (xVoxel + x < boundary[0] || xVoxel + x > boundary[1] || yVoxel + y < boundary[2] || yVoxel + y > boundary[3])
				{
					qDebug() << "DrawManager::drawAlgorithm->OUT_OF_BOUND_AT(" << xVoxel + x << "," << yVoxel + y << ")";
				}
				else
				{
					//Check for blockType - whether to draw to single, particular slices, or all slices
					if (drawType->currentText() == "Blocking (All Slices)")
					{
						for (int z = boundary[4]; z <= boundary[5]; z++)
						{
							unsigned char* voxel = static_cast<unsigned char*>(imagePairManager->segblock->GetScalarPointer(xVoxel + x, yVoxel + y, z));
							if (segmentation->checkState() == 2 && *voxel == ImagePairManager::SEGMENTATION)
							{
								qDebug() << "DrawManager::drawAlgorithm->CANNOT_DRAWN_ON_SEGMENTATION(" << xVoxel + x << "," << yVoxel + y << ")";
							}
							else
							{
								*voxel = blockType;
								qDebug() << "DrawManager::drawAlgorithm->DRAWN_AT(" << xVoxel + x << "," << yVoxel + y << ") for all slices";
							}
						}
					}
					else if (drawType->currentText() == "Blocking (Z Slices Range)")
					{
						for (int z = minZSlice->value(); z <= maxZSlice->value(); z++)
						{
							unsigned char* voxel = static_cast<unsigned char*>(imagePairManager->segblock->GetScalarPointer(xVoxel + x, yVoxel + y, z));
							if (segmentation->checkState() == 2 && *voxel == ImagePairManager::SEGMENTATION)
							{
								qDebug() << "DrawManager::drawAlgorithm->CANNOT_DRAWN_ON_SEGMENTATION(" << xVoxel + x << "," << yVoxel + y << ")";
							}
							else
							{
								*voxel = blockType;
								qDebug() << "DrawManager::drawAlgorithm->DRAWN_AT(" << xVoxel + x << "," << yVoxel + y << ") for the provided Z Slices range";
							}
						}
					}
					else
					{
						unsigned char* voxel = static_cast<unsigned char*>(imagePairManager->segblock->GetScalarPointer(xVoxel + x, yVoxel + y, zVoxel));
						if (segmentation->checkState() == 2 && *voxel == ImagePairManager::SEGMENTATION)
						{
							qDebug() << "DrawManager::drawAlgorithm->CANNOT_DRAWN_ON_SEGMENTATION(" << xVoxel + x << "," << yVoxel + y << ")";
						}
						else
						{
							*voxel = blockType;
							qDebug() << "DrawManager::drawAlgorithm->DRAWN_AT(" << xVoxel + x << "," << yVoxel + y << ") for single slices";
						}
					}
				}
			}
		}
	}
	else if (mode == DrawManager::ERASE)
	{
		//Set blocking to erase
		blockType = ImagePairManager::BACKGROUND;

		//Set corresponding pixels to the corresponding blocking type
		for (int y = -(drawSize->value()); y <= drawSize->value(); y++)
		{
			for (int x = -(drawSize->value()); x <= drawSize->value(); x++)
			{
				//Check boundary condition
				if (xVoxel + x < boundary[0] || xVoxel + x > boundary[1] || yVoxel + y < boundary[2] || yVoxel + y > boundary[3])
				{
					qDebug() << "DrawManager::drawAlgorithm->OUT_OF_BOUND_AT(" << xVoxel + x << "," << yVoxel + y << ")";
				}
				else
				{
					unsigned char* voxel = static_cast<unsigned char*>(imagePairManager->segblock->GetScalarPointer(xVoxel + x, yVoxel + y, zVoxel));
					if (segmentation->checkState() == 2 && *voxel == ImagePairManager::SEGMENTATION)
					{
						qDebug() << "DrawManager::drawAlgorithm->CANNOT_DRAWN_ON_SEGMENTATION(" << xVoxel + x << "," << yVoxel + y << ")";
					}
					else
					{
						*voxel = blockType;
						qDebug() << "DrawManager::drawAlgorithm->ERASE_AT(" << xVoxel + x << "," << yVoxel + y << ")";
					}
				}
			}
		}
	}
}
