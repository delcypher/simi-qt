/*! \file seedpointmanager.h */

//!  SeedPointManager.


#ifndef SEEDPOINTMANAGER_H
#define SEEDPOINTMANAGER_H

#include <QObject>
#include <vtkEvent.h>
#include <vtkObject.h>
#include <vector>

using std::vector;

//forward declare
class SeedPointManagerTester;

/*!
    SeedPointManager is the class responsible for managing the seed points of all slices of an image.
*/
class SeedPointManager : public QObject
{
	Q_OBJECT
	public:
        //! The SeedPointManager constructor.
            /*!
                The constructor takes as input the number of slices of an image.
            */
		SeedPointManager(int numberOfSlices);

        //! The default SeedPointManager destructor.
        ~SeedPointManager();
	
        //! The function gets the seed point for slice "sliceNumber". If no seed is currently set it reports (-1,-1).
            /*!
                \param sliceNumber is the number (integer) of the current slice.
                \param x is the x coordinate (integer) of the seed point passed by reference.
                \param y is the y coordinate (integer) of the seed point passed by reference.
                \return True if the seed point is already set, false otherwise.
            */
		bool getSeedPoint(int sliceNumber, int& x, int& y);

	signals:
        //! Functions that signals to other classes that the seed point changed.
            /*!
                \param sliceNumber is the number (integer) of the current slice.
                \param x is the x coordinate (integer) of the seed point passed by reference.
                \param y is the y coordinate (integer) of the seed point passed by reference.
            */
		void seedPointChanged(int sliceNumber, int x, int y);


	public slots:
        //! Function that allows the seed point to be manually set and informs other classes.
            /*!
                \param x is the x coordinate (integer) of the seed point passed by reference.
                \param y is the y coordinate (integer) of the seed point passed by reference.
                \param sliceNumber is the number (integer) of the current slice.
                \return Returns true when the seed point is set.
            */
		bool setSeedPoint(int x, int y, int sliceNumber);


	private:
        //! A private member keeps the number of all slices of the image.
		int numberOfSlices;

        //! An array with two columns (x,y) that keeps track of all seed points per slice.
        /*!
            If a slice has no seed point set, the coordinates of this seed point are set to (-1,-1).
        */
		vector< vector<int> > seedPerSlice;

	friend class SeedPointManagerTester;
};

#endif
