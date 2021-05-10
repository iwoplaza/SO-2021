#include "region.h"
#include <stdlib.h>

void regionset_init(RegionSet_t* set)
{
    set->regions_capacity = 16;
    set->regions_count = 0;
    set->regions = malloc(sizeof(Region_t) * set->regions_capacity);
}

void regionset_add(RegionSet_t* set, int y0, int y1, int width)
{
    // Resizing the regions array if necessary.
    if (set->regions_count >= set->regions_capacity)
    {
        set->regions_capacity += 16;
        set->regions = realloc(set->regions, sizeof(Region_t) * set->regions_capacity);
    }

    set->regions[set->regions_count].y0 = y0;
    set->regions[set->regions_count].y1 = y1;
    set->regions[set->regions_count].width = width;

    set->regions_count++;
}
