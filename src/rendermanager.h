#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <QObject>
#include <QVTKWidget.h>
#include "imagepairmanager.h"


class RenderManager : public QObject
{
	Q_OBJECT

	public:
        RenderManager(ImagePairManager* imagePairManager, QVTKWidget* qvtk3Ddisplayer);
	~RenderManager();
	
	void render3D();

	private:
        ImagePairManager* imagePairManager;
        QVTKWidget* qvtk3Ddisplayer;

};

#endif
