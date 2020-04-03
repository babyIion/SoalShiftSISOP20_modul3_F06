#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<pthread.h>

#define M 4
#define N 5

pthread_mutex_t lock;
int array_hasil[20];
int id_arr = 0;

void* jumlah(void* angka){
    //pthread_mutex_lock(&lock);
    int bil;

    bil = (int)(intptr_t)angka;
    //printf("angka = %d\n", bil);
    int hasil = 1;
    int i;

    for(i=2; i<=bil; i++)
        hasil += i;
    
    //printf("hasil = %d\n", hasil);
    //pthread_mutex_lock(&lock);
    //printf("angka>%d = %d\n", bil, hasil);
    array_hasil[id_arr] = hasil;
    id_arr++;
    //pthread_mutex_unlock(&lock);

}

int main(){
    if (pthread_mutex_init(&lock, NULL) != 0){
        printf("\n mutex init failed\n");
        return 1;
    }

    key_t key = 1234;
    int *hasil;

    int shmid = shmget(key, sizeof(int)*M*N, IPC_CREAT|0666);
    hasil = shmat(shmid, NULL, 0);

    int i, j;

    printf("Hasil perkalian matriks:\n");
    for(i=0; i<20; i++){
        printf("%d ", hasil[i]);
        if((i + 1) % 5 == 0){
            printf("\n");
        }
    }

    pthread_t thread[20];
    int k = 0 , err;
    for(i=0; i<20; i++){
        err = pthread_create(&(thread[i]), NULL, &jumlah, (void*)(intptr_t)hasil[i]);
    }

    for(i=0; i<20; i++){
        pthread_join(thread[i], NULL);
    }

    printf("\nHasil penjumlahan:\n");
        for(i=0; i<20; i++){
            printf("%d ", array_hasil[i]);
            //fflush(stdout);
            if((i + 1) % 5 == 0){
                printf("\n");
                //fflush(stdout);
            }
        }

    shmdt(hasil);
    shmctl(shmid, IPC_RMID, NULL);
}