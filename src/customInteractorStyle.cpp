#include "customInteractorStyle.h"
#include <QDebug>


void CustomInteractorStyle::OnMouseWheelForward()
{
	qDebug() << "Wheel Forward";
}

void CustomInteractorStyle::OnMouseWheelBackward()
{
	qDebug() << "Wheel Backward";
}

void CustomInteractorStyle::OnLeftButtonUp()
{
	qDebug() << "Picker implementation here";
}

