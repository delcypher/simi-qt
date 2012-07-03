#include "segmenter.h"
#include <QDebug>
#include <QApplication>
#include "vtkImageViewer2.h"
#include <QMutexLocker>


Segmenter::Segmenter(ImagePairManager* imagePairManager, BoundaryManager* boundaryManager, QComboBox* kernelType, QObject* parent) : QThread(parent)
{
        this->imagePairManager = imagePairManager;
        this->boundaryManager = boundaryManager;
        this->kernelType = kernelType;

        task=SLEEP; //Set inital task to sleep.
        abort=false;

        img_x = imagePairManager->getXDim();
        img_y = imagePairManager->getYDim();
        img_z = imagePairManager->getZDim();

        //set visited3D block
        visited3D = new char**[img_x];
        for (int i=0; i<img_x; i++)
                visited3D[i] = new char*[img_y];
        for (int i=0; i<img_x; i++)
                for (int j=0; j<img_y; j++)
                        visited3D[i][j] = new char[img_z];

        //fill visited3D with zeros
        clearVistedArray();

        qDebug() << "Constructing segmenter: " << img_x << " " << img_y << " " << img_z;

        //start the thread
        start();
}

Segmenter::~Segmenter()
{
        wait(); //wait for run() to terminate

        //release memory for 3D visited block
        for (int i=0; i<img_x; i++)
                for (int j=0; j<img_y; j++)
                        delete[] visited3D[i][j];
        for (int i=0; i<img_x; i++)
                delete[] visited3D[i];
        delete[] visited3D;
}

void Segmenter::run()
{
    //stack copy of needed variables (it could be dangerous to members)
    int seedX=0;
    int seedY=0;
    int seedZ=0;
    int minThreshold=0;
    int maxThreshold=0;
    unsigned int orientation=0;
    Segmenter::Tasks currentTask;

    //loop forever
    while(true)
    {
        //Assign stack copy of variables from our members
        mutex.lock();
        seedX=this->mSeedX;
        seedY=this->mSeedY;
        seedZ=this->mSeedZ;
        minThreshold=this->mMinThreshold;
        maxThreshold=this->mMaxThreshold;
        orientation=mOrientation;
        currentTask=this->task;
        mutex.unlock();

        switch(currentTask)
        {
            case SLEEP:
                mutex.lock();
                    qDebug() << "Segmenter going to sleep";
                    workToDo.wait(&mutex);//sleep
                    qDebug() << "Segmenter has woken up!";
                mutex.unlock();

            break;

            case SEGMENTATION_2D:
                qDebug() << "Segmenter doing 2D segmentation";
                doSegmentationIter2D_I(Node(seedX, seedY, seedZ), minThreshold, maxThreshold, orientation);
                imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK knows to trigger an update along the pipline
                emit segmentationDone();

                //It looks like we are done so let's request to go to sleep
                mutex.lock();
                this->task=SLEEP;
                mutex.unlock();
            break;

            case SEGMENTATION_3D:
                qDebug() << "Segmenter doing 3D segmentation";
                doSegmentationIter3D_I(Node(seedX, seedY, seedZ), minThreshold, maxThreshold);
                imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK knows to trigger an update along the pipline
                emit segmentationDone();

                //It looks like we are done so let's request to go to sleep
                mutex.lock();
                this->task=SLEEP;
                mutex.unlock();
            break;
        }

        //check if we need to abort (Warning not using a mutex here!)
        if(abort)
            return;



    }
}

void Segmenter::doSegmentation2D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold, unsigned int orientation)
{
    //set parameters
    QMutexLocker locker(&mutex); //lock mutex until we go out of scope

    //check thread is not doing work
    if(task!=SLEEP)
    {
        qWarning() << "Cannot do doSegmentation2D(). Thread appears to already be working!";
        return;
    }


    mSeedX = pos_x;
    mSeedY = pos_y;
    mSeedZ = pos_z;
    mMinThreshold = minThreshold;
    mMaxThreshold = maxThreshold;
    mOrientation = orientation;
    task = SEGMENTATION_2D; //Set the thread's task

    if(!isRunning())
        start();
    else
        workToDo.wakeOne(); //The Thread may be sleeping, wake it up!

}

void Segmenter::doSegmentation3D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold)
{
    //set parameters
    QMutexLocker locker(&mutex); //lock mutex until we go out of scope

    //check thread is not doing work
    if(task!=SLEEP)
    {
        qWarning() << "Cannot do doSegmentation3D(). Thread appears to already be working!";
        return;
    }


    mSeedX = pos_x;
    mSeedY = pos_y;
    mSeedZ = pos_z;
    mMinThreshold = minThreshold;
    mMaxThreshold = maxThreshold;
    task = SEGMENTATION_3D; //Set the thread's task

    if(!isRunning())
        start();
    else
        workToDo.wakeOne(); //The Thread may be sleeping, wake it up!
}

void Segmenter::doSegmentationIter2D_I(Node start, int minThreshold, int maxThreshold, unsigned int orientation)
{
        //set emtpty queue
        list<Node> queue;

        //clear the visited3D block
        clearVistedArray();

        // add the start node
        queue.push_back(start);

        //counter for checking if we need to stop processing
        int counter = 0;


        while (!queue.empty())
        {
                Node n = queue.back();
                queue.pop_back();
                if (predicate3D(n, minThreshold, maxThreshold))
                {
                        switch(orientation)
                        {
                            case vtkImageViewer2::SLICE_ORIENTATION_XY:
                                queue.push_back(Node(n.pos_x-1, n.pos_y, n.pos_z));
                                queue.push_back(Node(n.pos_x+1, n.pos_y, n.pos_z));
                                queue.push_back(Node(n.pos_x, n.pos_y-1, n.pos_z));
                                queue.push_back(Node(n.pos_x, n.pos_y+1, n.pos_z));
                            break;

                            case vtkImageViewer2::SLICE_ORIENTATION_XZ:
                                queue.push_back(Node(n.pos_x-1, n.pos_y, n.pos_z));
                                queue.push_back(Node(n.pos_x+1, n.pos_y, n.pos_z));
                                queue.push_back(Node(n.pos_x, n.pos_y, n.pos_z +1));
                                queue.push_back(Node(n.pos_x, n.pos_y, n.pos_z -1));
                            break;

                            case vtkImageViewer2::SLICE_ORIENTATION_YZ:
                                queue.push_back(Node(n.pos_x, n.pos_y +1, n.pos_z));
                                queue.push_back(Node(n.pos_x, n.pos_y -1, n.pos_z));
                                queue.push_back(Node(n.pos_x, n.pos_y, n.pos_z +1));
                                queue.push_back(Node(n.pos_x, n.pos_y, n.pos_z -1));
                            break;

                            default:
                                qWarning() << "Segmenger : Orientation not supported!";
                        }
                }

                counter++;
                //check every so often to see if we need to abort
                if (counter % 10000 == 0)
                {
                        //check if we have been asked to cancel
                        QMutexLocker locker(&mutex);

                        if(task==SLEEP || abort)
                        {
                            qDebug() << "doSegmentationIter2D_I() aborting...";
                            return;
                        }

                        counter = 0;
                }
        }
}

void Segmenter::doSegmentationIter3D_I(Node start, int minThreshold, int maxThreshold)
{
        //set emtpty queue
        list<Node> queue;

        //clear the visited3D block
        clearVistedArray();

        // add the start node
        queue.push_back(start);

        //counter for checking if we need to stop processing
        int counter = 0;

        while (!queue.empty())
        {
                Node n = queue.back();
                queue.pop_back();
                if (predicate3D(n, minThreshold, maxThreshold))
                {
                        queue.push_back(Node(n.pos_x-1, n.pos_y, n.pos_z));
                        queue.push_back(Node(n.pos_x+1, n.pos_y, n.pos_z));
                        queue.push_back(Node(n.pos_x, n.pos_y-1, n.pos_z));
                        queue.push_back(Node(n.pos_x, n.pos_y+1, n.pos_z));
                        queue.push_back(Node(n.pos_x, n.pos_y, n.pos_z-1));
                        queue.push_back(Node(n.pos_x, n.pos_y, n.pos_z+1));

                }

                counter++;
                //check every so often if we need to abort
                if (counter % 10000 == 0)
                {
                        //check if we have been asked to cancel
                        QMutexLocker locker(&mutex);

                        if(task==SLEEP || abort)
                        {
                            qDebug() << "doSegmentationIter3D_I() aborting...";
                            return;
                        }

                        counter = 0;
                }
        }
}


bool Segmenter::predicate3D(Node& node, int minThreshold, int maxThreshold)
{
        //Check if Node is in the current boundary
        if(!boundaryManager->isInBoundary(node.pos_x, node.pos_y, node.pos_z))
                return false;

        char* pixel_segmentation = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(node.pos_x, node.pos_y, node.pos_z));
        char pixel_visited = visited3D[node.pos_x][node.pos_y][node.pos_z];
        short* pixel_original = static_cast<short*>(imagePairManager->original->GetScalarPointer(node.pos_x, node.pos_y, node.pos_z));

        if (pixel_visited == 1)
                return false;//We've already visited this voxel
        if ((char)pixel_segmentation[0] == imagePairManager->BLOCKING)
                return false;
        if ((short)pixel_original[0] < minThreshold || (short)pixel_original[0] > maxThreshold)
                return false;

        //otherwise mark as visited
        visited3D[node.pos_x][node.pos_y][node.pos_z] = 1;

        //update segmentation results
        pixel_segmentation[0] = imagePairManager->SEGMENTATION;
        return true;
}

int Segmenter::contains_segmentation(int pos_x, int pos_y, int pos_z, Morphology type, Kernel kernel)
{
        int output = 0;
        char* pixels[8];
        int count = 4;
        pixels[0] = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x+1, pos_y, pos_z));
        pixels[1] = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x-1, pos_y, pos_z));
        pixels[2] = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x, pos_y+1, pos_z));
        pixels[3] = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x, pos_y-1, pos_z));
        if (kernel == SQUARE)
        {
                pixels[4] = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x+1, pos_y+1, pos_z));
                pixels[5] = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x-1, pos_y+1, pos_z));
                pixels[6] = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x+1, pos_y-1, pos_z));
                pixels[7] = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x-1, pos_y-1, pos_z));
                count = 8;
        }

        for (int i=0; i<count; i++)
        {
                if ((char)pixels[i][0] == imagePairManager->SEGMENTATION)
                {
                        output++;
                        if (type == DILATE) return output;
                }
        }

        return output;
}

void Segmenter::clearVistedArray()
{
        //fill visited3D with zeros
        for (int i=0; i<img_x; i++)
                for (int j=0; j<img_y; j++)
                        for (int k=0; k<img_z; k++)
                                visited3D[i][j][k] = 0;
}

void Segmenter::dilate(int pos_z, Kernel kernel)
{
        //array with pixels to be changed
        char** modified = new char*[img_x];
        for (int i=0; i<img_x; i++)
                modified[i] = new char[img_y];

        //fill with BACKGROUNG
        for (int i=0; i<img_x; i++)
                for (int j=0; j<img_y; j++)
                        modified[i][j] = imagePairManager->BACKGROUND;

        //mark pixels for segmentation
        for (int pos_x=1; pos_x < img_x-1; pos_x++)
        {
                for (int pos_y=1; pos_y < img_y-1; pos_y++)
                {
                        char* pixel = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x, pos_y, pos_z));
                        if ((char)pixel[0] == imagePairManager->BACKGROUND)
                        {
                                int neighbours = contains_segmentation(pos_x, pos_y, pos_z, DILATE, kernel);
                                if (neighbours >= 1)
                                {
                                        modified[pos_x][pos_y] = imagePairManager->SEGMENTATION;
                                }
                        }

                }
        }

        //copy the results
        for (int pos_x=0; pos_x < img_x; pos_x++)
        {
                for (int pos_y=0; pos_y < img_y; pos_y++)
                {
                        if ((int)modified[pos_x][pos_y] == imagePairManager->SEGMENTATION)
                        {
                                char* pixel = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x, pos_y, pos_z));
                                pixel[0] = imagePairManager->SEGMENTATION;
                        }
                }
        }

        //relese the memorey
        for (int i=0; i<img_x; i++)
                delete[] modified[i];
        delete[] modified;
}

void Segmenter::erode(int pos_z, Kernel kernel)
{
        //array with pixels to be changed
        char** modified = new char*[img_x];
        for (int i=0; i<img_x; i++)
                modified[i] = new char[img_y];

        //fill with SEGMENTATION
        for (int i=0; i<img_x; i++)
                for (int j=0; j<img_y; j++)
                        modified[i][j] = imagePairManager->SEGMENTATION;

        //mark pixels for removal
        for (int pos_x=1; pos_x < img_x-1; pos_x++)
        {
                for (int pos_y=1; pos_y < img_y-1; pos_y++)
                {
                        char* pixel = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x, pos_y, pos_z));
                        if ((char)pixel[0] == imagePairManager->SEGMENTATION)
                        {
                                int neighbours = contains_segmentation(pos_x, pos_y, pos_z, ERODE, kernel);
                                int count = 4;
                                if (kernel == SQUARE) count = 8;
                                if (neighbours != count)
                                {
                                        modified[pos_x][pos_y] = imagePairManager->BACKGROUND;
                                }
                        }

                }
        }

        //copy the results
        for (int pos_x=0; pos_x < img_x; pos_x++)
        {
                for (int pos_y=0; pos_y < img_y; pos_y++)
                {
                        if ((int)modified[pos_x][pos_y] == imagePairManager->BACKGROUND)
                        {
                                char* pixel = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x, pos_y, pos_z));
                                pixel[0] = imagePairManager->BACKGROUND;
                        }
                }
        }

        //relese the memorey
        for (int i=0; i<img_x; i++)
                delete[] modified[i];
        delete[] modified;
}

void Segmenter::doMorphClose(int pos_z)
{
        //assign kernel type
        Kernel kernel;
        if (kernelType->currentText() == "Cross")
                kernel = CROSS;
        else if (kernelType->currentText() == "Square")
                kernel = SQUARE;

        //run algorithms
        dilate(pos_z, kernel);
        erode(pos_z, kernel);

        //signal that we're complete
        imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
        emit segmentationDone();
}

void Segmenter::doMorphOpen(int pos_z)
{
        //assign kernel type
        Kernel kernel;
        if (kernelType->currentText() == "Cross")
                kernel = CROSS;
        else if (kernelType->currentText() == "Square")
                kernel = SQUARE;

        //run algorithms
        erode(pos_z, kernel);
        dilate(pos_z, kernel);

        //signal that we're complete
        imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
        emit segmentationDone();
}

void Segmenter::doDilate(int pos_z)
{
        //assign kernel type
        Kernel kernel;
        if (kernelType->currentText() == "Cross")
                kernel = CROSS;
        else if (kernelType->currentText() == "Square")
                kernel = SQUARE;

        //run algorithm
        dilate(pos_z, kernel);

        //signal that we're complete
        imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
        emit segmentationDone();
}

void Segmenter::doErode(int pos_z)
{
        //assign kernel type
        Kernel kernel;
        if (kernelType->currentText() == "Cross")
                kernel = CROSS;
        else if (kernelType->currentText() == "Square")
                kernel = SQUARE;

        //run algorithm
        erode(pos_z, kernel);

        //signal that we're complete
        imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
        emit segmentationDone();
}


void Segmenter::cancel()
{
      qDebug() << "Segmenter::cancel() : Requesting cancel";
      QMutexLocker locker(&mutex);

      task=SLEEP;
}












