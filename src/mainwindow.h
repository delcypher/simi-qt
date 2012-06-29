/*! \file mainwindow.h */
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H 1

#include <QMainWindow>
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QActionGroup>
#include <QCloseEvent>
#include <QProgressDialog>
#include <assert.h>
#include "ui_mainwindow.h"

#include "imagepairmanager.h"
#include "viewmanager.h"
#include "multiviewmanager.h"
#include "drawmanager.h"
#include "segmenter.h"
#include "volumerendermanager.h"

//Include Main page for Doxygen
#include "doxygenmain.h"


//Forward Declare UiTester
class UiTester;

//! The Main user interface window

/*! This class provides the main user interface and creates the others classes and links them together appropriately.
*
*/
class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:

        //! Constructor
		MainWindow();

		//! Destructor
		~MainWindow();

	private slots:
		void on_actionOpen_Image_triggered();
		void on_actionSlice_up_triggered();
		void on_actionSlice_down_triggered();
		void on_actionAbout_triggered();

		void on_actionZoom_in_triggered();
		void on_actionZoom_out_triggered();

		void on_minIntensitySlider_valueChanged(int value);
		void on_maxIntensitySlider_valueChanged(int value);
		void updateStatusBar(ViewManager* av);
		void seedPointChanged();
		void on_actionClear_Segmentation_on_All_Slices_triggered();
		void on_actionClear_Blocking_on_All_Slices_triggered();

		//toolbar slots
        void on_actionHandTool_toggled(bool t);
        void on_actionPenTool_toggled(bool t);
        void on_actionCrosshairTool_toggled(bool t);
        void on_actionEraseTool_toggled(bool t);

		void on_minSegIntensitySlider_valueChanged(int value);
		void on_maxSegIntensitySlider_valueChanged(int value);

		void on_doSegmentation2D_clicked();
		void on_doSegmentation3D_clicked();

		void on_actionClear_Drawing_triggered();
		void on_actionClear_Segmentation_triggered();

		void on_actionLoad_Segmentation_triggered();
		bool on_actionSave_Segmentation_triggered();

        void on_actionInterpolate_Image_toggled(bool enable);

		void tryEnableSegmentationWidgets();
		void enableSegmentationWidgets();
		void disableSegmentationWidgets();

		void on_doDilate2D_clicked();
		void on_doErode2D_clicked();
		void on_doClose2D_clicked();
		void on_doOpen2D_clicked();

		void on_actionRotate_view_by_180_toggled(bool flip);
		void on_actionResetView_triggered();

        void on_render3DButton_clicked();

        void on_xySingleViewButton_toggled(bool checked);

        void on_yzSingleViewButton_toggled(bool checked);

        void on_xzSingleViewButton_toggled(bool checked);

        void on_render3DSingleViewButton_toggled(bool checked);

        void viewChanged();

        void on_actionDump_debug_triggered();

protected:
        //! Handle QCloseEvents so that we can prompt the user to save if necessary
		void closeEvent(QCloseEvent* close);

	private:		
		//helper classes
		ImagePairManager* imagePairManager;
        ViewManager* xyView;
        ViewManager* xzView;
        ViewManager* yzView;
        MultiViewManager* multiViewManager;
		DrawManager* drawManager;
		Segmenter* segmenter;
		VolumeRenderManager* volumeRenderManager;

		QFileInfo imageInfo;
		QDir workPath; //Directory used file open dialogs
		Ui::MainWindow* ui; //handle to user interface
		QActionGroup* toolbarActions;
		QProgressDialog* progressDialog;


		//setup methods
		void contrastControlSetup();
		void toolbarSetup();
		void segmentationControlSetup();
		void showWaitDialog();
		void hideWaitDialog();

        void cleanUp();

        //load an image
        void loadOriginalImage(QString file);


		//Change the contrast based on what the widgets are currently set to
		void changeContrast();

		//prompt the user to save save segblock if necessary
		bool okToContinue();

		//! UiTester is a unit test class that needs internal access to this class
		friend class UiTester;
};

#endif
