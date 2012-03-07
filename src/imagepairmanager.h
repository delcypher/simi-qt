#ifndef IMAGEPAIRMANAGER_H
#define IMAGEPAIRMANAGER_H

#include <QObject>
#include <QFileInfo>
#include "vtkSmartPointer.h"
#include <vtkStructuredPoints.h>
#include "vtkStructuredPointsReader.h"
#include "vtkStructuredPointsWriter.h"


/* Responsible for loading original image and making it avaliable.
*  It is also responsible for producing the segmentation/blocking image
*/
class ImagePairManager : public QObject
{
	Q_OBJECT
	public:
		ImagePairManager();
		~ImagePairManager();
		bool loadImage(QFileInfo image);
		


		//Images
		vtkSmartPointer<vtkStructuredPoints> original;
		vtkSmartPointer<vtkStructuredPoints> segblock;

		int getXDim() {return xDim;}
		int getYDim() {return yDim;}
		int getZDim() {return zDim;}

		int getZMin() {return extentZMin;}
		int getZMax() {return extentZMax;}

		double getYSpacing();

		double getMinimumIntensity();
		double getMaximumIntensity();

		enum BlockType
		{
			BACKGROUND,
			BLOCKING,
			SEGMENTATION
		};

		//Set all Voxels in slice "slice" that are value "from" to the value "to
		bool setAll(int slice, ImagePairManager::BlockType from, ImagePairManager::BlockType to);

		bool setSimBlockVoxelsTo(ImagePairManager::BlockType type);

		bool saveSegblock(QString path);
		bool loadSegblock(QString path);

		bool segblockModified();

	private:
		vtkSmartPointer<vtkStructuredPointsReader> reader;

		//Widths in terms of voxels
		int xDim;
		int yDim;
		int zDim;

		//Extents
		int extentZMax;
		int extentZMin;

		void debugDump();

		unsigned long segblockInitTime;



};

#endif
