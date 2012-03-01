#ifndef DRAWMANAGER_H
#define DRAWMANAGER_H

#include <QObject>
#include "imagepairmanager.h"
#include <QSpinBox>
#include <QComboBox>

class DrawManager : public QObject
{
	Q_OBJECT

	public:
        DrawManager(ImagePairManager* imageManager, QSpinBox* drawSize, QComboBox* drawType);
		~DrawManager();
	
	public slots:
		void draw(int xVoxel, int yVoxel, int zVoxel);
		void erase(int xVoxel, int yVoxel, int zVoxel);

	private:
		ImagePairManager* imageManager;
        QSpinBox* drawSize;
        QComboBox* drawType;

	
};

#endif
