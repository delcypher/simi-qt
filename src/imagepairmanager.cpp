#include "imagepairmanager.h"
#include <QDebug>
#include <QApplication>

ImagePairManager::ImagePairManager()
{
	reader = vtkStructuredPointsReader::New();
	readerSegBlock = vtkStructuredPointsReader::New();

	//Create our VtkStructured points to hold our image data. We will populate it later.
	original = vtkStructuredPoints::New();
	segblock = vtkStructuredPoints::New();
}


ImagePairManager::~ImagePairManager()
{
	reader->CloseVTKFile();
}

bool ImagePairManager::loadImage(QFileInfo image)
{
	//close file just incase we already had one open
	reader->CloseVTKFile();

	reader->SetFileName(image.absoluteFilePath().toAscii());
	reader->Update();

	if(!reader->IsFileStructuredPoints())
	{
		qDebug() << "File " << image.absoluteFilePath() << " is not a VTK structured points file!";
		return false;
	}

	//setup original image
	return loadImageFromSource(reader->GetOutput());
}




double ImagePairManager::getYSpacing()
{
    double spacing[3];
    original->GetSpacing(spacing);

    return spacing[1];
}

double ImagePairManager::getXSpacing()
{
    double spacing[3];
    original->GetSpacing(spacing);
    return spacing[0];
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

bool ImagePairManager::setAll(int slice, ImagePairManager::BlockType from, ImagePairManager::BlockType to)
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

    char* voxel=NULL;

    if( slice < getZMin() || slice > getZMax())
    {
        qWarning() << "Slice " << slice << " is not in the valid range [" << getZMin() << "," << getZMax() << "]";
        return false;
    }

    qDebug() << "ImagePairManager::setAll() on slice " << slice << ". Set from " << from << " to " << to;
    for(int x= extent[0]; x <= extent[1]; x++)
    {
	for(int y= extent[2]; y <= extent[3]; y++)
        {
            voxel = static_cast<char*>(segblock->GetScalarPointer(x,y,slice));

            if(voxel==0)
            {
                qWarning() << "Error getting pointer to voxel at (" << x << "," << "y" << "," << slice << ")";
                return false;
            }

            //check to see if voxel value is set to from
            if(*voxel== from)
            {
                *voxel = to;
            }
        }
    }

    segblock->Modified(); //Mark as modifed so if a re-render is done VTK knows that this part of the pipeline needs to be reprocessed
    return true; //success
}

bool ImagePairManager::setAllSimBlockVoxels(ImagePairManager::BlockType from, ImagePairManager::BlockType to)
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

    char* voxel=NULL;

     //loop over each voxel and set
    for(int z= extent[4]; z <= extent[5]; z++)
    {

        for(int y=extent[2]; y <= extent[3]; y++)
        {

            for(int x= extent[0]; x <= extent[1] ; x++)
            {
                //get pointer to voxel
                voxel = static_cast<char*>(segblock->GetScalarPointer(x,y,z));

		if(*voxel == from)
			*voxel = to;
            }
        }
	QApplication::processEvents();
    }

    segblock->Modified(); //Mark as modifed so if a re-render is done VTK knows that this part of the pipeline needs to be reprocessed
    return true;
}

bool ImagePairManager::saveSegblock(QString path)
{
	//Setup and connect the structured points writer with the segblock
	vtkStructuredPointsWriter* segblockWriter = vtkStructuredPointsWriter::New();
	segblockWriter->SetInput(segblock);

	//Pass in the file name and path
	const char* filename = path.toAscii();
	segblockWriter->SetFileName(filename);

	//Write out file
	int writeResult;
	writeResult = segblockWriter->Write();

	qDebug() << "ImagePairManager::saveSegblock(" << path << ")";

    segblockInitTime = segblock->GetMTime(); //update the init time
	return writeResult;
}

bool ImagePairManager::loadSegblock(QString path)
{
    // make sure that the file is already closed
    readerSegBlock->CloseVTKFile();

    readerSegBlock->SetFileName(path.toAscii());
    readerSegBlock->Update();

    if(!readerSegBlock->IsFileStructuredPoints())
    {
	    qDebug() << "File with path " << path.toAscii() << " is not a VTK structured points file!";
	    return false;
    }

    //setup the segmentation block
    segblock->DeepCopy(readerSegBlock->GetOutput());

    qDebug() << "ImagePairManager::loadSegblock(" << path << ")" ;

    segblockInitTime = segblock->GetMTime(); //update the init time
    return true;
}

bool ImagePairManager::segblockModified()
{
    if(segblock==0)
        return false;

    return (segblock->GetMTime() > segblockInitTime);
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

bool ImagePairManager::loadImageFromSource(vtkStructuredPoints *src)
{
	//setup original image from a source
	original->DeepCopy(src);

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

	/* Workaround:
	*  We do not know when we load an image where it thinks its (0,0,0) voxel should be (i.e. the origin property of the VtkStructuredPoints)
	*  could be different to what we expect. So we force it so that when start the (0,0,0) point in the world co-ordinate system will be at
	*  the centre of the image in the x and y planes (I don't know about the Z).
	*/
	original->SetOrigin( -(xDim/2.0)*spacing[0], -(yDim/2.0)*spacing[1],0 );

	//now setup block/segmentation image
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

	segblockInitTime = segblock->GetMTime(); //Record modification time which we'll use later to check if segblock has been modified.

	qDebug() << "segblock now occupies :" << segblock->GetActualMemorySize() << "KB";

	//should probably initialise now...
	debugDump();

	return true;
}


