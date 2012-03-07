#include "segmenter.h"
#include <QDebug>


Segmenter::Segmenter(SeedPointManager* seedPointManager, ImagePairManager* imagePairManager)
{
    this->imagePairManager=imagePairManager;        

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
        if (pos_x < 0 || pos_x == 512 || pos_y < 0 || pos_y == 512 || pos_z < 0 || pos_z == 120)
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
        }
}

bool Segmenter::predicate2D(Node& node, int minThreshold, int maxThreshold)
{
        if (node.pos_x < 0 || node.pos_x == 512 || node.pos_y < 0 || node.pos_y == 512)
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
        if (node.pos_x < 0 || node.pos_x == 512 || node.pos_y < 0 || node.pos_y == 512 || node.pos_z < 0 || node.pos_z == 120)
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
