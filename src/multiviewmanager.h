#ifndef MULTIVIEWMANAGER_H
#define MULTIVIEWMANAGER_H

#include <QObject>
#include <QPushButton>
#include "viewmanager.h"
#include <QVTKWidget.h>
#include <vtkImageViewer2.h>

class MultiViewManager : public QObject
{
    Q_OBJECT

    public:
        MultiViewManager(ViewManager* xy, ViewManager* xz, ViewManager* yz,
                        QPushButton* xyButton, QPushButton* xzButton,
                        QPushButton* yzButton,
                        unsigned int defaultView);
        unsigned int getActiveView();
        ViewManager* getActiveViewPointer();
        bool getSeedPoint(int& x, int& y, int& z);
        void update();
        void enableInterpolation(bool enable);
        void enablePanning(bool e);


    signals:
        void seedPointChanged();
        void viewChanged();


    public slots:
        void setXYActive();
        void setXZActive();
        void setYZActive();
        bool setSeedPoint(int x, int y, int z);


    private:
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
