#include "mergeutils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define _BUFFER_SIZE 256
static char BUFFER[_BUFFER_SIZE];

/**
 * Returns the length of a line starting at the start of the buffer.
 * If the line doesn't end, the length returned will equal the buffer_size.
 * @param buffer
 * @param buffer_size
 * @return
 */
static int get_line_length(char *buffer, int buffer_size)
{
    int length = 0;

    while (length < buffer_size && buffer[length] != '\n')
        length++;

    return length;
}

static MUMergeRow_t *add_row(MUMergeBlock_t *block)
{
    int index = block->amount;
    block->amount++;

    if (block->amount > block->capacity)
    {
        // Resizing the storage.
        block->capacity *= 2;
        block->rows = realloc(block->rows, sizeof(MUMergeRow_t) * block->capacity);
    }

    MUMergeRow_t *row = &block->rows[index];
    row->capacity = 8;
    row->content = calloc(row->capacity, sizeof(char));
    return row;
}

static void resize_row(MUMergeRow_t *row, int length)
{
    if (length >= row->capacity)
    {
        row->capacity = length + 1;
        row->content = realloc(row->content, sizeof(char) * row->capacity);
    }
}

static MUMergeBlock_t* read_block(const char *filepath)
{
    FILE* file = fopen(filepath, "r");

    if (file == NULL)
    {
        return NULL;
    }

    MUMergeBlock_t* block = malloc(sizeof(MUMergeBlock_t));
    block->capacity = 8;
    block->amount = 0;
    block->rows = calloc(block->capacity, sizeof(MUMergeRow_t));

    char* buffer = BUFFER;
    int bytes_left = fread(buffer, sizeof(char), _BUFFER_SIZE, file);

    while (bytes_left > 0)
    {
        int content_length = 0;
        MUMergeRow_t* row = add_row(block);

        while (true)
        {
            int subline_length = get_line_length(buffer, bytes_left);

            resize_row(row, content_length + subline_length);
            memcpy(row->content + content_length, buffer, subline_length);

            content_length += subline_length;

            if (subline_length == bytes_left)
            {
                // This is not the end of the line. (Might be the end of the file)
                buffer = BUFFER;
                bytes_left = fread(buffer, sizeof(char), _BUFFER_SIZE, file);

                // Is end of the file
                if (bytes_left == 0) {
                    break;
                }
            }
            else
            {
                buffer += subline_length + 1;
                bytes_left -= subline_length + 1;
                break;
            }
        }

        row->content[content_length] = '\0';

        if (bytes_left == 0)
        {
            // Refilling the buffer
            buffer = BUFFER;
            bytes_left = fread(buffer, sizeof(char), _BUFFER_SIZE, file);
        }
    }

    fclose(file);

    return block;
}

MUMergeTable_t *mu_init_merge_table()
{
    MUMergeTable_t *table = malloc(sizeof(MUMergeTable_t));

    table->capacity = 4;
    table->amount = 0;
    table->blocks = calloc(table->capacity, sizeof(MUMergeBlock_t));

    return table;
}

void free_row(MUMergeRow_t *row, bool free_content)
{
    if (free_content)
        free(row->content);
}

void free_block(MUMergeBlock_t *block, bool free_content)
{
    for (int r = 0; r < block->amount; ++r)
    {
        free_row(&block->rows[r], free_content);
    }

    free(block->rows);
}

void mu_free_table(MUMergeTable_t *table)
{
    for (int b = 0; b < table->amount; ++b)
    {
        free_block(&table->blocks[b], true);
    }

    free(table->blocks);
    free(table);
}

/**
 * Creates a new batch, interweaving lines of the input files.
 */
int mu_merge_files(MUMergeTable_t *table, const char *filepath_a, const char *filepath_b)
{
    MUMergeBlock_t* block_a = read_block(filepath_a);
    MUMergeBlock_t* block_b = read_block(filepath_b);

    if (block_a == NULL || block_b == NULL)
    {
        // We don't free the content, because it's been copied to the main table.
        if (block_a != NULL)
        {
            free_block(block_a, false);
            free(block_a);
        }

        if (block_b != NULL)
        {
            free_block(block_b, false);
            free(block_b);
        }

        return block_a == NULL ? MULIB_A_NOT_FOUND : MULIB_B_NOT_FOUND;
    }

    int index = table->amount;
    table->amount++;

    if (table->amount >= table->capacity)
    {
        // Reserving more space
        table->capacity *= 2;
        table->blocks = realloc(table->blocks, table->capacity * sizeof(MUMergeBlock_t));
    }

    // Adding a new block to the table
    MUMergeBlock_t *block = &table->blocks[index];
    block->amount = 0;
    block->capacity = block_a->amount + block_b->amount;
    block->rows = calloc(block->capacity, sizeof(MUMergeRow_t));

    int block_a_index = 0;
    int block_b_index = 0;

    while (block_a_index < block_a->amount || block_b_index < block_b->amount)
    {
        if (block_a_index < block_a->amount)
        {
            block->rows[block->amount++] = block_a->rows[block_a_index++];
        }

        if (block_b_index < block_b->amount)
        {
            block->rows[block->amount++] = block_b->rows[block_b_index++];
        }
    }

    // We don't free the content, because it's been copied to the main table.
    if (block_a != NULL)
    {
        free_block(block_a, false);
        free(block_a);
    }

    if (block_b != NULL)
    {
        free_block(block_b, false);
        free(block_b);
    }
    return MULIB_SUCCESS;
}

int mu_remove_block(MUMergeTable_t *table, int block_index)
{
    if (block_index < 0 || block_index >= table->amount)
    {
        return MULIB_OUT_OF_RANGE;
    }

    free_block(&table->blocks[block_index], true);

    // Moving all blocks after the index one index back.
    for (int i = block_index + 1; i < table->amount; ++i)
    {
        table->blocks[i - 1] = table->blocks[i];
    }

    table->amount--;

    return MULIB_SUCCESS;
}

int mu_remove_row(MUMergeTable_t *table, int block_index, int row_index)
{
    if (block_index < 0 || block_index >= table->amount)
    {
        return MULIB_OUT_OF_RANGE;
    }

    MUMergeBlock_t *block = &table->blocks[block_index];
    if (row_index < 0 || row_index >= block->amount)
    {
        return MULIB_OUT_OF_RANGE;
    }

    free_row(&block->rows[row_index], true);

    // Moving all rows after the index one index back.
    for (int i = row_index + 1; i < block->amount; ++i)
    {
        block->rows[i - 1] = block->rows[i];
    }

    block->amount--;

    return MULIB_SUCCESS;
}

int mu_get_num_of_blocks(const MUMergeTable_t *table)
{
    return table->amount;
}

int mu_get_num_of_rows(const MUMergeTable_t *table, int block_index)
{
    if (block_index < 0 || block_index >= table->amount)
    {
        return MULIB_OUT_OF_RANGE;
    }

    MUMergeBlock_t *block = &table->blocks[block_index];
    return block->amount;
}

void mu_print_block(const MUMergeTable_t *table, int block_index)
{
    if (block_index < 0 || block_index >= table->amount)
    {
        return;
    }

    MUMergeBlock_t *block = &table->blocks[block_index];
    for (int i = 0; i < block->amount; ++i)
    {
        MUMergeRow_t* row = &block->rows[i];
        printf("%s\n", row->content);
    }
}

int mu_write_block_to_file(const MUMergeTable_t *table, int block_index, const char *filename)
{
    if (block_index < 0 || block_index >= table->amount)
    {
        return MULIB_OUT_OF_RANGE;
    }

    FILE *file = fopen(filename, "w");

    MUMergeBlock_t *block = &table->blocks[block_index];
    for (int i = 0; i < block->amount; ++i)
    {
        MUMergeRow_t *row = &block->rows[i];
        fprintf(file, "%s\n", row->content);
    }

    fclose(file);

    return MULIB_SUCCESS;
}