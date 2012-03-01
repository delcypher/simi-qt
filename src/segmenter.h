#include <QObject>
#include "seedpointmanager.h"
#include "imagepairmanager.h"

class Segmenter : public QObject
{
	Q_OBJECT

	public:
		Segmenter(SeedPointManager* seedManager, ImagePairManager* imageManager);
		~Segmenter();

		void doSegmentation(int sliceNumber, int minThreshold, int maxThreshold);

	signals:
		void segmentationDone(int sliceNumber);
};
