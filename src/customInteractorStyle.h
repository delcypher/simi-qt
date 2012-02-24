#include "vtkInteractorStyleImage.h"
#include "vtkSmartPointer.h"
#include "vtkCellPicker.h"
#include "vtkRenderWindowInteractor.h"

class CustomInteractorStyle : public vtkInteractorStyleImage
{
public:
  static CustomInteractorStyle* New();
  virtual void OnLeftButtonDown();
  virtual void OnRightButtonDown();
};
