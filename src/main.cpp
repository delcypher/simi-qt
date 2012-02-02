#include <iostream>
#include <QApplication>
#include <QFileInfo>
#include <QString>

#include "mainwindow.h"

int main(int argc, char* argv[])
{
	QApplication app(argc,argv);

        if(argc < 2)
	{
		std::cerr << "Usage: " << std::endl << argv[0] << " <image filename>" << std::endl;
		return -1;
	}

	//check can open file
	QString filename(argv[1]);
	QFileInfo info(filename);

	if(!info.isReadable())
	{
		std::cerr << "Error: Image " << filename.toStdString() << " is not readable!" << std::endl;
		return -2;
	}



	MainWindow* mw = new MainWindow(filename);
	mw->show();

	return app.exec();
}
