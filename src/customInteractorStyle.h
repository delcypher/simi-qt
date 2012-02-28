#include "vtkInteractorStyleImage.h"
#include "vtkSmartPointer.h"
#include "vtkCellPicker.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkStructuredPoints.h"

class CustomInteractorStyle : public vtkInteractorStyleImage
{
	public:
		CustomInteractorStyle(vtkStructuredPoints* img, vtkRenderer* renderer);
		virtual void OnLeftButtonDown();
		virtual void OnRightButtonDown();
		virtual void OnMouseWheelForward() { zoomOut();}
		virtual void OnMouseWheelBackward() { zoomIn();}
		bool zoomIn();
		bool zoomOut();
		void resetZoom();

	private:
		vtkStructuredPoints* img;
		double minScale; // smallest height of viewport (i.e. max zoom)
		double maxScale; //largest height of viewport (i.e. min zoom)
		double zoomScaleStep; //the just in scale when calling zoom methods

};
