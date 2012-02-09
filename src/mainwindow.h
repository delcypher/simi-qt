#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H 1

#include <QMainWindow>
#include <QString>
#include "ui_mainwindow.h"

#include "vtkImageViewer2.h"
#include "vtkSmartPointer.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkStructuredPoints.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkStructuredPointsReader.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(QString& filepath );
		~MainWindow();
	
private slots:
		void on_doSomething_clicked();
		void on_actionSlice_up_triggered();
		void on_actionSlice_down_triggered();

private:
		QString mFilepath;
		Ui::MainWindow* ui; //handle to user interface
		vtkSmartPointer<vtkStructuredPointsReader> reader;
		vtkImageViewer2* imageView;

		int currentSlice;



};

#endif
