#include <QObject>
#include <QFileInfo>
#include "vtkSmartPointer.h"
#include <vtkStructuredPoints.h>
#include "vtkStructuredPointsReader.h"

/* Responsible for loading original image and making it avaliable.
*  It is also responsible for producing the segmentation/blocking image
*/
class ImagePairManager : public QObject
{
	Q_OBJECT
	public:
		ImagePairManager();
		~ImagePairManager();
		bool loadImage(QFileInfo image);
		
		//Reset data for slice
		void resetSegmentation(int slice);
		void resetBlocking(int slice);

		//Images
		vtkSmartPointer<vtkStructuredPoints> original;
		vtkSmartPointer<vtkImageData> segblock;

		int getXWidth() {return xWidth;}
		int getYWidth() {return yWidth;}
		int getZWidth() {return zWidth;}

	private:
		vtkSmartPointer<vtkStructuredPointsReader> reader;

		//Widths in terms of voxels
		int xWidth;
		int yWidth;
		int zWidth;

};
