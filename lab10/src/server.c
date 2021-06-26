#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "logs.h"
#include "comm.h"
#include "defs.h"
#include "game_state.h"

#define MAX_INSTANCES 16

void parse_arguments();
void init_game_instances();
bool is_username_taken(const char* username);
int add_player(int player_fd, const char* username);
bool try_to_start_instance(int instance_idx);

void handle_ping_msg(char* msg_buffer, int client_fd);
void handle_join_msg(char* msg_buffer, int client_fd);
void handle_move_msg(char* msg_buffer, int client_fd);
void handle_quit_msg(char* msg_buffer, int client_fd);

int port;
const char* socket_path;
bool running = true;

typedef struct Player_t
{
    int fd;
    char name[MAX_USERNAME_LENGTH];
} Player_t;

typedef struct GameInstance_t
{
    bool occupied;
    bool started;
    Player_t players[2];
    GameState_t state;
} GameInstance_t;

GameInstance_t game_instances[MAX_INSTANCES];

int main(int argc, char** argv)
{
    printf("[SERVER]\n");

    // Parsing arguments
    parse_arguments(argc, argv);

    // Initializing the game instances table.
    init_game_instances();

    ServerComm_t* server = comm_server_init(5001, socket_path);

    if (server == NULL)
    {
        return 1;
    }

    struct epoll_event events[MAX_EVENTS];

    while (running)
    {
        int event_count = comm_wait_for_events(server, events);

        for (int i = 0; i < event_count; ++i)
        {
            if (comm_handle_join_request(server, &events[i]))
            {
                // A client established a connection with us.
            }
            else
            {
                int client_fd = events[i].data.fd;
                // A client has sent us a message.
                char msg_buffer[256];
                bzero(msg_buffer, 256);
                int n = read(client_fd, msg_buffer, 255);

                if (n < 0)
                {
                    perror("Failed to read from the socket.");
                    continue;
                }

                MessageType_t msg_type;
                sscanf(msg_buffer, "%d", &msg_type);

                if (msg_type < 0 || msg_type >= MSG_MAX)
                {
                    printf("Unknown message type: %d\n", msg_type);
                    continue;
                }

                printf("Received a %s message from %d: %s\n", MESSAGE_TYPE_NAMES[msg_type], events[i].data.fd, msg_buffer);

                switch (msg_type)
                {
                    case MSG_PING:
                        handle_ping_msg(msg_buffer, client_fd);
                        break;
                    case MSG_JOIN:
                        handle_join_msg(msg_buffer, client_fd);
                        break;
                    case MSG_MOVE:
                        handle_move_msg(msg_buffer, client_fd);
                        break;
                    case MSG_QUIT:
                        handle_quit_msg(msg_buffer, client_fd);
                        break;
                    default:
                        fprintf(stderr, "Received message of unknown type: %d\n", msg_type);
                        break;
                }
            }
        }
    }

    comm_server_free(server);

    return 0;
}

void parse_arguments(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("Usage: <port> <socket_path>\n");
        exit(1);
    }

    port = atoi(argv[1]);
    socket_path = argv[2];
}

void init_game_instances()
{
    for (int i = 0; i < MAX_INSTANCES; ++i)
    {
        game_instances[i].occupied = false;
        game_instances[i].started = false;
        game_instances[i].players[0].fd = -1;
        game_instances[i].players[1].fd = -1;
    }
}

void handle_ping_msg(char* msg_buffer, int client_fd)
{
    int n = write(client_fd, "I got your message", 18);

    if (n < 0)
    {
        perror("Failed to write to the socket");
        return;
    }
}

void handle_join_msg(char* msg_buffer, int client_fd)
{
    static char response_buffer[128];

    // Reading message components.
    int dummy_msg_type;
    char username[MAX_USERNAME_LENGTH];

    sscanf(msg_buffer, "%d %s", &dummy_msg_type, username);

    if (is_username_taken(username))
    {
        printf(COL_YELLOW "Client %d tried to login with a username that's already taken: %s\n" COL_RESET, client_fd, username);

        sprintf(response_buffer, "%d %d", MSG_JOIN, JOIN_NAME_TAKEN);
        comm_send_msg(client_fd, response_buffer);
        return;
    }

    int instance_idx = add_player(client_fd, username);
    if (instance_idx == -1)
    {
        printf(COL_YELLOW "Client %d (%s) tried to login, but no space left.\n" COL_RESET, client_fd, username);

        sprintf(response_buffer, "%d %d", MSG_JOIN, JOIN_ALREADY_FULL);
        comm_send_msg(client_fd, response_buffer);
        return;
    }

    printf(COL_YELLOW "Client %d logged in as %s\n" COL_RESET, client_fd, username);

    sprintf(response_buffer, "%d %d", MSG_JOIN, JOIN_SUCCESS);
    comm_send_msg(client_fd, response_buffer);

    // Trying to start the instance.
    if (try_to_start_instance(instance_idx))
    {
        // We have succeeded, so we have two waiting players.
        GameInstance_t* instance = &game_instances[instance_idx];

        sprintf(response_buffer, "%d ", MSG_START);

        serialize_game_state(response_buffer + 2, &instance->state);

        if (!comm_send_msg(instance->players[0].fd, response_buffer))
        {
            fprintf(stderr, "Failed to send JOIN response to player 0\n");
            perror("Error details");
            exit(1);
        }

        if (!comm_send_msg(instance->players[1].fd, response_buffer))
        {
            fprintf(stderr, "Failed to send JOIN response to player 1\n");
            perror("Error details");
            exit(1);
        }

        printf("Sending message: '%s' to %d and %d\n", response_buffer, instance->players[0].fd, instance->players[1].fd);
    }
}

void handle_move_msg(char* msg_buffer, int client_fd)
{

}

void handle_quit_msg(char* msg_buffer, int client_fd)
{
    // Stop running.
    // TODO Perform actual job.
    running = false;
}

bool is_username_taken(const char* username)
{
    for (int i = 0; i < MAX_INSTANCES; ++i)
    {
        GameInstance_t* instance = &game_instances[i];
        if (game_instances[i].occupied == true)
        {
            // Found an occupied game instance.
            if (instance->players[0].fd != -1 && strcmp(instance->players[0].name, username) == 0)
            {
                return true;
            }

            if (instance->players[1].fd != -1 && strcmp(instance->players[1].name, username) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

int add_player(int player_fd, const char* username)
{
    // Looking for a waiting player
    int pending_index = -1;
    for (int i = 0; i < MAX_INSTANCES; ++i)
    {
        if (game_instances[i].occupied == true && game_instances[i].started == false)
        {
            // Found a pending game instance.
            pending_index = i;
            break;
        }
    }

    if (pending_index != -1)
    {
        // Assuming that the 1st index is always free to take for pending rooms.
        game_instances[pending_index].players[1].fd = player_fd;
        strcpy(game_instances[pending_index].players[1].name, username);

        return pending_index;
    }

    // We have to create a new room
    int free_index = -1;
    for (int i = 0; i < MAX_INSTANCES; ++i)
    {
        if (game_instances[i].occupied == false)
        {
            // Found an inactive game instance.
            free_index = i;
            break;
        }
    }

    if (free_index != -1)
    {
        // The room is now occupied.
        game_instances[free_index].occupied = true;
        game_instances[free_index].players[0].fd = player_fd;
        strcpy(game_instances[free_index].players[0].name, username);
        game_instances[free_index].players[1].fd = -1;

        return free_index;
    }

    return -1;
}

/**
 * Removes the player from their game instance (if there is any), and
 * if it was the last player from that game instance, it becomes inactive.
 * ---------TO DO---------
 * @param player_fd
 */
void remove_player(int player_fd)
{
//    for (int i = 0; i < MAX_INSTANCES; ++i)
//    {
//        GameInstance_t* instance = &(game_instances[i]);
//        if (instance->occupied)
//        {
//            if (instance->player_fd[0] == player_fd || instance->player_fd[1] == player_fd)
//            {
//                // Our player is one of the players.
//
//                // Let's assume our player is player 0.
//                int* our_player = &instance->player_fd[0];
//                int* other_player = &instance->player_fd[1];
//
//                if (instance->player_fd[1] == player_fd)
//                {
//                    // We are actually player 1.
//                    our_player = &instance->player_fd[1];
//                    other_player = &instance->player_fd[0];
//                }
//
//                *our_player = -1;
//
//                if (*other_player != -1)
//                {
//                    // Send rejection message.
//
//                }
//                else
//                {
//                    // Our player was the only player left.
//                }
//            }
//        }
//    }
}

bool try_to_start_instance(int instance_idx)
{
    GameInstance_t* instance = &game_instances[instance_idx];

    if (instance->occupied && instance->players[0].fd != -1 && instance->players[1].fd != -1)
    {
        // Marking the instance as "started".
        instance->started = true;
        // Resetting the game state for that instance.
        reset_game_state(&instance->state);
        return true;
    }

    return false;
}