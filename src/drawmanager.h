/*! \file drawmanager.h */

//!  Manages drawing into the blocking/segmentation image

#ifndef DRAWMANAGER_H
#define DRAWMANAGER_H

#include <QObject>
#include "imagepairmanager.h"
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>

/*!
  The class provides blocking and segmentation drawing and erasing tools.
  Note that blocking can be either drawn one slice at a time or all slices at once.
  Segmentation drawing and erasing can only be performed one slice at a time.
*/
class DrawManager : public QObject
{
	Q_OBJECT

	public:
		//! Class constructor.
		DrawManager(ImagePairManager* imagePairManager, QSpinBox* drawSize, QComboBox* drawType, QSpinBox* minZSlice, QSpinBox* maxZSlice, QCheckBox* segmentation);

		//! Class destructor.
		~DrawManager();
	
	signals:
        /*! Emitted when drawing is complete */
        void drawingDone(); //emitted to indicate drawing is done

	public slots:
		//! Drawing both blocking and segmentation function.
			/*!
				\param xVoxel current position of cursor in x coordinate.
				\param yVoxel current position of cursor in y coordinate.
				\param zVoxel current position of cursor in z coordinate.
			*/
		void draw(int xVoxel, int yVoxel, int zVoxel);

		//! Erasing function.
			/*!
				\param xVoxel current position of cursor in x coordinate.
				\param yVoxel current position of cursor in y coordinate.
				\param zVoxel current position of cursor in z coordinate.
			*/
		void erase(int xVoxel, int yVoxel, int zVoxel);

	private:
        ImagePairManager* imagePairManager;
        QSpinBox* drawSize;
        QComboBox* drawType;
		QSpinBox* minZSlice;
		QSpinBox* maxZSlice;
		QCheckBox* segReadOnly;

		void drawAlgorithm(int &xVoxel, int &yVoxel, int &zVoxel, int &mode); // helper function for draw and erase function

		enum DrawMode
		{
			DRAW,
			ERASE
		};

	
};

#endif
