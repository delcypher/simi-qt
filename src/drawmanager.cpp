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
    DrawMode mode = static_cast<DrawManager::DrawMode>(drawType->currentIndex());
    drawAlgorithm(xVoxel, yVoxel, zVoxel, mode);

	//Inform other classes that we're done (so they can update)
	imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
	emit drawingDone();
}

void DrawManager::erase(int xVoxel, int yVoxel, int zVoxel)
{
	// Set draw mode and call draw algorithm function
    drawAlgorithm(xVoxel, yVoxel, zVoxel, ERASE_SINGLE_SLICE);

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

void DrawManager::drawAlgorithm(int &xVoxel, int &yVoxel, int &zVoxel, DrawManager::DrawMode mode)
{
    int abscissaMin=0;
    int abscissaMax=0;
    int ordinateMin=0;
    int ordinateMax=0;
    int depthMin=0;
    int depthMax=0;
    unsigned char* voxel=0;

    //this determines how many voxels either side of the clicked voxel are drawn to make
    // a square (or cube)
    int numVoxels= drawSize->value() -1;

    vtkStructuredPoints* segblock = imagePairManager->segblock;

    bool segmentationReadOnly = segReadOnly->isChecked();

    //determine block type to draw
    ImagePairManager::BlockType block;
    switch(mode)
    {
        case BLOCKING_SINGLE_SLICE:
        case BLOCKING_MULTIPLE_SLICES:
            block=ImagePairManager::BLOCKING;
        break;
        case SEGMENTATION_SINGLE_SLICE:
            block=ImagePairManager::SEGMENTATION;
        break;
        case ERASE_SINGLE_SLICE:
            block=ImagePairManager::BACKGROUND;
        break;
        default:
            qWarning() << "Drawing mode not supported";
            return;
    }

    //use orientation to determine limits
    switch(orientation)
    {
        case vtkImageViewer2::SLICE_ORIENTATION_XY:
            abscissaMin= xVoxel - numVoxels;
            abscissaMax= xVoxel + numVoxels;
            ordinateMin= yVoxel - numVoxels;
            ordinateMax= yVoxel + numVoxels;

            if(mode==BLOCKING_MULTIPLE_SLICES)
            {
                depthMin=boundaryManager->getZMin();
                depthMax=boundaryManager->getZMax();

                if(zVoxel > depthMax || zVoxel < depthMin)
                    return; //Can't draw because out of range
            }
            else
            {
                //We are drawing on a single slice

                if(!boundaryManager->isInZRange(zVoxel))
                    return; //can't draw because out of range
                depthMin=zVoxel;
                depthMax=zVoxel;
            }

            //make corrections if we're on the edge of boundary
            if(!boundaryManager->isInXRange(abscissaMin))
                abscissaMin=boundaryManager->getXMin();

            if(!boundaryManager->isInXRange(abscissaMax))
                abscissaMax=boundaryManager->getXMax();

            if(!boundaryManager->isInYRange(ordinateMin))
                ordinateMin=boundaryManager->getYMin();

            if(!boundaryManager->isInYRange(ordinateMax))
                ordinateMax=boundaryManager->getYMax();



            //loop over relavant voxels
            for(int abscissa=abscissaMin; abscissa <= abscissaMax; abscissa++)
            {
                for(int ordinate=ordinateMin; ordinate <= ordinateMax; ordinate++)
                {
                    for(int depth=depthMin; depth <= depthMax; depth++)
                    {
                        voxel = static_cast<unsigned char*>(segblock->GetScalarPointer(abscissa,ordinate,depth));

                        if(segmentationReadOnly && *voxel == ImagePairManager::SEGMENTATION)
                            break;//do not modify

                        *voxel = block;
                    }
                }
            }

        break;

        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
            abscissaMin= xVoxel - numVoxels;
            abscissaMax= xVoxel + numVoxels;
            ordinateMin= zVoxel - numVoxels;
            ordinateMax= zVoxel + numVoxels;

            if(mode==BLOCKING_MULTIPLE_SLICES)
            {
                depthMin=boundaryManager->getYMin();
                depthMax=boundaryManager->getYMax();

                if(yVoxel > depthMax || yVoxel < depthMin)
                    return; //Can't draw because out of range
            }
            else
            {
                //We are drawing on a single slice

                if(!boundaryManager->isInYRange(yVoxel))
                    return; //can't draw because out of range
                depthMin=yVoxel;
                depthMax=yVoxel;
            }

            //make corrections if we're on the edge of boundary
            if(!boundaryManager->isInXRange(abscissaMin))
                abscissaMin=boundaryManager->getXMin();

            if(!boundaryManager->isInXRange(abscissaMax))
                abscissaMax=boundaryManager->getXMax();

            if(!boundaryManager->isInZRange(ordinateMin))
                ordinateMin=boundaryManager->getZMin();

            if(!boundaryManager->isInZRange(ordinateMax))
                ordinateMax=boundaryManager->getZMax();

            //loop over relavant voxels
            for(int abscissa=abscissaMin; abscissa <= abscissaMax; abscissa++)
            {
                for(int ordinate=ordinateMin; ordinate <= ordinateMax; ordinate++)
                {
                    for(int depth=depthMin; depth <= depthMax; depth++)
                    {
                        voxel = static_cast<unsigned char*>(segblock->GetScalarPointer(abscissa,depth,ordinate));

                        if(segmentationReadOnly && *voxel == ImagePairManager::SEGMENTATION)
                            break;//do not modify

                        *voxel = block;
                    }
                }
            }

        break;

        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
            abscissaMin= yVoxel - numVoxels;
            abscissaMax= yVoxel + numVoxels;
            ordinateMin= zVoxel - numVoxels;
            ordinateMax= zVoxel + numVoxels;

            if(mode==BLOCKING_MULTIPLE_SLICES)
            {
                depthMin=boundaryManager->getXMin();
                depthMax=boundaryManager->getXMax();

                if(xVoxel > depthMax || xVoxel < depthMin)
                    return; //Can't draw because out of range
            }
            else
            {
                //We are drawing on a single slice

                if(!boundaryManager->isInXRange(xVoxel))
                    return; //can't draw because out of range
                depthMin=xVoxel;
                depthMax=xVoxel;
            }

            //make corrections if we're on the edge of boundary
            if(!boundaryManager->isInYRange(abscissaMin))
                abscissaMin=boundaryManager->getYMin();

            if(!boundaryManager->isInYRange(abscissaMax))
                abscissaMax=boundaryManager->getYMax();

            if(!boundaryManager->isInZRange(ordinateMin))
                ordinateMin=boundaryManager->getZMin();

            if(!boundaryManager->isInZRange(ordinateMax))
                ordinateMax=boundaryManager->getZMax();

            //loop over relavant voxels
            for(int abscissa=abscissaMin; abscissa <= abscissaMax; abscissa++)
            {
                for(int ordinate=ordinateMin; ordinate <= ordinateMax; ordinate++)
                {
                    for(int depth=depthMin; depth <= depthMax; depth++)
                    {
                        voxel = static_cast<unsigned char*>(segblock->GetScalarPointer(depth,abscissa,ordinate));

                        if(segmentationReadOnly && *voxel == ImagePairManager::SEGMENTATION)
                            break;//do not modify

                        *voxel = block;
                    }
                }
            }

        break;

        default:
            qWarning() << "Orientation not supported!";
            return;
    }

}
