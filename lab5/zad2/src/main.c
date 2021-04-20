#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* USAGE = "<sender|date> | [address] [subject] [contents]";

typedef enum SortingKey_t {
    SORT_SENDER,
    SORT_DATE
} SortingKey_t;

void print_mails(SortingKey_t sort_by)
{
    FILE* mail_handler;
    if (sort_by == SORT_SENDER)
    {
        mail_handler = popen("mail | tail -n +2 | sort -k3d -", "w");
    }
    else
    {
        // All mail seems to be sorted by date of arrival by default.
        mail_handler = popen("mail | tail -n +2", "w");
    }

    // Telling the exit command to stop printing out mail.
    fputs("exit", mail_handler);
    pclose(mail_handler);
}

void write_mail(const char* email, const char* subject, const char* message)
{
    FILE* mail_handler;
    char command_buffer[256];
    sprintf(command_buffer, "mail -s %s %s", subject, email);
    mail_handler = popen(command_buffer, "w");

    // Passing the message to the input of the process.
    fputs(message, mail_handler);
    pclose(mail_handler);
}

int main(int argc, char** argv)
{
    if (argc != 2 && argc != 4)
    {
        printf("%s\n", USAGE);
        return 0;
    }

    if (argc == 2)
    {
        SortingKey_t sorting_key;
        if (strcmp(argv[1], "nadawca") == 0)
        {
            sorting_key = SORT_SENDER;
        }
        else if (strcmp(argv[1], "data") == 0)
        {
            sorting_key = SORT_DATE;
        }
        else
        {
            fprintf(stderr, "Unsupported sorting key: '%s'\n", argv[1]);
            exit(1);
        }

        print_mails(sorting_key);
    }
    else
    {
        const char* email = argv[1];
        const char* subject = argv[2];
        const char* message = argv[3];

        write_mail(email, subject, message);
    }

    return 0;
}
