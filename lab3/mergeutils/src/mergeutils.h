#ifndef MERGE_UTILS_H
#define MERGE_UTILS_H

#define MULIB_SUCCESS 0
#define MULIB_A_NOT_FOUND 1
#define MULIB_B_NOT_FOUND 2
#define MULIB_OUT_OF_RANGE 3

typedef struct MUMergeRow_t {
    char* content;
    int capacity;
} MUMergeRow_t;

typedef struct MUMergeBlock_t {
    MUMergeRow_t* rows;
    int amount;
    int capacity;
} MUMergeBlock_t;

typedef struct MUMergeTable_t {
    MUMergeBlock_t* blocks;
    int amount;
    int capacity;
} MUMergeTable_t;

/**
 * Creates an empty table used for merging.
 */
MUMergeTable_t* mu_init_merge_table();

void mu_free_table(MUMergeTable_t* table);

/**
 * Creates a new batch, interweaving lines of the input files.
 */
int mu_merge_files(MUMergeTable_t* table, const char* filepath_a, const char* filepath_b);

int mu_remove_block(MUMergeTable_t* table, int block_index);

int mu_remove_row(MUMergeTable_t* table, int block_index, int row_index);

int mu_get_num_of_blocks(const MUMergeTable_t* table);

int mu_get_num_of_rows(const MUMergeTable_t* table, int block_index);

void mu_print_block(const MUMergeTable_t* table, int block_index);

int mu_write_block_to_file(const MUMergeTable_t* table, int block_index, const char* filename);

#endif // MERGE_UTILS_H