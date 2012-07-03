#ifndef SEGMENTER_H
#define SEGMENTER_H

/*! \file segmenter.h */



#include <QObject>
#include <QComboBox>
#include "imagepairmanager.h"
#include <list>
#include "boundarymanager.h"
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

using std::list;

//! Node used to represent pixels (2D) and voxels(3D).
struct Node
{
        int pos_x; //!< voxel x coordinate.
        int pos_y; //!< voxel y coordinate.
        int pos_z; //!< voxel z coordinate.
        //! Struct constructor.
        Node(int x, int y, int z):pos_x(x), pos_y(y), pos_z(z){}
};

//forward declare class
class SegmenterTester;

//!  Implements segmentation and morphological algorithms

/*!
  The class responsible for image processing algorithms.
  It contains 2D and 3D versions of the flood fill algorithms to segment regions.
  Additionally it includes 2D morphological operators (dilate, erode, close, open).
*/
class Segmenter : public QThread
{
        Q_OBJECT

        public:
                //! Class constructor.
                Segmenter(ImagePairManager* imagePairManager, BoundaryManager* boundaryManager,QComboBox* kernelType, QObject* parent=0);

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
                void doSegmentation2D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold, unsigned int orientation);

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
                void doSegmentation3D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold);

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

        signals:
                //! Signals that the segmentation algorithm has finished running.
                      /*!
                         \param sliceNumber third dimension (z axis).
                      */
                void segmentationDone();

        public slots:
                //! Slot for stopping 2D/3D segmentation.
                void cancel();

        protected:
            void run();

        private:
                ImagePairManager* imagePairManager;
                BoundaryManager* boundaryManager;

                bool predicate3D(Node& node, int minThreshold, int maxThreshold);

                void doSegmentationIter2D_I(Node start, int minThreshold, int maxThreshold, unsigned int orientation);

                void doSegmentationIter3D_I(Node start, int minThreshold, int maxThreshold);

                void erode(int pos_z, Kernel kernel);

                void dilate(int pos_z, Kernel kernel);

                int contains_segmentation(int pos_x, int pos_y, int pos_z, Morphology type, Kernel kernel);

                void clearVistedArray();

                int img_x;
                int img_y;
                int img_z;

                QComboBox* kernelType;

                char*** visited3D;

                //used to protect the variables below
                QMutex mutex;
                //Indicates
                enum Tasks
                {
                    SLEEP,
                    SEGMENTATION_2D,
                    SEGMENTATION_3D
                } task;
                bool abort;

                QWaitCondition workToDo;

                int mSeedX;
                int mSeedY;
                int mSeedZ;
                int mMinThreshold;
                int mMaxThreshold;
                unsigned int mOrientation;


		friend class SegmenterTester;//! Friend class is unit test
};

#endif
