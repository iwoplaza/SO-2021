#ifndef LAB8_REGION_H
#define LAB8_REGION_H

typedef struct Region_t {
    int x_from;
    int x_to;
    int y_from;
    int y_to;
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
 * @param x_from The left bound (inclusive)
 * @param x_to The right bound (exclusive)
 * @param y_from The top bound (inclusive)
 * @param y_to The bottom bound (exclusive)
 */
void regionset_add(RegionSet_t* set, int x_from, int x_to, int y_from, int y_to);

#endif //LAB8_REGION_H