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
	int extent[6];
	original->GetExtent(extent);
	xWidth = extent[1] - extent[0] +1;
	yWidth = extent[3] - extent[2] +1;
	zWidth = extent[5] - extent[4] +1;

	qDebug() << "Image opened with widths x:" << xWidth << ", y:" << yWidth << ", z:" << zWidth ;

	//now setup block/segmentation image
	segblock = vtkImageData::New();
	segblock->SetExtent(extent);
	segblock->SetSpacing(original->GetSpacing());
	segblock->SetScalarTypeToChar();

	//should probably initialise now...


	return true;
}


void ImagePairManager::resetSegmentation(int slice)
{

}

void ImagePairManager::resetBlocking(int slice)
{

}


