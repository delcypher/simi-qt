#include "customInteractorStyle.h"
#include <QDebug>

void CustomInteractorStyle::OnLeftButtonDown()
{
	// Get the location of the click (in window coordinates)
	int* pos = this->GetInteractor()->GetEventPosition();

	vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
	//picker->SetTolerance(0.0005);

	qDebug() << "Window position is:" << pos[0] << pos[1] << endl;

	// Pick from this location.
	picker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());

	if(picker->GetCellId() != -1)
	{
		qDebug() << "GetCellID is: " << picker->GetCellId();
		qDebug() << "GetPickPosition is:" << picker->GetPickPosition()[0] << picker->GetPickPosition()[1] << picker->GetPickPosition()[2];
		qDebug() << "GetPointIJK is:" << picker->GetPointIJK()[0] << picker->GetPointIJK()[1] << picker->GetPointIJK()[2] << endl;
	}
	else
	{
		qDebug() << "Out of image." << endl;
	}


    //If CTRL key is pressed then leave function so we don't call parent's method that will allow rotation.
    if(this->GetInteractor()->GetControlKey())
        return;

	// Forward events
	vtkInteractorStyleImage::OnLeftButtonDown();
}

void CustomInteractorStyle::OnRightButtonDown()
{
	qDebug() << "Zoom via scroll wheel; right mouse button is disabled";
}

