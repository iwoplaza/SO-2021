#include "region.h"
#include <stdlib.h>

void regionset_init(RegionSet_t* set)
{
    set->regions_capacity = 16;
    set->regions_count = 0;
    set->regions = malloc(sizeof(Region_t) * set->regions_capacity);
}

void regionset_add(RegionSet_t* set, int x_from, int x_to, int y_from, int y_to)
{
    // Resizing the regions array if necessary.
    if (set->regions_count >= set->regions_capacity)
    {
        set->regions_capacity += 16;
        set->regions = realloc(set->regions, sizeof(Region_t) * set->regions_capacity);
    }

    set->regions[set->regions_count].x_from = x_from;
    set->regions[set->regions_count].x_to = x_to;
    set->regions[set->regions_count].y_from = y_from;
    set->regions[set->regions_count].y_to = y_to;

    set->regions_count++;
}
