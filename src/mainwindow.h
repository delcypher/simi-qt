#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H 1

#include <QMainWindow>
#include <QString>
#include <QDir>
#include <QFileInfo>
#include "ui_mainwindow.h"

#include "imagepairmanager.h"
#include "seedpointmanager.h"
#include "layoutmanager.h"




//Forward Declare UiTester
class UiTester;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow();
		~MainWindow();

		//helper classes
		ImagePairManager* imageManager;
		SeedPointManager* seedManager;
		LayoutManager* layoutManager;


	
	private slots:
		void on_actionOpen_Image_triggered();
		void on_actionSlice_up_triggered();
		void on_actionSlice_down_triggered();
		void on_actionAbout_triggered();

		void on_minIntensitySlider_valueChanged(int value);
		void on_maxIntensitySlider_valueChanged(int value);
		void on_sliceSlider_valueChanged(int value);

	private:
		QFileInfo imageInfo;
		QDir workPath; //Directory used file open dialogs
		Ui::MainWindow* ui; //handle to user interface



		//setup methods
		void contrastControlSetup();
		void sliceControlSetup();


		void changeContrast();

		//Friend class for UI testing.
		friend class UiTester;
};

#endif
