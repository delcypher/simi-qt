#include "segmentation.h"

void flood_fill(ptrImg original, ptrImg visited, int pos_x, int pos_y, int range_from, int range_to, isValid predicate)
{
        if (!predicate(original, visited, pos_x, pos_y, range_from, range_to)) return;

        //up
        flood_fill(original,visited, pos_x, pos_y-1, range_from, range_to, predicate);

        //right
        flood_fill(original,visited, pos_x+1, pos_y, range_from, range_to, predicate);

        //down
        flood_fill(original,visited, pos_x, pos_y+1, range_from, range_to, predicate);

        //left
        flood_fill(original,visited, pos_x-1, pos_y, range_from, range_to, predicate);
}

bool predicate1(ptrImg original, ptrImg visited, int pos_x, int pos_y, int range_from, int range_to)
{
        return true;
}
