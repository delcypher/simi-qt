#include <QApplication>


#include "mainwindow.h"

int main(int argc, char* argv[])
{
	QApplication app(argc,argv);

	// change the application's GUI style to "plastique"
	QApplication::setStyle("plastique");

	MainWindow* mw = new MainWindow();
	mw->show();

	return app.exec();
}
