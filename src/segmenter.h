#include <QObject>
#include <QComboBox>
#include "seedpointmanager.h"
#include "imagepairmanager.h"
#include <list>

using std::list;

struct Node
{
        int pos_x;
        int pos_y;
        int pos_z;
        Node(int x, int y, int z):pos_x(x), pos_y(y), pos_z(z){}
};


class Segmenter : public QObject
{
        Q_OBJECT

        public:
                Segmenter(SeedPointManager* seedPointManager, ImagePairManager* imagePairManager, QComboBox* kernelType);
                ~Segmenter();

                enum Segmentation{
                        SEGMENTATION_2D,
                        SEGMENTATION_3D
                };

                enum Morphology{
                        DILATE,
                        ERODE
                };

                enum Kernel{
                        CROSS,
                        SQUARE
                };

                void doSegmentation2D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold);

                void doSegmentation3D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold, int min_Z, int max_Z);

                void doMorphOpen(int pos_z);

                void doMorphClose(int pos_z);

                void doDilate(int pos_z);

                void doErode(int pos_z);

        signals:
                void segmentationDone(int sliceNumber);

                void filteringDone(int sliceNumber);

        private:
                ImagePairManager* imagePairManager;                           

                bool predicate2D(Node& node, int minThreshold, int maxThreshold);

                bool predicate3D(Node& node, int minThreshold, int maxThreshold, int min_Z, int max_Z);

                void doSegmentationIter2D_I(Node start, int minThreshold, int maxThreshold);

                void doSegmentationIter3D_I(Node start, int minThreshold, int maxThreshold, int min_Z, int max_Z);

                void erode(int pos_z, Kernel kernel);

                void dilate(int pos_z, Kernel kernel);

                int contains_segmentation(int pos_x, int pos_y, int pos_z, Morphology type, Kernel kernel);

                int img_x;
                int img_y;
                int img_z;

                QComboBox* kernelType;


};
