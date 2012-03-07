#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H 1

#include <QMainWindow>
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QActionGroup>
#include <QCloseEvent>
#include <assert.h>
#include "ui_mainwindow.h"

#include "imagepairmanager.h"
#include "seedpointmanager.h"
#include "viewmanager.h"
#include "drawmanager.h"
#include "segmenter.h"
#include "volumerendermanager.h"




//Forward Declare UiTester
class UiTester;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();
		~MainWindow();

		//helper classes
		ImagePairManager* imagePairManager;
		SeedPointManager* seedPointManager;
		ViewManager* viewManager;
		DrawManager* drawManager;
		Segmenter* segmenter;
		VolumeRenderManager* volumeRenderManager;


	private slots:
		void on_actionOpen_Image_triggered();
		void on_actionSlice_up_triggered();
		void on_actionSlice_down_triggered();
		void on_actionAbout_triggered();

		void on_minIntensitySlider_valueChanged(int value);
		void on_maxIntensitySlider_valueChanged(int value);
		void on_sliceSlider_valueChanged(int value);
		void updateStatusBar();
		void seedPointChanged();
		void on_actionClear_Segmentation_on_All_Slices_triggered();
		void on_actionClear_Blocking_on_All_Slices_triggered();

        //toolbar slots
        void on_actionHandTool_triggered();
        void on_actionPenTool_triggered();
        void on_actionCrosshairTool_triggered();
        void on_actionEraseTool_triggered();

        void on_minSegIntensitySlider_valueChanged(int value);
        void on_maxSegIntensitySlider_valueChanged(int value);

        void on_doSegmentation2D_clicked();
        void on_doSegmentation3D_clicked();

        void on_actionClear_Drawing_triggered();
        void on_actionClear_Segmentation_triggered();

        void on_actionLoad_Segmentation_triggered();
	bool on_actionSave_Segmentation_triggered();

        void tryEnableSegmentationWidgets();

	void on_do3Drender_clicked();


        void on_doDilate2D_clicked();

protected:
	  void closeEvent(QCloseEvent* close);

	private:

	  void on_do3Drendering_clicked();

private:
		QFileInfo imageInfo;
		QDir workPath; //Directory used file open dialogs
		Ui::MainWindow* ui; //handle to user interface
		QActionGroup* toolbarActions;



		//setup methods
		void contrastControlSetup();
		void sliceControlSetup();
		void toolbarSetup();
		void segmentationControlSetup();


		void changeContrast();

		//prompt the user to save save segblock if necessary
		bool okToContinue();

		//Friend class for UI testing.
		friend class UiTester;
};

#endif
