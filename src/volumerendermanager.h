#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

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
	VolumeRenderManager(ImagePairManager* imagePairManager, QVTKWidget* qvtk3Ddisplayer);
	~VolumeRenderManager();

	public slots:
		void render3D();
		void flipView(bool flip);
		void rebuildPipeline();

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
