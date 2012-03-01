#include "segmenter.h"
#include <QDebug>


Segmenter::Segmenter(SeedPointManager* seedManager, ImagePairManager* imageManager)
{

}

Segmenter::~Segmenter()
{

}

void Segmenter::doSegmentation(int sliceNumber, int minThreshold, int maxThreshold)
{
	qDebug() << "Segmenter::doSegmentation(" << sliceNumber << "," << minThreshold << "," << maxThreshold << ")";

	//do something

	//signal that we're complete
	emit segmentationDone(sliceNumber);
}
