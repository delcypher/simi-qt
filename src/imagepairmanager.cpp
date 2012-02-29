#include "imagepairmanager.h"
#include <QDebug>

ImagePairManager::ImagePairManager()
{
	reader = vtkStructuredPointsReader::New();
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

	qDebug() << "Image opened with widths x:" << xDim << ", y:" << yDim << ", z:" << zDim ;

	//now setup block/segmentation image
	segblock = vtkImageData::New();
	segblock->SetExtent(original->GetExtent());
	segblock->SetSpacing(original->GetSpacing());
	segblock->SetNumberOfScalarComponents(1);
	segblock->SetScalarTypeToChar();
	segblock->AllocateScalars();

	//Make sure everything is consistent as we have changed the dimensions.
	segblock->Update();

	qDebug() << "segblock now occupies :" << segblock->GetActualMemorySize() << "KB";

	//should probably initialise now...


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


