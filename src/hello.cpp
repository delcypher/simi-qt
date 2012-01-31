#include "itkImage.h"
#include <iostream>
#include <QApplication>
#include <QHBoxLayout>
#include <QSlider>
#include <QSpinBox>
#include <vtkMath.h>

int main(int argc, char* argv[])
{
	typedef itk::Image< unsigned short, 3 > ImageType;
	ImageType::Pointer image = ImageType::New();
	std::cout << "ITK Hello World !" << vtkMath::Gaussian(0.0,2.0) << std::endl;

	QApplication app(argc,argv);
	QWidget* window = new QWidget;
	window->setWindowTitle("Simi-qt");

	QSpinBox* spinBox= new QSpinBox;
	QSlider* slider = new QSlider(Qt::Horizontal);
	spinBox->setRange(0,130);
	slider->setRange(0,130);

	QObject::connect(spinBox,SIGNAL(valueChanged(int)),
			slider,SLOT(setValue(int)));

	QObject::connect(slider, SIGNAL(valueChanged(int)),
			spinBox, SLOT(setValue(int)));

	spinBox->setValue(35);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(spinBox);
	layout->addWidget(slider);
	window->setLayout(layout);

	window->show();


	return app.exec();
}
