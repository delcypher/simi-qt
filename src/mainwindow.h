#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H 1

#include <QMainWindow>
#include <QString>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(QString& filepath );
		~MainWindow();
	
	private:
		QString mFilepath;
		Ui::MainWindow* ui; //handle to user interface
};

#endif
