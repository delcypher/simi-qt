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
        DrawManager(ImagePairManager* imagePairManager, QSpinBox* drawSize, QComboBox* drawType);
		~DrawManager();
	
	signals:
        void drawingDone(); //emitted to indicate drawing is done

	public slots:
		void draw(int xVoxel, int yVoxel, int zVoxel);
		void erase(int xVoxel, int yVoxel, int zVoxel);

	private:
        ImagePairManager* imagePairManager;
        QSpinBox* drawSize;
        QComboBox* drawType;

	
};

#endif
