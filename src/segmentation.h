#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <vtkSmartPointer.h>
#include <vtkImageData.h>

typedef vtkSmartPointer<vtkImageData> ptrImg;
typedef bool (*isValid)(ptrImg, ptrImg, int, int, int, int, int);

/* Flood fill algorithm */
void flood_fill(ptrImg original, ptrImg visited, int pos_x, int pos_y, int pos_z, int range_from, int range_to, isValid predicate);

/* Logical predicates */
bool predicate1(ptrImg original, ptrImg visited, int pos_x, int pos_y, int pos_z, int range_from, int range_to);

#endif
