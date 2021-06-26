#include <stdio.h>
#include "game_state.h"

void reset_game_state(GameState_t* game_state)
{
    for (int i = 0; i < 9; ++i)
    {
        game_state->fields[i] = FIELD_EMPTY;
    }
}

void serialize_game_state(char* buffer, const GameState_t* state)
{
    for (int i = 0; i < 9; ++i)
    {
        buffer[i] = '0' + state->fields[i];
    }

    buffer[9] = '\0';
}

void deserialize_game_state(const char* buffer, GameState_t* state)
{
    for (int i = 0; i < 9; ++i)
    {
        state->fields[i] = buffer[i] - '0';
    }
}

void print_game_state(GameState_t* state)
{
    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            FieldValue_t value = state->fields[x + y * 3];
            char field_rep = (value == FIELD_CROSS) ? 'X' : (value == FIELD_NOUGHT ? 'O' : ' ');

            if (x < 2)
            {
                printf("%c|", field_rep);
            }
            else
            {
                printf("%c\n", field_rep);
            }
        }

        if (y < 2)
        {
            // Horizontal Divider
            for (int i = 0; i < 5; ++i)
                printf("-");
            printf("\n");
        }
    }
}