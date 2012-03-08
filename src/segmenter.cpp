#include "segmenter.h"
#include <QDebug>
#include <QApplication>


Segmenter::Segmenter(SeedPointManager* seedPointManager, ImagePairManager* imagePairManager, QComboBox* kernelType)
{
        this->imagePairManager = imagePairManager;
        this->kernelType = kernelType;

        int* dims = imagePairManager->original->GetDimensions();
        img_x = dims[0];
        img_y = dims[1];
        img_z = dims[2];

        qDebug() << "Constructing segmenter: " << img_x << " " << img_y << " " << img_z;
}

Segmenter::~Segmenter()
{

}

void Segmenter::doSegmentation2D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold)
{
        qDebug() << "Segmenter::doSegmentation2D(" << pos_z << "," << minThreshold << "," << maxThreshold << ")";

        //run algorithm
        //doSegmentation2D_I(pos_x, pos_y, pos_z, minThreshold, maxThreshold);
        doSegmentationIter2D_I(Node(pos_x, pos_y, pos_z), minThreshold, maxThreshold);

        //signal that we're complete
        imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
        emit segmentationDone(pos_z);
}

void Segmenter::doSegmentation3D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold)
{
        qDebug() << "Segmenter::doSegmentation3D(" << pos_z << "," << minThreshold << "," << maxThreshold << ")";

        //run algorithm
        //doSegmentation3D_I(pos_x, pos_y, pos_z, minThreshold, maxThreshold);
        doSegmentationIter3D_I(Node(pos_x, pos_y, pos_z), minThreshold, maxThreshold);

        //signal that we're complete
        imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
        emit segmentationDone(pos_z);
}

bool Segmenter::predicate2D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold)
{
        if (pos_x < 0 || pos_x == 512 || pos_y < 0 || pos_y == 512)
                return false;

        char* pixel_visited = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x,pos_y,pos_z));
        short* pixel_original = static_cast<short*>(imagePairManager->original->GetScalarPointer(pos_x,pos_y,pos_z));

        if ((char)pixel_visited[0] == imagePairManager->SEGMENTATION)
                return false;
        if ((char)pixel_visited[0] == imagePairManager->BLOCKING)
                return false;
        if ((short)pixel_original[0] < minThreshold || (short)pixel_original[0] > maxThreshold)
                return false;

        //otherwise mark as visited
        pixel_visited[0] = imagePairManager->SEGMENTATION;
        return true;
}

bool Segmenter::predicate3D(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold)
{
        if (pos_x < 0 || pos_x == img_x || pos_y < 0 || pos_y == img_y || pos_z < 0 || pos_z == img_z)
                return false;

        char* pixel_visited = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(pos_x,pos_y,pos_z));
        short* pixel_original = static_cast<short*>(imagePairManager->original->GetScalarPointer(pos_x,pos_y,pos_z));

        if ((char)pixel_visited[0] == imagePairManager->SEGMENTATION)
                return false;
        if ((char)pixel_visited[0] == imagePairManager->BLOCKING)
                return false;
        if ((short)pixel_original[0] < minThreshold || (short)pixel_original[0] > maxThreshold)
                return false;

        //otherwise mark as visited
        pixel_visited[0] = imagePairManager->SEGMENTATION;
        return true;
}

void Segmenter::doSegmentation2D_I(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold)
{
        if (!predicate2D(pos_x, pos_y, pos_z, minThreshold, maxThreshold)) return;

        //up
        doSegmentation2D_I(pos_x, pos_y-1, pos_z, minThreshold, maxThreshold);

        //right
        doSegmentation2D_I(pos_x+1, pos_y, pos_z, minThreshold, maxThreshold);

        //down
        doSegmentation2D_I(pos_x, pos_y+1, pos_z, minThreshold, maxThreshold);

        //left
        doSegmentation2D_I(pos_x-1, pos_y, pos_z, minThreshold, maxThreshold);
}

void Segmenter::doSegmentation3D_I(int pos_x, int pos_y, int pos_z, int minThreshold, int maxThreshold)
{
        if (!predicate3D(pos_x, pos_y, pos_z, minThreshold, maxThreshold)) return;

        //up
        doSegmentation3D_I(pos_x, pos_y-1, pos_z, minThreshold, maxThreshold);

        //right
        doSegmentation3D_I(pos_x+1, pos_y, pos_z, minThreshold, maxThreshold);

        //down
        doSegmentation3D_I(pos_x, pos_y+1, pos_z, minThreshold, maxThreshold);

        //left
        doSegmentation3D_I(pos_x-1, pos_y, pos_z, minThreshold, maxThreshold);

        //up slice
        doSegmentation3D_I(pos_x, pos_y, pos_z+1, minThreshold, maxThreshold);

        //down slice
        doSegmentation3D_I(pos_x, pos_y, pos_z-1, minThreshold, maxThreshold);
}

void Segmenter::doSegmentationIter2D_I(Node start, int minThreshold, int maxThreshold)
{
        //set emtpty queue
        list<Node> queue;

        // add the start node
        queue.push_back(start);

        while (!queue.empty())
        {
                Node n = queue.back();
                queue.pop_back();
                if (predicate2D(n, minThreshold, maxThreshold))
                {
                        queue.push_back(Node(n.pos_x-1, n.pos_y, n.pos_z));
                        queue.push_back(Node(n.pos_x+1, n.pos_y, n.pos_z));
                        queue.push_back(Node(n.pos_x, n.pos_y-1, n.pos_z));
                        queue.push_back(Node(n.pos_x, n.pos_y+1, n.pos_z));
                }
        }
}

void Segmenter::doSegmentationIter3D_I(Node start, int minThreshold, int maxThreshold)
{
        //set emtpty queue
        list<Node> queue;

        // add the start node
        queue.push_back(start);

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
		QApplication::processEvents();
        }
}

bool Segmenter::predicate2D(Node& node, int minThreshold, int maxThreshold)
{
        if (node.pos_x < 0 || node.pos_x == img_x || node.pos_y < 0 || node.pos_y == img_y)
                return false;

        char* pixel_visited = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(node.pos_x, node.pos_y, node.pos_z));
        short* pixel_original = static_cast<short*>(imagePairManager->original->GetScalarPointer(node.pos_x, node.pos_y, node.pos_z));

        if ((char)pixel_visited[0] == imagePairManager->SEGMENTATION)
                return false;
        if ((char)pixel_visited[0] == imagePairManager->BLOCKING)
                return false;
        if ((short)pixel_original[0] < minThreshold || (short)pixel_original[0] > maxThreshold)
                return false;

        //otherwise mark as visited
        pixel_visited[0] = imagePairManager->SEGMENTATION;
        return true;
}

bool Segmenter::predicate3D(Node& node, int minThreshold, int maxThreshold)
{
        if (node.pos_x < 0 || node.pos_x == img_x || node.pos_y < 0 || node.pos_y == img_y || node.pos_z < 0 || node.pos_z == img_z)
                return false;

        char* pixel_visited = static_cast<char*>(imagePairManager->segblock->GetScalarPointer(node.pos_x, node.pos_y, node.pos_z));
        short* pixel_original = static_cast<short*>(imagePairManager->original->GetScalarPointer(node.pos_x, node.pos_y, node.pos_z));

        if ((char)pixel_visited[0] == imagePairManager->SEGMENTATION)
                return false;
        if ((char)pixel_visited[0] == imagePairManager->BLOCKING)
                return false;
        if ((short)pixel_original[0] < minThreshold || (short)pixel_original[0] > maxThreshold)
                return false;

        //otherwise mark as visited
        pixel_visited[0] = imagePairManager->SEGMENTATION;
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
        char** modified = new char*[512];
        for (int i=0; i<512; i++)
                modified[i] = new char[512];

        //fill with BACKGROUNG
        for (int i=0; i<512; i++)
                for (int j=0; j<512; j++)
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
        for (int i=0; i<512; i++)
                delete[] modified[i];
        delete[] modified;
}

void Segmenter::erode(int pos_z, Kernel kernel)
{
        //array with pixels to be changed
        char** modified = new char*[512];
        for (int i=0; i<512; i++)
                modified[i] = new char[512];

        //fill with SEGMENTATION
        for (int i=0; i<512; i++)
                for (int j=0; j<512; j++)
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
        for (int i=0; i<512; i++)
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
        emit segmentationDone(pos_z);
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
        emit segmentationDone(pos_z);
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
        emit segmentationDone(pos_z);
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
        emit segmentationDone(pos_z);
}












