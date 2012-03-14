#include <QObject>
#include <QtTest/QtTest>
#include "segmenter.h"

class SegmenterTester : public QObject
{
	Q_OBJECT
        private:
                Segmenter* segmenter;
                SeedPointManager* seedPointManager;
                ImagePairManager* imagePairManager;
                QComboBox* box;


	private slots:
	void initTestCase()
	{
		//Intialise everything for test
                seedPointManager = new SeedPointManager(120);
                imagePairManager = new ImagePairManager();
                box = new QComboBox();

                segmenter = new Segmenter(seedPointManager, imagePairManager, box);
	}

	void testNoSeedPoint()
	{
//		int x=0;
//		int y=0;

//		//check we get false back
//		QVERIFY(!seedPointManager->getSeedPoint(0,x,y));

//		QCOMPARE(x,-1);
//		QCOMPARE(y,-1);
	}

	void cleanupTestCase()
	{
		delete seedPointManager;
                delete imagePairManager;
                delete box;
                delete segmenter;
        }

};
