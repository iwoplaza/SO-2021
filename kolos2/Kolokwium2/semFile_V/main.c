#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/wait.h>

#define FILE_NAME "common.txt"
#define SEM_NAME "/kol_sem"


// Sami musimy zadeklarować tą strukturę.
union semun {
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

void _sem_change(int sem_group_id, int sem_idx, short delta)
{
    static struct sembuf sops[1];

    sops[0].sem_num = sem_idx;
    sops[0].sem_op = delta;
    sops[0].sem_flg = 0;

    if (semop(sem_group_id, sops, 1) == -1)
    {
        perror("Semop failed");
    }
}

void _request_access(int sem_group_id, int sem_idx)
{
    _sem_change(sem_group_id, sem_idx, -1);
}

void _return_access(int sem_group_id, int sem_idx)
{
    _sem_change(sem_group_id, sem_idx, 1);
}


int main(int argc, char** args){

   if(argc !=4){
    printf("Not a suitable number of program parameters\n");
    return(1);
   }

    /**************************************************
    Stworz semafor systemu V
    Ustaw jego wartosc na 1
    ***************************************************/

    key_t key = ftok("/tmp/kol2_sem", 'K');
    if (key == -1)
    {
        perror("Couldn't run ftok()");
        exit(1);
    }

    int sem_group_id;

    sem_group_id = semget(key, 1, IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO);

    if (sem_group_id == -1)
    {
        perror("Failed to open semaphore group");
        return 1;
    }

    // Setting default value
    union semun arg;
    arg.val = 1;

    if (semctl(sem_group_id, 0, SETVAL, arg) == -1)
    {
        perror("Couldn't set semaphore value");
        return 1;
    }

    ///////////////////////////////////////////////////

    
     
     int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC , 0644);
     
     int parentLoopCounter = atoi(args[1]);
     int childLoopCounter = atoi(args[2]);

    // Zwiększam wielkość bufforu żeby wpisywanie z użyciem sprintf
    // nie wpisywało poza buffor.
     char buf[64];
     pid_t childPid;
     int max_sleep_time = atoi(args[3]);
     


     if(childPid=fork()){
      int status = 0;
      srand((unsigned)time(0)); 

        while(parentLoopCounter--){
    	    int s = rand()%max_sleep_time+1;
    	    sleep(s);    
            
            /*****************************************
            sekcja krytyczna zabezpiecz dostep semaforem
            **********************************************/

            _request_access(sem_group_id, 0);

            ///////////////////////////////////////////////////
        
            sprintf(buf, "Wpis rodzica. Petla %d. Spalem %d\n", parentLoopCounter,s);
            write(fd, buf, strlen(buf));
            write(1, buf, strlen(buf));

            /*********************************
            Koniec sekcji krytycznej
            **********************************/

            _return_access(sem_group_id, 0);

            ///////////////////////////////////////////////////
        }
        waitpid(childPid,&status,0);
     }
     else{

	srand((unsigned)time(0)); 
        while(childLoopCounter--){

	    int s = rand()%max_sleep_time+1;
            sleep(s);                
            

            /*****************************************
            sekcja krytyczna zabezpiecz dostep semaforem
            **********************************************/

            _request_access(sem_group_id, 0);

            ///////////////////////////////////////////////////
            
            sprintf(buf, "Wpis dziecka. Petla %d. Spalem %d\n", childLoopCounter,s);
            write(fd, buf, strlen(buf));
            write(1, buf, strlen(buf));

            /*********************************
            Koniec sekcji krytycznej
            **********************************/

            _return_access(sem_group_id, 0);

            ///////////////////////////////////////////////////
        }
        _exit(0);
     }
     
    /*****************************
    posprzataj semafor
    ******************************/

    semctl(sem_group_id, 0, IPC_RMID);

    close(fd);
    return 0;
}
     
        
