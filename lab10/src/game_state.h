#ifndef LAB10_GAME_STATE_H
#define LAB10_GAME_STATE_H

#define STATE_SERIALIZATION_LEN 10

typedef enum FieldValue_t {
    FIELD_EMPTY,
    FIELD_CROSS,
    FIELD_NOUGHT,
} FieldValue_t;

typedef struct GameState_t
{
    enum FieldValue_t fields[9];
} GameState_t;

void reset_game_state(GameState_t* game_state);

void serialize_game_state(char* buffer, const GameState_t* state);

void deserialize_game_state(const char* buffer, GameState_t* state);

void print_game_state(GameState_t* state);

#endif //LAB10_GAME_STATE_H
