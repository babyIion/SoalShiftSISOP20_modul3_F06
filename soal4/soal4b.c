#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<pthread.h>

#define M 4
#define N 5

void* jumlah(void* angka){
    int bil;

    bil = (int)(intptr_t)angka;

    int hasil = 1;
    int i;

    for(i=2; i<=bil; i++)
        hasil += i;

    int *tambah = (int*)malloc(sizeof(int));
    *tambah = hasil;
    pthread_exit(tambah);
}

int main(){

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

    printf("Hasil penjumlahan:\n");
    for(i=0; i<20; i++){
        void *k;
        pthread_join(thread[i], &k);

        int* p = (int*)k;
        printf("%d ", *p);
        if((i + 1) % 5 == 0)
            printf("\n");
    }

    shmdt(hasil);
    shmctl(shmid, IPC_RMID, NULL);
}