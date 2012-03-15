#include <QObject>
#include <QtTest/QtTest>
#include "imagepairmanager.h"

#include "vtkSmartPointer.h"
#include "vtkStructuredPoints.h"

const int sizeX=100;
const int sizeY=100;
const int sizeZ=100;

const double originX=2;
const double originY=3;
const double originZ=5;

const double spacingX=1.2;
const double spacingY=2.5;
const double spacingZ=1.7;

class ImagePairManagerTester : public QObject
{
	Q_OBJECT
	private:
		ImagePairManager* ipm2;
		ImagePairManager* imagePairManager;
		vtkSmartPointer<vtkStructuredPoints> source;

	public:


	private slots:
	void initTestCase()
	{
		//Intialise everything for test
		imagePairManager = new ImagePairManager();

		source = vtkStructuredPoints::New();
		source->SetDimensions(sizeX,sizeY,sizeZ);

		source->SetOrigin(originX,originY,originZ);

		source->SetSpacing(spacingX,spacingY,spacingZ);
		source->SetScalarTypeToShort();

		QVERIFY(imagePairManager->loadImageFromSource(source));
		
	}

	void checkModified()
	{
		QVERIFY(! imagePairManager->segblockModified());

		imagePairManager->segblock->Modified();

		QVERIFY( imagePairManager->segblockModified());
	}

	void checkSegBlock()
	{
		double spacing[3];

		imagePairManager->segblock->GetSpacing(spacing);
		QCOMPARE(spacing[0], spacingX);
		QCOMPARE(spacingX, imagePairManager->getXSpacing());

		QCOMPARE(spacing[1], spacingY);
		QCOMPARE(spacingY, imagePairManager->getYSpacing());

		QCOMPARE(spacing[2], spacingZ);

		int dim[3];
		imagePairManager->segblock->GetDimensions(dim);

		QCOMPARE(sizeX, dim[0]);
		QCOMPARE(sizeX, imagePairManager->getXDim());
		QCOMPARE(sizeY, dim[1]);
		QCOMPARE(sizeY, imagePairManager->getYDim());
		QCOMPARE(sizeZ, dim[2]);
		QCOMPARE(sizeZ, imagePairManager->getZDim());

	}

	void checkMatchingOrigins()
	{
		double oOrigin[3];
		double sbOrigin[3];

		imagePairManager->original->GetOrigin(oOrigin);
		imagePairManager->segblock->GetOrigin(sbOrigin);

		QCOMPARE(oOrigin[0], sbOrigin[0]);
		QCOMPARE(oOrigin[1], sbOrigin[1]);
		QCOMPARE(oOrigin[2], sbOrigin[2]);
	}

	void allBackground()
	{
		int extent[6];
		imagePairManager->segblock->GetExtent(extent);

		for(int x=extent[0]; x <= extent[1]; x++)
		{
			for(int y=extent[2]; y <= extent[3]; y++)
			{
				for(int z=extent[4]; z <= extent[5]; z++)
				{
					char* voxel = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(x,y,z));

					QVERIFY(voxel != NULL);

					QVERIFY(*voxel == ImagePairManager::BACKGROUND);
				}
			}
		}
	}

	void setSingleSliceBlock()
	{
		int extent[6];
		imagePairManager->segblock->GetExtent(extent);

		imagePairManager->setAll(0,ImagePairManager::BACKGROUND, ImagePairManager::BLOCKING);

		for(int x=extent[0]; x <= extent[1]; x++)
		{
			for(int y=extent[2]; y <= extent[3]; y++)
			{
				char* voxel = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(x,y,0));

				QVERIFY(voxel != NULL);

				QVERIFY(*voxel == ImagePairManager::BLOCKING);
			}
		}
	}

	void setAllSliceSeg()
	{
		int extent[6];
		imagePairManager->segblock->GetExtent(extent);

		imagePairManager->setAllSimBlockVoxels(ImagePairManager::BACKGROUND, ImagePairManager::SEGMENTATION);
		imagePairManager->setAllSimBlockVoxels(ImagePairManager::BLOCKING, ImagePairManager::SEGMENTATION);

		for(int x=extent[0]; x <= extent[1]; x++)
		{
			for(int y=extent[2]; y <= extent[3]; y++)
			{
				for(int z=extent[4]; z <= extent[5]; z++)
				{
					char* voxel = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(x,y,z));

					QVERIFY(voxel != NULL);

					QVERIFY(*voxel == ImagePairManager::SEGMENTATION);
				}
			}
		}
	}


	void checkMinimumIntensity()
	{
		int extent[6];
		imagePairManager->segblock->GetExtent(extent);

		double minIntensity= imagePairManager->getMinimumIntensity();

		for(int x=extent[0]; x <= extent[1]; x++)
		{
			for(int y=extent[2]; y <= extent[3]; y++)
			{
				for(int z=extent[4]; z <= extent[5]; z++)
				{
					short* voxel = static_cast<short*>(imagePairManager->segblock->GetScalarPointer(x,y,z));

					QVERIFY(voxel != NULL);

					QVERIFY(minIntensity <= *voxel);
				}
			}
		}
	}

	void checkMaximumIntensity()
	{
		int extent[6];
		imagePairManager->segblock->GetExtent(extent);

		short maxIntensity= imagePairManager->getMaximumIntensity();

		for(int x=extent[0]; x <= extent[1]; x++)
		{
			for(int y=extent[2]; y <= extent[3]; y++)
			{
				for(int z=extent[4]; z <= extent[5]; z++)
				{
					short* voxel = static_cast<short*>(imagePairManager->original->GetScalarPointer(x,y,z));

					QVERIFY(voxel != NULL);

					QVERIFY(maxIntensity >= *voxel);
				}
			}
		}
	}

	void save()
	{
		QVERIFY(imagePairManager->saveSegblock( QString("test") ));
	}

	void load()
	{
		ipm2 = new ImagePairManager();
		
		source = vtkStructuredPoints::New();
		source->SetDimensions(sizeX,sizeY,sizeZ);

		source->SetOrigin(originX,originY,originZ);

		source->SetSpacing(spacingX,spacingY,spacingZ);
		source->SetScalarTypeToShort();
		QVERIFY(ipm2->loadImageFromSource(source));

		QVERIFY(ipm2->loadSegblock( QString("test.vtk") ));

		//check matching dims
		double spacing[3];

		ipm2->segblock->GetSpacing(spacing);
		QCOMPARE(spacing[0], spacingX);
		QCOMPARE(spacingX, ipm2->getXSpacing());

		QCOMPARE(spacing[1], spacingY);
		QCOMPARE(spacingY, ipm2->getYSpacing());

		QCOMPARE(spacing[2], spacingZ);

		int dim[3];
		ipm2->segblock->GetDimensions(dim);

		QCOMPARE(sizeX, dim[0]);
		QCOMPARE(sizeX, ipm2->getXDim());
		QCOMPARE(sizeY, dim[1]);
		QCOMPARE(sizeY, ipm2->getYDim());
		QCOMPARE(sizeZ, dim[2]);
		QCOMPARE(sizeZ, ipm2->getZDim());

		//Compare every voxel
		int extent[6];
		imagePairManager->segblock->GetExtent(extent);

		for(int x=extent[0]; x <= extent[1]; x++)
		{
			for(int y=extent[2]; y <= extent[3]; y++)
			{
				for(int z=extent[4]; z <= extent[5]; z++)
				{
					short* voxelOrig = static_cast<short*>(imagePairManager->original->GetScalarPointer(x,y,z));
					short* voxelNew = static_cast<short*>(ipm2->original->GetScalarPointer(x,y,z));

					QVERIFY(voxelOrig != NULL);
					QVERIFY(voxelNew !=NULL);

					QVERIFY( *voxelOrig == *voxelNew);
				}
			}
		}

	}

	void loadFromCreatedFile()
	{
		QFileInfo file("test.vtk");
		QVERIFY(file.exists());
		
		QVERIFY(imagePairManager->loadImage(file));	
	}

	void cleanupTestCase()
	{
		delete imagePairManager;
		delete ipm2;
	}

};
