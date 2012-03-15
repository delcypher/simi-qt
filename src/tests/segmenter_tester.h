#include <QObject>
#include <QtTest/QtTest>
#include "segmenter.h"
#include <ctime>
#include <cstdlib>

class SegmenterTester : public QObject
{
	Q_OBJECT
        private:
                Segmenter* segmenter;
                SeedPointManager* seedPointManager;
                ImagePairManager* imagePairManager;
                QComboBox* box;
                vtkSmartPointer<vtkStructuredPoints> testData;

	private slots:
	void initTestCase()
	{
		//Intialise everything for test
                seedPointManager = new SeedPointManager(120);
                imagePairManager = new ImagePairManager();
                box = new QComboBox();
                box->addItem("Cross");
                box->addItem("Square");

                //setup 3D structure
                testData = vtkStructuredPoints::New();
                testData->SetDimensions(64, 64, 32);
                testData->SetScalarTypeToShort();
                testData->SetNumberOfScalarComponents(1);
                testData->AllocateScalars();

                //fill the structure with random values
                srand(time(NULL));
                for (int i=0; i<64; i++)
                {
                        for (int j=0; j<64; j++)
                        {
                                for (int k=0; k<32; k++)
                                {
                                        short* pixel_original = static_cast<short*>(testData->GetScalarPointer(i, j, k));
                                        short rv = rand() % 3600;
                                        pixel_original[0] = rv;
                                }
                        }
                }

                //mark entry point
                short* pixel_original = static_cast<short*>(testData->GetScalarPointer(31, 31, 15));
                pixel_original[0] = 1800;

                //load the synthetic image
                imagePairManager->loadImageFromSource(testData);

                //initialise segmenter
                segmenter = new Segmenter(seedPointManager, imagePairManager, box);
	}

        void test2DSegmentation()
        {
                segmenter->doSegmentation2D(31, 31, 15, 1800, 3600);
	}

        void test3DSegmentation()
        {
                segmenter->doSegmentation3D(31, 31, 15, 1800, 3600, 0, 16);
        }

        void testErodeCross()
        {
                segmenter->kernelType->setCurrentIndex(0);
                segmenter->doErode(15);
        }

        void testDilateCross()
        {
                segmenter->kernelType->setCurrentIndex(0);
                segmenter->doDilate(15);
        }

        void testErodeSquare()
        {
                segmenter->kernelType->setCurrentIndex(1);
                segmenter->doErode(15);
        }

        void testDilateSquare()
        {
                segmenter->kernelType->setCurrentIndex(1);
                segmenter->doDilate(15);
        }

	void cleanupTestCase()
	{
		delete seedPointManager;
                delete imagePairManager;
                delete box;
                delete segmenter;
        }
};
