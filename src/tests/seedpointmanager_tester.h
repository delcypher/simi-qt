#include <QObject>
#include <QtTest/QtTest>
#include "seedpointmanager.h"

class SeedPointManagerTester : public QObject
{
	Q_OBJECT
	private:
		SeedPointManager* seedPointManager;

	private slots:
	void initTestCase()
	{
		//Intialise everything for test
		seedPointManager = new SeedPointManager(50);
	}

	void testNoSeedPoint()
	{
		int x=0;
		int y=0;

		//check we get false back
		QVERIFY(!seedPointManager->getSeedPoint(0,x,y));

		QCOMPARE(x,-1);
		QCOMPARE(y,-1);
	}

	void testIncorrectNumberSlice()
	{
		int x=0;
		int y=0;

		// check we get false back
		QVERIFY(!seedPointManager->getSeedPoint(51,x,y));
	}

	void testAssignmentOfSeedPoint()
	{
		int x=0;
		int y=0;

		for (int count=0; count < seedPointManager->numberOfSlices; count++)
		{
			seedPointManager->setSeedPoint(count,count,count);
		}

		for (int count2=0; count2 < seedPointManager->numberOfSlices; count2++)
		{
			seedPointManager->getSeedPoint(count2,x,y);

			QCOMPARE(x,count2);
			QCOMPARE(y,count2);
		}
	}

	void cleanupTestCase()
	{
		delete seedPointManager;
	}

};
