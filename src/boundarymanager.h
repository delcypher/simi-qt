#ifndef BOUNDARYMANAGER_H
#define BOUNDARYMANAGER_H

#include <QObject>
#include <QSpinBox>
#include <QPushButton>
#include "imagepairmanager.h"

class BoundaryManager : public QObject
{
    Q_OBJECT
    public:
        explicit BoundaryManager(ImagePairManager* imagePairManager,
                                QSpinBox* xMin,
                                QSpinBox* xMax,
                                QSpinBox* yMin,
                                QSpinBox* yMax,
                                QSpinBox* zMin,
                                QSpinBox* zMax,
                                QPushButton* resetButton,
                                QObject *parent);
        int getXMin();
        int getXMax();
        int getYMin();
        int getYMax();
        int getZMin();
        int getZMax();
        bool isInBoundary(int x, int y, int z);
        bool isInXRange(int x);
        bool isInYRange(int y);
        bool isInZRange(int z);


    signals:
        void boundaryChanged();
    public slots:
        bool reset();
    private slots:
        void on_xMin_valueChanged(int value);
        void on_xMax_valueChanged(int value);
        void on_yMin_valueChanged(int value);
        void on_yMax_valueChanged(int value);
        void on_zMin_valueChanged(int value);
        void on_zMax_valueChanged(int value);


    private:
        QSpinBox* xMinSpinBox;
        QSpinBox* xMaxSpinBox;
        QSpinBox* yMinSpinBox;
        QSpinBox* yMaxSpinBox;
        QSpinBox* zMinSpinBox;
        QSpinBox* zMaxSpinBox;
        QPushButton* resetPushButton;

        ImagePairManager* imagePairManager;

};

#endif // BOUNDARYMANAGER_H
