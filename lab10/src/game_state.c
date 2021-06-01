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