void wait_for_all_children()
{
    // Waiting for child processes to end.
    int status = 0;
    while ((wait(&status)) > 0);
}