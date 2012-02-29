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

		int getXDim() {return xDim;}
		int getYDim() {return yDim;}
		int getZDim() {return zDim;}

	private:
		vtkSmartPointer<vtkStructuredPointsReader> reader;

		//Widths in terms of voxels
		int xDim;
		int yDim;
		int zDim;

};
