#include <QObject>
#include "seedpointmanager.h"
#include "imagepairmanager.h"

class Segmenter : public QObject
{
	Q_OBJECT

	public:
        Segmenter(SeedPointManager* seedPointManager, ImagePairManager* imagePairManager);
		~Segmenter();

		void doSegmentation(int sliceNumber, int minThreshold, int maxThreshold);

	signals:
		void segmentationDone(int sliceNumber);

	private:
        ImagePairManager* imagePairManager;
};
