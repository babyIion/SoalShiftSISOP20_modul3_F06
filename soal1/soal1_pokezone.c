#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int *pokerarety;
int *pokeshiny;
int *lulpow;
int *pokeball;
int *berry;

key_t keypokerarety = 1231;
key_t keypokeshiny = 1232;
key_t keylulpow = 1233;
key_t keypokeball = 1234;
key_t keyberry = 1235;



void main()
{
        srand(time(0));
        int idpokerarety = shmget(1231, sizeof(int), IPC_CREAT | 0666);
        int idpokeshiny = shmget(1232, sizeof(int), IPC_CREAT | 0666);
        int idlulpow = shmget(1233, sizeof(int), IPC_CREAT | 0666);
        int idpokeball = shmget(1234, sizeof(int), IPC_CREAT | 0666);
        int idberry = shmget(1235, sizeof(int), IPC_CREAT | 0666);
        




        shmdt(pokerarety);
        shmdt(pokeshiny);
        shmdt(lulpow);
        shmdt(pokeball);
        shmdt(berry);
        shmctl(idpokerarety, IPC_RMID, NULL);
        shmctl(idpokeshiny, IPC_RMID, NULL);
        shmctl(idlulpow, IPC_RMID, NULL);
        shmctl(idpokeball, IPC_RMID, NULL);
        shmctl(idberry, IPC_RMID, NULL);
}