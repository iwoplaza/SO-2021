#ifndef LAB8_REGION_H
#define LAB8_REGION_H

typedef struct Region_t {
    int y0;
    int y1;
    int width;
} Region_t;

typedef struct RegionSet_t {
    Region_t* regions;
    int regions_count;
    int regions_capacity;
} RegionSet_t;

/**
 * Initializes the passed in set.
 * @param set The set to initialize.
 */
void regionset_init(RegionSet_t* set);

/**
 * Adds the passed in region to the region set.
 * @param set The set to add the region to.
 * @param region The region to add.
 */
void regionset_add(RegionSet_t* set, Region_t* region);

#endif //LAB8_REGION_H