#include <QObject>
#include "imagepairmanager.h"

class DrawManager : public QObject
{
	Q_OBJECT

	public:
		DrawManager(ImagePairManager* imageManager);
		~DrawManager();
	
	private:
		ImagePairManager* imageManager;
	
};
