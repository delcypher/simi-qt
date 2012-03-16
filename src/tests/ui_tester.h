#include <QObject>
#include <QtTest/QtTest>
#include <QtGui>
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

	/* Check minimum segmentation intensity slider and spin box in sync*/
	void synchronisedMinimumSegIntensity()
	{
		mw->ui->minSegIntensitySlider->setValue(0);
		mw->ui->maxSegIntensitySlider->setValue(10);

		mw->ui->minSegIntensitySlider->setValue(5);
		QVERIFY(mw->ui->minSegIntensitySlider->value() == mw->ui->minSegIntensitySpinBox->value());
	}

	/* Check maximum segmentation intensity slider and spin box in sync*/
	void synchronisedMaxmiumSegIntensity()
	{
		mw->ui->minSegIntensitySlider->setValue(0);
		mw->ui->maxSegIntensitySlider->setValue(10);

		mw->ui->maxSegIntensitySlider->setValue(20);
		QVERIFY(mw->ui->maxSegIntensitySlider->value() == mw->ui->maxSegIntensitySpinBox->value());
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

	/* Check that minimum intensity segmentation slider can't be made > maximum segmation intensity slider */
	void consistentMinimumSegSlider()
	{
		mw->ui->minSegIntensitySlider->setValue(0);
		mw->ui->maxSegIntensitySlider->setValue(5);

		mw->ui->minSegIntensitySlider->setValue( mw->ui->maxSegIntensitySlider->value() + 5);

		QVERIFY(mw->ui->minSegIntensitySlider->value() <= mw->ui->maxSegIntensitySlider->value());
	}

	/* Check that maximum intensity segmentation slider can't be made < maximum segmentation intensity slider */
	void consistentMaximumSegSlider()
	{
		mw->ui->maxSegIntensitySlider->setValue(15);
		mw->ui->minSegIntensitySlider->setValue(10);

		mw->ui->maxSegIntensitySlider->setValue( mw->ui->minSegIntensitySlider->value() + 6);

		QVERIFY(mw->ui->maxSegIntensitySlider->value() >= mw->ui->minSegIntensitySlider->value());
	}

    void tryToCrash()
    {
        //Image isn't loaded yet so see if we can crash it
		mw->on_actionSlice_up_triggered();
		mw->on_actionSlice_down_triggered();


		mw->on_minIntensitySlider_valueChanged(50);
		mw->on_maxIntensitySlider_valueChanged(20);
		mw->on_sliceSlider_valueChanged(2);
		mw->updateStatusBar();
		mw->seedPointChanged();
		mw->on_actionClear_Segmentation_on_All_Slices_triggered();
		mw->on_actionClear_Blocking_on_All_Slices_triggered();

		//toolbar slots
		mw->on_actionHandTool_triggered();
		mw->on_actionPenTool_triggered();
		mw->on_actionCrosshairTool_triggered();
		mw->on_actionEraseTool_triggered();

		mw->on_minSegIntensitySlider_valueChanged(20);
		mw->on_maxSegIntensitySlider_valueChanged(50);

		mw->on_doSegmentation2D_clicked();
		mw->on_doSegmentation3D_clicked();

		mw->on_actionClear_Drawing_triggered();
		mw->on_actionClear_Segmentation_triggered();

		mw->on_actionLoad_Segmentation_triggered();
		mw->on_actionSave_Segmentation_triggered();

		mw->tryEnableSegmentationWidgets();
		mw->enableSegmentationWidgets();
		mw->disableSegmentationWidgets();

		mw->on_do3Drender_clicked();
		mw->on_doDilate2D_clicked();
		mw->on_doErode2D_clicked();
		mw->on_doClose2D_clicked();
		mw->on_doOpen2D_clicked();

		mw->on_actionRotate_view_by_180_toggled(true);
		mw->on_actionResetView_triggered();

		//mw->on_actionOpen_Image_triggered();
    }

	void cleanupTestCase()
	{ 
		delete mw;
	}
};
