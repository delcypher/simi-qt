#include "vtkInteractorStyleImage.h"


class CustomInteractorStyle : public vtkInteractorStyleImage
{
  public:
  virtual void OnMouseWheelForward();
  virtual void OnMouseWheelBackward();
  virtual void OnLeftButtonUp();
};
