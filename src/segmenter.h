#include <QObject>
#include "seedpointmanager.h"
#include "imagepairmanager.h"

class Segmentation : public QObject
{
	Q_OBJECT

	public:
		Segmentation(SeedPointManager* seedManager, ImagePairManager* imageManager);
		~Segmentation();
};
