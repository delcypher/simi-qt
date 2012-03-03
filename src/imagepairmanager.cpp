#include "imagepairmanager.h"
#include <QDebug>


ImagePairManager::ImagePairManager()
{
	reader = vtkStructuredPointsReader::New();

	//set other pointers to NULL
	original = NULL;
	segblock =  NULL;
}


ImagePairManager::~ImagePairManager()
{

}

bool ImagePairManager::loadImage(QFileInfo image)
{
	reader->SetFileName(image.absoluteFilePath().toAscii());
	reader->Update();

	if(!reader->IsFileStructuredPoints())
	{
		qDebug() << "File " << image.absoluteFilePath() << " is not a VTK structured points file!";
		return false;
	}

	//setup original image
	original = reader->GetOutput();

	//determine the image dimensions in terms of voxels
	int dimensions[3];
	original->GetDimensions(dimensions);
	xDim = dimensions[0];
	yDim = dimensions[1];
	zDim = dimensions[2];

	//determine extents
	int extent[6];
	original->GetExtent(extent);
	extentZMin = extent[4];
	extentZMax = extent[5];

	//determine spacing
	double spacing[3];
	original->GetSpacing(spacing);
	qDebug() << "Image opened with widths x:" << xDim << ", y:" << yDim << ", z:" << zDim ;
	qDebug() << "Spacing: " << spacing[0] << "," << spacing[1] << "," << spacing[2];


	//now setup block/segmentation image
	segblock = vtkStructuredPoints::New();
	segblock->SetExtent(original->GetExtent());
	segblock->SetSpacing(original->GetSpacing());
	segblock->SetNumberOfScalarComponents(1);
	segblock->SetOrigin(original->GetOrigin());
	segblock->SetScalarTypeToChar();
	segblock->AllocateScalars();

	//Make sure everything is consistent as we have changed the dimensions.
	segblock->Update();
	//Initialise segblock
	//setSimBlockVoxelsTo(BACKGROUND);

	qDebug() << "segblock now occupies :" << segblock->GetActualMemorySize() << "KB";

	//should probably initialise now...
    debugDump();

	return true;
}


void ImagePairManager::resetSegmentation(int slice)
{

}

void ImagePairManager::resetBlocking(int slice)
{

}

double ImagePairManager::getYSpacing()
{
    double spacing[3];
    original->GetSpacing(spacing);

    return spacing[1];
}

double ImagePairManager::getMinimumIntensity()
{
	double range[2];
	original->GetScalarRange(range);
	return range[0];
}

double ImagePairManager::getMaximumIntensity()
{
	double range[2];
	original->GetScalarRange(range);
    return range[1];
}

bool ImagePairManager::setSimBlockVoxelsTo(ImagePairManager::BlockType type)
{
    if(segblock==NULL)
    {
        qDebug() << "segblock does NOT exist!";
        return false;
    }

    //get ranges to loop over
    /*
    * [0] : x_min
    * [1] : x_max
    * [2] : y_min
    * [3] : y_max
    * [4] : z_min
    * [5] : z_max
    */
    int extent[6];
    segblock->GetExtent(extent);

    short* voxel=NULL;

     //loop over each voxel and set
    for(int z= extent[4]; z <= extent[5]; z++)
    {

        for(int y=extent[2]; y <= extent[3]; y++)
        {

            for(int x= extent[0]; x <= extent[1] ; x++)
            {
                //get pointer to voxel
                voxel = static_cast<short*>(segblock->GetScalarPointer(x,y,z));

                *voxel = type;
            }
        }

    }
    return true;
}

void ImagePairManager::debugDump()
{
    double origin[3];

    vtkStructuredPoints* img;

    for(int i=0; i<2; i++)
    {
        img = (i==0)?original:segblock;
        img->GetOrigin(origin);
        qDebug() << "origin of " << ((i==0)?"original":"segblock") << " " << origin[0] << "," << origin[1] << origin[2];


    }
}


