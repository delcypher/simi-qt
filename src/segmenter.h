#ifndef SEGMENTER_H
#define SEGMENTER_H

/*! \file segmenter.h */

//!  Segmenter.

#include <QObject>
#include <QComboBox>
#include "seedpointmanager.h"
#include "imagepairmanager.h"
#include <list>

using std::list;

//! Node used to represent pixels (2D) and voxels(3D).
struct Node
{
        int pos_x;
        int pos_y;
        int pos_z;
        Node(int x, int y, int z):pos_x(x), pos_y(y), pos_z(z){}
};

/*!
  The class responsible for image processing algorithms.
  It contains 2D and 3D versions of the flood fill algorithms to segment regions.
  Additionally it includes 2D morphological operators (dilate, erode, close, open).
*/
class Segmenter : public QObject
{
        Q_OBJECT

        public:
                //! Class constructor.
                Segmenter(SeedPointManager* seedPointManager, ImagePairManager* imagePairManager, QComboBox* kernelType);

                //! Class destructor.
                ~Segmenter();

                //! Types of morphological operators.
                enum Morphology{
                        DILATE,
                        ERODE
                };

                //! Type of the structuring element used in morphological operators.
                enum Kernel{
                        CROSS,
                        SQUARE
                };

                //! 2D segmentation algorithm. Performs segmentation on the current slice.
                      /*!
                         \param pos_x first dimension.
                         \param pos_y second dimension.
                         \param pos_z third dimension (slice number).
                         \param minThreshold minimum level of intensity that will pass the segmentation.
                         \param maxThreshold maximum level of intensity that will pass the segmentation.
                      */
                void doSegmentation2D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold);

                //! 3D segmentation algorithm. Performs segmentation on a range of slices.
                      /*!
                         \param pos_x first dimension.
                         \param pos_y second dimension.
                         \param pos_z third dimension (slice number).
                         \param minThreshold minimum level of intensity that will pass the segmentation.
                         \param maxThreshold maximum level of intensity that will pass the segmentation.
                         \param min_Z segmentation lower slice limit.
                         \param max_Z segmentation upper slice limit.
                      */
                void doSegmentation3D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold, int min_Z, int max_Z);

                //! Morphological open algorithm (erosion followed by dilation).
                      /*!
                         \param pos_z third dimension (slice number).
                      */
                void doMorphOpen(int pos_z);

                //! Morphological close algorithm (dilation followed by erosion).
                      /*!
                         \param pos_z third dimension (slice number).
                      */
                void doMorphClose(int pos_z);

                //! Morphological dilate algorithm.
                      /*!
                         \param pos_z third dimension (slice number).
                      */
                void doDilate(int pos_z);

                //! Morphological erode algorithm.
                      /*!
                         \param pos_z third dimension (slice number).
                      */
                void doErode(int pos_z);

                bool isWorking();

        signals:
                void segmentationDone(int sliceNumber);

        public slots:
                bool cancel3D();

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

                char*** visited3D;

                char** visited2D;

                list<Node> visited3D_list;
                list<Node> visited2D_list;

                bool segmentation_running;

};

#endif
