#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define UNIX_PATH_MAX 108
#define SOCK_PATH "/tmp/kol2_socket" // Changed this so that the program doesn't have to have elevated permissions.

int main(int argc, char *argv[]) {
    

   if(argc !=2){
    printf("Not a suitable number of program parameters\n");
    return(1);
   }
    sleep(1);


    /*********************************************
    Utworz socket domeny unixowej typu datagramowego
    Utworz strukture adresowa ustawiajac adres/sciezke komunikacji na SOCK_PATH
    Polacz sie korzystajac ze zdefiniowanych socketu i struktury adresowej
    ***********************************************/
    int fd = socket(AF_UNIX, SOCK_DGRAM, 0 /*automatic*/);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to create UNIX socket\n");
        perror("Error details");
        return 1;
    }

    struct sockaddr_un server_addr;
    // Zeroing out the address buffer.
    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
    {
        perror("Failed to connect to receiver");
        return 1;
    }

    //////////////////////////////////////////

    char buff[20];
    int to_send = sprintf(buff, argv[1]);

    if(write(fd, buff, to_send + 1) == -1) {
        perror("Error sending msg to server");
    }


    /*****************************
    posprzataj po sockecie
    ********************************/
    shutdown(fd, SHUT_RDWR);

    close(fd); // Zamykamy file descriptor
    //////////////////////////
    return 0;
}

