#include <QObject>
#include "seedpointmanager.h"
#include "imagepairmanager.h"



class Segmenter : public QObject
{
	Q_OBJECT

	public:
                Segmenter(SeedPointManager* seedPointManager, ImagePairManager* imagePairManager);
                ~Segmenter();

                enum Segmentation{
                        SEGMENTATION_2D,
                        SEGMENTATION_3D
                };

                void doSegmentation2D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold);

                void doSegmentation3D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold);

	signals:
		void segmentationDone(int sliceNumber);

	private:
                ImagePairManager* imagePairManager;

                bool predicate2D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold);

                bool predicate3D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold);

                void doSegmentation2D_I(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold);

                void doSegmentation3D_I(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold);


};
