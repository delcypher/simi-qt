#include "segmentation.h"

void flood_fill(ptrImg original, ptrImg visited, int pos_x, int pos_y, int pos_z, int range_from, int range_to, isValid predicate)
{
        if (!predicate(original, visited, pos_x, pos_y, pos_z, range_from, range_to)) return;

        //up
        flood_fill(original,visited, pos_x, pos_y-1, pos_z, range_from, range_to, predicate);

        //right
        flood_fill(original,visited, pos_x+1, pos_y, pos_z, range_from, range_to, predicate);

        //down
        flood_fill(original,visited, pos_x, pos_y+1, pos_z, range_from, range_to, predicate);

        //left
        flood_fill(original,visited, pos_x-1, pos_y, pos_z, range_from, range_to, predicate);
}

bool predicate1(ptrImg original, ptrImg visited, int pos_x, int pos_y, int pos_z, int range_from, int range_to)
{
        if (pos_x < 0 || pos_x == 512 || pos_y < 0 || pos_y == 512)
                return false;

        unsigned char* pixel_visited = static_cast<unsigned char*>(visited->GetScalarPointer(pos_x,pos_y,0));
        short* pixel_original = static_cast<short*>(original->GetScalarPointer(pos_x,pos_y,pos_z));

        if ((int)pixel_visited[0] == 255)
                return false;
        if ((short)pixel_original[0] < range_from || (short)pixel_original[0] > range_to)
                return false;

        //otherwise mark as visited
        static int counter = 0;
        pixel_visited[0] = 255;
        pixel_visited[3] = 255;
        //cout << counter << " visted" << endl;
        counter++;
        return true;

}
