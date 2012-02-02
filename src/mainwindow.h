#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H 1

#include <QMainWindow>
#include <QString>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow , public Ui::MainWindow
{
	Q_OBJECT

	public:
		MainWindow(QString& filepath );
	
	private:
		QString mFilepath;
};

#endif
