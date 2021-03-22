#ifndef LAB1_CLI_CLI_CONTEXT_H
#define LAB1_CLI_CLI_CONTEXT_H

#include <stdio.h>

typedef void* (*InitMergeTableFunction)(void);
typedef void* (*FreeTableFunction)(void* table);
typedef int (*MergeFilesFunction)(void* table, const char* filepath_a, const char* filepath_b);
typedef int (*RemoveBlockFunction)(void* table, int block_index);
typedef int (*RemoveRowFunction)(void* table, int block_index, int row_index);
typedef int (*GetNumOfBlocksFunction)(const void* table);
typedef int (*GetNumOfRowsFunction)(const void* table, int block_index);
typedef void (*PrintBlockFunction)(const void* table, int block_index);
typedef int (*WriteBlockToFileFunction)(const void* table, int block_index, const char* filename);

typedef struct CLIContext_t
{
    void* table;
    int table_size;
    FILE* report_file;

    InitMergeTableFunction init_merge_table;
    FreeTableFunction free_table;
    MergeFilesFunction merge_files;
    RemoveBlockFunction remove_block;
    RemoveRowFunction remove_row;
    GetNumOfBlocksFunction get_num_of_blocks;
    GetNumOfRowsFunction get_num_of_rows;
    PrintBlockFunction print_block;
    WriteBlockToFileFunction write_block_to_file;
} CLIContext_t;

#endif //LAB1_CLI_CLI_CONTEXT_H
