#include "drawmanager.h"
#include <QDebug>
#include <vtkImageViewer2.h>

DrawManager::DrawManager(ImagePairManager* imagePairManager,
                    QSpinBox* drawSize,
                    QComboBox* drawType,
                    BoundaryManager* boundaryManager,
                    QCheckBox* segmentation,
                    unsigned int initialOrientation){
    this->imagePairManager = imagePairManager;
    this->drawSize=drawSize;
    this->drawType=drawType;
    this->segReadOnly=segReadOnly;
    this->boundaryManager=boundaryManager;
    this->segReadOnly = segmentation;

    //setup widgets
    drawSize->setValue(5);// we do this here as we only want to call it once!
    setupWidgets(initialOrientation);
}


DrawManager::~DrawManager()
{

}

void DrawManager::draw(int xVoxel, int yVoxel, int zVoxel)
{
	// Set draw mode and call draw algorithm function
    int mode =0;// DrawManager::DRAW;
	drawAlgorithm(xVoxel, yVoxel, zVoxel, mode);

	//Inform other classes that we're done (so they can update)
	imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
	emit drawingDone();
}

void DrawManager::erase(int xVoxel, int yVoxel, int zVoxel)
{
	// Set draw mode and call draw algorithm function
    int mode = 0;//DrawManager::ERASE;
	drawAlgorithm(xVoxel, yVoxel, zVoxel, mode);

	//Inform other classes that we're done (so they can update)
	imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
    emit drawingDone();
}

void DrawManager::setupWidgets(unsigned int ort)
{
    //record the new orientation
    orientation=ort;

    drawSize->setMinimum(1);
    drawSize->setMaximum(20);

    //record currently selected
    int selectedIndex = drawType->currentIndex();

    //Initally when the comboBox is empty there is no currentIndex
    if(selectedIndex ==-1)
        selectedIndex=0;

    /* Remove all items from combo box. We must do this
     * backwards so the index of items does not change
     */
    for(int i= drawType->count() -1; i >=0 ; i--)
        drawType->removeItem(i);

    //repopulate combo box

    //Add just blocking current slice
    drawType->insertItem(BLOCKING_SINGLE_SLICE,"Blocking (single slice)");

    //add different text depending on orientation
    switch(orientation)
    {
        case vtkImageViewer2::SLICE_ORIENTATION_XY:
            drawType->insertItem(BLOCKING_MULTIPLE_SLICES,"Blocking (bounded slices in Z)");
        break;
        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
            drawType->insertItem(BLOCKING_MULTIPLE_SLICES,"Blocking (bounded slices in Y)");
        break;
        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
            drawType->insertItem(BLOCKING_MULTIPLE_SLICES,"Blocking (bounded slices in X)");
        break;
        default:
            qWarning() << "Orientation not supported!";

    }

    drawType->insertItem(SEGMENTATION_SINGLE_SLICE,"Segmentation (single slice)");

    //not set the active option
    drawType->setCurrentIndex(selectedIndex);
}

void DrawManager::drawAlgorithm(int &xVoxel, int &yVoxel, int &zVoxel, int &mode)
{
//	//Get the boundary of the segblock
//	int* boundary = imagePairManager->segblock->GetExtent();

//	qDebug() << "DrawManager::drawAlgorithm->boundary(" << boundary[0] << "," << boundary[1] << "," << boundary[2] << ","
//			 << boundary[3] << "," << boundary[4] << "," << boundary[5] << ")";

//	//Initialise require variables
//	int blockType;
//	int yLim = drawSize->value() - 1;
//	int xLim = drawSize->value() - 1;

//	//Check for draw type
//	if (mode == DrawManager::DRAW)
//	{
//		//Set blocking according to the drawType spinbox
//		if (drawType->currentText() == "Blocking (Current Slice)" || drawType->currentText() == "Blocking (Z Slices Range)"
//				|| drawType->currentText() == "Blocking (All Slices)")
//		{
//			blockType = ImagePairManager::BLOCKING;
//		}
//		else if (drawType->currentText() == "Segmentation")
//		{
//			blockType = ImagePairManager::SEGMENTATION;
//		}


//		//Set corresponding pixels to the corresponding blocking type
//		for (int y = -yLim; y <= yLim; y++)
//		{
//			for (int x = -xLim; x <= xLim; x++)
//			{
//				//Check boundary condition
//				if (xVoxel + x < boundary[0] || xVoxel + x > boundary[1] || yVoxel + y < boundary[2] || yVoxel + y > boundary[3])
//				{
//					qDebug() << "DrawManager::drawAlgorithm->OUT_OF_BOUND_AT(" << xVoxel + x << "," << yVoxel + y << ")";
//				}
//				//Segmentation read-only mode is toggled
//				else if (segReadOnly->checkState() == 2)
//				{
//					{
//						//Check for blockType - whether to draw to single, particular slices, or all slices
//						if (drawType->currentText() == "Blocking (All Slices)")
//						{
//							for (int z = boundary[4]; z <= boundary[5]; z++)
//							{
//								unsigned char* voxel = static_cast<unsigned char*>(imagePairManager->segblock->GetScalarPointer(xVoxel + x, yVoxel + y, z));
//								if (*voxel == ImagePairManager::SEGMENTATION)
//								{
//									qDebug() << "DrawManager::drawAlgorithm->CANNOT_DRAWN_ON_SEGMENTATION(" << xVoxel + x << "," << yVoxel + y << ")";
//								}
//								else
//								{
//									*voxel = blockType;
//								}
//							}
//							qDebug() << "DrawManager::drawAlgorithm->DRAWN_AT(" << xVoxel + x << "," << yVoxel + y << ") for all slices";
//						}
//						else if (drawType->currentText() == "Blocking (Z Slices Range)")
//						{
//							for (int z = minZSlice->value(); z <= maxZSlice->value(); z++)
//							{
//								unsigned char* voxel = static_cast<unsigned char*>(imagePairManager->segblock->GetScalarPointer(xVoxel + x, yVoxel + y, z));
//								if (*voxel == ImagePairManager::SEGMENTATION)
//								{
//									qDebug() << "DrawManager::drawAlgorithm->CANNOT_DRAWN_ON_SEGMENTATION(" << xVoxel + x << "," << yVoxel + y << ")";
//								}
//								else
//								{
//									*voxel = blockType;
//								}
//							}
//							qDebug() << "DrawManager::drawAlgorithm->DRAWN_AT(" << xVoxel + x << "," << yVoxel + y << ") for the provided Z Slices range";
//						}
//						else
//						{
//							unsigned char* voxel = static_cast<unsigned char*>(imagePairManager->segblock->GetScalarPointer(xVoxel + x, yVoxel + y, zVoxel));
//							if (*voxel == ImagePairManager::SEGMENTATION)
//							{
//								qDebug() << "DrawManager::drawAlgorithm->CANNOT_DRAWN_ON_SEGMENTATION(" << xVoxel + x << "," << yVoxel + y << ")";
//							}
//							else
//							{
//								*voxel = blockType;
//							}
//							qDebug() << "DrawManager::drawAlgorithm->DRAWN_AT(" << xVoxel + x << "," << yVoxel + y << ") for single slices";
//						}
//					}
//				}
//				//Draw on anything
//				else
//				{
//					//Check for blockType - whether to draw to single, particular slices, or all slices
//					if (drawType->currentText() == "Blocking (All Slices)")
//					{
//						for (int z = boundary[4]; z <= boundary[5]; z++)
//						{
//							unsigned char* voxel = static_cast<unsigned char*>(imagePairManager->segblock->GetScalarPointer(xVoxel + x, yVoxel + y, z));
//							*voxel = blockType;
//						}
//						qDebug() << "DrawManager::drawAlgorithm->DRAWN_AT(" << xVoxel + x << "," << yVoxel + y << ") for all slices";
//					}
//					else if (drawType->currentText() == "Blocking (Z Slices Range)")
//					{
//						for (int z = minZSlice->value(); z <= maxZSlice->value(); z++)
//						{
//							unsigned char* voxel = static_cast<unsigned char*>(imagePairManager->segblock->GetScalarPointer(xVoxel + x, yVoxel + y, z));
//							*voxel = blockType;
//						}
//						qDebug() << "DrawManager::drawAlgorithm->DRAWN_AT(" << xVoxel + x << "," << yVoxel + y << ") for the provided Z Slices range";
//					}
//					else
//					{
//						unsigned char* voxel = static_cast<unsigned char*>(imagePairManager->segblock->GetScalarPointer(xVoxel + x, yVoxel + y, zVoxel));
//						*voxel = blockType;
//						qDebug() << "DrawManager::drawAlgorithm->DRAWN_AT(" << xVoxel + x << "," << yVoxel + y << ") for single slices";
//					}
//				}
//			}
//		}
//	}
//	else if (mode == DrawManager::ERASE)
//	{
//		//Set blocking to erase
//		blockType = ImagePairManager::BACKGROUND;

//		//Set corresponding pixels to the corresponding blocking type
//		for (int y = -yLim; y <= yLim; y++)
//		{
//			for (int x = -xLim; x <= xLim; x++)
//			{
//				//Check boundary condition
//				if (xVoxel + x < boundary[0] || xVoxel + x > boundary[1] || yVoxel + y < boundary[2] || yVoxel + y > boundary[3])
//				{
//					qDebug() << "DrawManager::drawAlgorithm->OUT_OF_BOUND_AT(" << xVoxel + x << "," << yVoxel + y << ")";
//				}
//				else
//				{
//					unsigned char* voxel = static_cast<unsigned char*>(imagePairManager->segblock->GetScalarPointer(xVoxel + x, yVoxel + y, zVoxel));
//					if (segReadOnly->checkState() == 2 && *voxel == ImagePairManager::SEGMENTATION)
//					{
//						qDebug() << "DrawManager::drawAlgorithm->CANNOT_DRAWN_ON_SEGMENTATION(" << xVoxel + x << "," << yVoxel + y << ")";
//					}
//					else
//					{
//						*voxel = blockType;
//						qDebug() << "DrawManager::drawAlgorithm->ERASE_AT(" << xVoxel + x << "," << yVoxel + y << ")";
//					}
//				}
//			}
//		}
//	}
}
