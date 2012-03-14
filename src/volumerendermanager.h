//!  VolumeRenderManager.
/*!
  This class is responsible for 3D rendering.
  It receives StructuredPoint data from ImagePairManager and send output to qvtk3Ddisplayer.
  Only segmentation StructuredPoint will be rendered and displayed as 3D object.
*/

#ifndef VOLUMERENDERMANAGER_H
#define VOLUMERENDERMANAGER_H

#include <QObject>
#include <QVTKWidget.h>
#include "imagepairmanager.h"
#include "vtkMarchingCubes.h"
#include "volumerendermanager.h"


#include "vtkSmartPointer.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageMaskBits.h"




class VolumeRenderManager : public QObject
{
	Q_OBJECT

	public:
	//! VolumeRenderManager constructor.
	VolumeRenderManager(ImagePairManager* imagePairManager, QVTKWidget* qvtk3Ddisplayer);


	//! VolumeRenderManager destructor.
	~VolumeRenderManager();

	public slots:

		//! This function is to draw 3D object on qvtk3Ddisplayer (QVTKWidget) and also keep updating the drawn object.
		    /*!
		      This function will be called when "Render" button in mainwindow is pressed.
		      Only segmented structuredPoint will be filtered and drawn on the widget.
		    */
		void render3D();

		//! This function is to rotate (or flip) 3D object drawn in the QVTKWidget.
		    /*!
		      This function will be called when "Rotate view by 180" dropdown in mainwindow is triggered.
		      \param flip flag checking if Rotate view by 180 is triggered
		    */
		void flipView(bool flip);

	private:
        ImagePairManager* imagePairManager;
        QVTKWidget* qvtk3Ddisplayer;
	vtkSmartPointer<vtkMarchingCubes> surface ;
	vtkSmartPointer<vtkRenderer> renderer ;
	vtkSmartPointer<vtkRenderWindow> renderWindow ;
	vtkSmartPointer<vtkPolyDataMapper> mapper ;
	vtkSmartPointer<vtkActor> actor ;
	vtkSmartPointer<vtkImageMaskBits> mask ;

};

#endif
