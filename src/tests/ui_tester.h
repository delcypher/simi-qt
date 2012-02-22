#include <QObject>
#include <QtTest/QtTest>
#include "mainwindow.h"

class UiTester: public QObject
{
	Q_OBJECT
	private:
		MainWindow* mw;
	private slots:
	void initTestCase()
	{ mw = new MainWindow(); mw->show(); }

	/* Check that minimum intensity slider can't be made > maximum intensity slider */
	void consistentMinimumSlider()
	{ 
		
	}

	void mySecondTest()
	{ QVERIFY(1 != 2); }

	void cleanupTestCase()
	{ 
		delete mw;
	}
};
