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

	/* Check minimum intensity slider and spin box in sync*/
	void synchronisedMinimumIntensity()
	{
		mw->ui->minIntensitySlider->setValue(0);
		mw->ui->maxIntensitySlider->setValue(10);

		mw->ui->minIntensitySlider->setValue(5);
		QVERIFY(mw->ui->minIntensitySlider->value() == mw->ui->minIntensitySpinBox->value());
	}

	/* Check maximum intensity slider and spin box in sync*/
	void synchronisedMaxmiumIntensity()
	{
		mw->ui->minIntensitySlider->setValue(0);
		mw->ui->maxIntensitySlider->setValue(10);

		mw->ui->maxIntensitySlider->setValue(20);
		QVERIFY(mw->ui->maxIntensitySlider->value() == mw->ui->maxIntensitySpinBox->value());
	}

	/* Check that minimum intensity slider can't be made > maximum intensity slider */
	void consistentMinimumSlider()
	{ 
		mw->ui->minIntensitySlider->setValue(0);
		mw->ui->maxIntensitySlider->setValue(5);


		mw->ui->minIntensitySlider->setValue( mw->ui->maxIntensitySlider->value() + 5);

		QVERIFY(mw->ui->minIntensitySlider->value() <= mw->ui->maxIntensitySlider->value());
	}

	/* Check that maximum intensity slider can't be made < maximum intensity slider */
	void consistentMaximumSlider()
	{
		mw->ui->maxIntensitySlider->setValue(15);
		mw->ui->minIntensitySlider->setValue(10);

		mw->ui->maxIntensitySlider->setValue( mw->ui->minIntensitySlider->value() + 6);

		QVERIFY(mw->ui->maxIntensitySlider->value() >= mw->ui->minIntensitySlider->value());
	}

	void cleanupTestCase()
	{ 
		delete mw;
	}
};
