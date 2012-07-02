#include "segmenter.h"
#include <QDebug>
#include <QApplication>
#include "vtkImageViewer2.h"


Segmenter::Segmenter(ImagePairManager* imagePairManager, BoundaryManager* boundaryManager, QComboBox* kernelType)
{
        this->imagePairManager = imagePairManager;
        this->boundaryManager = boundaryManager;
        this->kernelType = kernelType;

        segmentation_running = false;

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
        for (int i=0; i<img_x; i++)
                for (int j=0; j<img_y; j++)
                        for (int k=0; k<img_z; k++)
                                visited3D[i][j][k] = 0;

        qDebug() << "Constructing segmenter: " << img_x << " " << img_y << " " << img_z;
}

Segmenter::~Segmenter()
{
        //release memory for 3D visited block
        for (int i=0; i<img_x; i++)
                for (int j=0; j<img_y; j++)
                        delete[] visited3D[i][j];
        for (int i=0; i<img_x; i++)
                delete[] visited3D[i];
        delete[] visited3D;
}

void Segmenter::doSegmentation2D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold, unsigned int orientation)
{
    qDebug() << "Segmenter::doSegmentation2D(" << pos_z << "," << minThreshold << "," << maxThreshold << ")";


    //run algorithm
    doSegmentationIter2D_I(Node(pos_x, pos_y, pos_z), minThreshold, maxThreshold, orientation);

    //signal that we're complete
    imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
    emit segmentationDone();
}

void Segmenter::doSegmentation3D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold)
{
    qDebug() << "Segmenter::doSegmentation3D(" << pos_z << "," << minThreshold << "," << maxThreshold << ")";


    //enable event flag
    segmentation_running = true;

    //run algorithm
    doSegmentationIter3D_I(Node(pos_x, pos_y, pos_z), minThreshold, maxThreshold);

    //signal that we're complete
    imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
    emit segmentationDone();

    //disable event flag
    segmentation_running = false;
}

void Segmenter::doSegmentationIter2D_I(Node start, int minThreshold, int maxThreshold, unsigned int orientation)
{
        //set emtpty queue
        list<Node> queue;

        //clear the visited3D block
        while(!visited3D_list.empty())
        {
                Node n = visited3D_list.back();
                visited3D[n.pos_x][n.pos_y][n.pos_z] = 0;
                visited3D_list.pop_back();
        }

        // add the start node
        queue.push_back(start);

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
        }
}

void Segmenter::doSegmentationIter3D_I(Node start, int minThreshold, int maxThreshold)
{
        //set emtpty queue
        list<Node> queue;

        //clear the visited3D block
        while(!visited3D_list.empty())
        {
                Node n = visited3D_list.back();
                visited3D[n.pos_x][n.pos_y][n.pos_z] = 0;
                visited3D_list.pop_back();
        }

        // add the start node
        queue.push_back(start);

        //counter for the event loop
        int counter = 0;

        //process events
        QApplication::processEvents();

        while (!queue.empty() && segmentation_running)
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
                //dirty hack to prevent GUI from freezing
                if (counter % 10000 == 0)
                {
                        QApplication::processEvents();
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
        visited3D_list.push_back(Node(node.pos_x, node.pos_y, node.pos_z));

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

bool Segmenter::isWorking()
{
        return segmentation_running;
}

bool Segmenter::cancel3D()
{
        if (segmentation_running)
        {
                segmentation_running = false;
                return true;
        }
        else
                return false;
}












