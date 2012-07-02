#ifndef MULTIVIEWMANAGER_H
#define MULTIVIEWMANAGER_H

#include <QObject>
#include <QPushButton>
#include "viewmanager.h"
#include <QVTKWidget.h>
#include <vtkImageViewer2.h>
#include "boundarymanager.h"

class MultiViewManager : public QObject
{
    Q_OBJECT

    public:
        MultiViewManager(BoundaryManager* boundaryManager, ViewManager* xy, ViewManager* xz, ViewManager* yz,
                        QPushButton* xyButton, QPushButton* xzButton,
                        QPushButton* yzButton,
                        unsigned int defaultView);
        unsigned int getActiveView();
        ViewManager* getActiveViewPointer();
        bool getSeedPoint(int& x, int& y, int& z);

        void enableInterpolation(bool enable);
        void enablePanning(bool e);
        bool isCrosshairInBoundary();


    signals:
        void seedPointChanged();
        void viewChanged(unsigned int v);


    public slots:
        void setXYActive();
        void setXZActive();
        void setYZActive();
        bool setSeedPoint(int x, int y, int z);
        void redrawCrosshair();
        void update();

    private slots:
        void checkCrosshairInBoundary();


    private:
        BoundaryManager* boundaryManager;
        ViewManager* xyView;
        ViewManager* xzView;
        ViewManager* yzView;

        QPushButton* xyButton;
        QPushButton* xzButton;
        QPushButton* yzButton;

        int xSeed;
        int ySeed;
        int zSeed;

        unsigned int activeView;



};

#endif // MULTIVIEWMANAGER_H
