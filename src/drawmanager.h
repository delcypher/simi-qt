#ifndef DRAWMANAGER_H
#define DRAWMANAGER_H

#include <QObject>
#include "imagepairmanager.h"

class DrawManager : public QObject
{
	Q_OBJECT

	public:
		DrawManager(ImagePairManager* imageManager);
		~DrawManager();
	
	public slots:
		void draw(int xVoxel, int yVoxel, int zVoxel);
		void erase(int xVoxel, int yVoxel, int zVoxel);

	private:
		ImagePairManager* imageManager;
	
};

#endif
