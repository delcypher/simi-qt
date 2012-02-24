#include "customInteractorStyle.h"
#include <QDebug>

void CustomInteractorStyle::OnLeftButtonDown()
{
	// Get the location of the click (in window coordinates)
	int* pos = this->GetInteractor()->GetEventPosition();

	vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
	//picker->SetTolerance(0.0005);

	// Pick from this location.
	picker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());

	if(picker->GetCellId() != -1)
	{
		qDebug() << "GetCellID is: " << picker->GetCellId();
		qDebug() << "GetPickPosition is:" << *picker->GetPickPosition();
		qDebug() << "GetPointIJK is:" << *picker->GetPointIJK();
	}
	else
	{
		qDebug() << "Out of image.";
	}

	// Forward events
	vtkInteractorStyleImage::OnLeftButtonDown();
}

void CustomInteractorStyle::OnRightButtonDown()
{
	qDebug() << "Zoom via scroll wheel; right mouse button is disabled";
}

