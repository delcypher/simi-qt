/*! \file imagepairmanager.h */
#ifndef IMAGEPAIRMANAGER_H
#define IMAGEPAIRMANAGER_H

#include <QObject>
#include <QFileInfo>
#include "vtkSmartPointer.h"
#include <vtkStructuredPoints.h>
#include "vtkStructuredPointsReader.h"
#include "vtkStructuredPointsWriter.h"

//! Manages the original image and the blocking/segmentation image

/*! Responsible for loading original image and making it available along with its
*   blocking/segmentation image.
*
*   Note that most properties of the blocking/segmentation are the same as that of the
*   original image. However segblock uses voxel type char which should only be set to
*   values in the ImagePairManager::BlockType enum.
*/
class ImagePairManager : public QObject
{
	Q_OBJECT
	public:

        /*! Constructor
        *   This allocates the vtkSmartPointers for original and segblock.
        *   Note no image data is set at this time.
        */
        ImagePairManager();

        /*! Destructor
        */
		~ImagePairManager();

        /*! Load a VtkStructurePoints file into original. After this is done
        *  the segmentation/blocking data is created which duplicates
        *  original's origin, dimensions, spacing and extent.
        *
        *  The segmention/blocking voxels are initialised to ImagePairManager::BACKGROUND
        *  \param image is the VtkStructuredPoints file to load
        *  \sa original
        *  \sa segblock
        *  \return True if successful.
        */
        bool loadImage(QFileInfo image);

        /*! Load the contents of an existing VtkStructuredPoints object into original.
        *   This does a deep copy into src. After this is done
        *  the segmentation/blocking data is created which duplicates
        *  original's origin, dimensions, spacing and extent.
        *
        *  The segmention/blocking voxels are initialised to ImagePairManager::BACKGROUND
        *  \param src is a pointer to an existing VtkStructuredPoints object to do a deep copy from
        *  \sa original
        *  \sa segblock
        *  \return True if successful.
        */
        bool loadImageFromSource(vtkStructuredPoints* src);
		
        //! Original (MRI/CT) image
		vtkSmartPointer<vtkStructuredPoints> original;
        //! blocking/segmentation image
        vtkSmartPointer<vtkStructuredPoints> segblock;

        /*! \return The number of voxels in the x direction in original or segblock (they are identical)
        * \sa original
        * \sa segblock
        */
        int getXDim() {return xDim;}

        int getXMin() { return extentXMin;}

        int getXMax() { return extentXMax;}


        /*! \return The number of voxels in the y direction in original or segblock (they are identical)
        * \sa original
        * \sa segblock
        */
		int getYDim() {return yDim;}

        int getYMin() {return extentYMin;}
        int getYMax() {return extentYMax;}

        /*! \return The number of voxels in the z direction in original or segblock (they are identical)
        * \sa original
        * \sa segblock
        */
		int getZDim() {return zDim;}

        /*! \return The minimum voxel index in the z direction in original or segblock (they are identical).
        *   This is usually 0.
        *   \sa original
        *   \sa segblock
        */
		int getZMin() {return extentZMin;}

        /*! \return The maximum voxel index in the z direction in original or segblock (they are identical)
        *   This is usually equivilant to getZDim() -1;
        *   \sa original
        *   \sa segblock
        */
		int getZMax() {return extentZMax;}

         /*! \return The length of an individual voxel in the x direction in the
        *   original or segblock image (they are identical).
        *   \sa original
        *   \sa segblock
        */
        double getXSpacing();

        /*! \return The length of an individual voxel in the y direction in the
        *   original or segblock image (they are identical).
        *   \sa original
        *   \sa segblock
        */
		double getYSpacing();

		/*! \return The length of an individual voxel in the z direction in the
        *   original or segblock image (they are identical).
        *   \sa original
        *   \sa segblock
        */
		double getZSpacing();

        /*!
        *  \return the minimum value of all the voxels in the original image.
        *  \sa original
        */
		double getMinimumIntensity();

        /*!
        *  \return the maximum value of all the voxels in the original image.
        *  \sa original
        */
		double getMaximumIntensity();

        //! These are the different possible values of voxels in segblock \sa segblock
		enum BlockType
		{
            BACKGROUND, //!< Used to represent "nothing" \sa ViewManager
            BLOCKING, //!< Used to represent "blocking" voxels \sa DrawManager
            SEGMENTATION //!< Used to represent "segmentation" voxels \sa Segmenter
		};

        /*! Set all the voxels in a particular slice of segblock that have value from to the value to
        *   \param orientation is the slice orientation to modify. Acceptible values are the enum values prefixed with "SLICE_ORIENTATION_" in VtkImageViewer2
        *   \param slice is the the slice number.
        *   \param from is the value of voxels in the slice to change
        *   \param to is the value to set for voxels that are being changed.
        *   \return True if successful otherwise false.
        *   \sa segblock
        */
        bool setAll(unsigned int orientation, int slice, ImagePairManager::BlockType from, ImagePairManager::BlockType to);


        /*! Set all the voxels in all slices that have value from to the value to
        *   \param from is the value of voxels to change
        *   \param to is the value to set for voxels that are being changed.
        *   \return True if successful otherwise false.
        */
        bool setAllSimBlockVoxels(ImagePairManager::BlockType from, ImagePairManager::BlockType to);


        /*! Save the blocking/segmentation image (segblock) to file named by path.
        *  This also resets ImagePairManager::segblockModified() to false.
        *  \param path is the filepath to save the image to
        *  \return True if successful otherwise false.
        *  \sa segblock
        */
        bool saveSegblock(QString path);

        /*! Load the blocking/segmentation image to file named by path. This will overwrite
        *   any data currently stored in blocking/segmentation (segblock).
        *
        *  \param path is the filepath to save the image to
        *  \return True if successful otherwise false.
        *  \sa segblock
        */
        bool loadSegblock(QString path);

        /*! This is used to determine whether or not the blocking/segmentation data has been
        *   modified. Internally this uses VTK's MTime to keep track of this so it is up to
        *   classes that modify this image to call vtkStructuredPoints::Modified().
        *  \return True if the blocking/segmentation has been modified since calling
        *   saveSegBlock(), loadImage(), loadImageFromSource() or loadSegblock()
        */
		bool segblockModified();

	private:
		vtkSmartPointer<vtkStructuredPointsReader> reader;
		vtkSmartPointer<vtkStructuredPointsReader> readerSegBlock;

		//Widths in terms of voxels
		int xDim;
		int yDim;
		int zDim;

		//Extents
		int extentXMin;
		int extentXMax;
		int extentYMin;
		int extentYMax;
		int extentZMin;
		int extentZMax;

		void debugDump();

		unsigned long segblockInitTime;

};

#endif
