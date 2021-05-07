#define CLOSE_READ 1
#define CLOSE_WRITE 2

/**
 * @param pipe_desc A 2-element table containing file descriptors for both the in and out of the pipe.
 * @param mode Flags to determine if the pipe 'read' and/or 'write' should be closed.
 *             CLOSE_READ and CLOSE_WRITE are the appropriate flags to use.
 */
void close_pipe(int* pipe_desc, int mode)
{
    if ((mode & CLOSE_READ) && pipe_desc[0] != STDIN_FILENO)
        close(pipe_desc[0]);

    if ((mode & CLOSE_WRITE) && pipe_desc[1] != STDOUT_FILENO)
        close(pipe_desc[1]);
}