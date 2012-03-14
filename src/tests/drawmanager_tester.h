#include <QObject>
#include <QtTest/QtTest>
#include <QSpinBox>
#include <QComboBox>
#include "drawmanager.h"
#include "imagepairmanager.h"
#include "vtkStructuredPoints.h"

class DrawManagerTester : public QObject
{
	Q_OBJECT;

	private:
		DrawManager* drawManager;
		ImagePairManager* imagePairManager;
		QSpinBox* drawSize;
		QComboBox* drawType;
		vtkStructuredPoints* dummyImage;

	private slots:
		void initTestCase()
		{
			//Setup dummy drawSize and drawType UI
			drawSize = new QSpinBox();
			drawSize->setRange(1, 20);
			drawType = new QComboBox();
			drawType->addItem("Dummy Type");

			//Create a dummy VTK structure point image and load it to ImagePairManager
			imagePairManager = new ImagePairManager();
			dummyImage = vtkStructuredPoints::New();
			dummyImage->SetExtent(0, 100, 0, 100, 0, 100);
			imagePairManager->loadImageFromSource(dummyImage);

			//Initialise DrawManager
			drawManager = new DrawManager(imagePairManager, drawSize, drawType);
		}

		void testBrushSize()
		{
			//Set brush size and start drawing
			for (int size = 1; size <= 20; size++)
			{
				drawSize->setValue(size);
				drawType->setItemText(0, "Blocking (Current Slice)");
				drawManager->draw(50, 50, 50);

				for (int y = 50-size; y <= 50+size; y++)
				{
					for (int x = 50-size; x <= 50+size; x++)
					{
						char* pixel;
						pixel = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(x, y, 50));
						QCOMPARE(static_cast<int>(*pixel), 1);
					}
				}
			}
		}

		void testDrawBlocking()
		{
			//Draw Blocking
			drawSize->setValue(5);
			drawType->setItemText(0, "Blocking (Current Slice)");
			drawManager->draw(0, 50, 50);

			char* pixel;
			pixel = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(0, 50, 50));
			QCOMPARE(static_cast<int>(*pixel), 1);
		}

		void testDrawBlockingAllSlices()
		{
			//Draw Blocking for all slices
			drawSize->setValue(5);
			drawType->setItemText(0, "Blocking (All Slices)");
			drawManager->draw(50, 0, 50);

			char* pixel;

			for (int z = 0; z <= 100; z++)
			{
				pixel = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(50, 0, z));
				QCOMPARE(static_cast<int>(*pixel), 1);
			}
		}

		void testDrawSegmentation()
		{
			//Draw Segmentation
			drawSize->setValue(5);
			drawType->setItemText(0, "Segmentation");
			drawManager->draw(50, 50, 0);

			char* pixel;
			pixel = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(50, 50, 0));
			QCOMPARE(static_cast<int>(*pixel), 2);
		}

		void testErase()
		{
			//Erase
			drawSize->setValue(5);
			drawManager->erase(0, 50, 50);

			char* pixel;
			pixel = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(0, 50, 50));
			QCOMPARE(static_cast<int>(*pixel), 0);
		}

		void cleanupTestCase()
		{
			delete drawManager;
			delete imagePairManager;
		}

};
