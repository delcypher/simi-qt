#include "segmenter.h"
#include <QDebug>


Segmenter::Segmenter(SeedPointManager* seedPointManager, ImagePairManager* imagePairManager)
{
    this->imagePairManager=imagePairManager;
}

Segmenter::~Segmenter()
{

}

void Segmenter::doSegmentation(int sliceNumber, int minThreshold, int maxThreshold)
{
	qDebug() << "Segmenter::doSegmentation(" << sliceNumber << "," << minThreshold << "," << maxThreshold << ")";

	//do something

	//signal that we're complete
	imagePairManager->segblock->Modified(); //Mark the segblock as modified so VTK know's to trigger an update along the pipline
	emit segmentationDone(sliceNumber);
}
