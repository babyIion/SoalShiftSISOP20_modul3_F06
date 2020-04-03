#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define M 4
#define K 2
#define N 5

int main(){
    key_t key = 1234;
    //int (*hasil)[M][N];
    int *hasil;

    int shmid = shmget(key, sizeof(int)*M*N, IPC_CREAT|0666);

    hasil = shmat(shmid, NULL, 0);

    int matriksA[M][K] = {{3, 2}, {1, 11}, {5, 10}, {9, 15}};
    int matriksB[K][N] = {{6, 17, 7, 12, 13}, {18, 1, 14, 4, 8}};

    int res[M][N];
    int i, j, k;
    int tot=0;
    
	for(i=0; i<M; i++){
		for(j=0; j<N; j++)
		    res[i][j] = 0;
            //*hasil[i][j] = 0;
	}
    
    

    for(i=0; i<M; i++){
        for(j=0; j<N; j++){
            //*hasil[i][j] = 0;
            for(k=0; k<K; k++){
                res[i][j] = res[i][j] + matriksA[i][k] * matriksB[k][j];
            }
        }
    }
    printf("Hasil perkalian matriks:\n");

    for(i=0; i<M; i++){
        for(j=0; j<N; j++)
            printf("%d ", res[i][j]);
        printf("\n");
    }

    int index = 0;
    for(i=0; i<M; i++){
        for(j=0; j<N; j++){
            hasil[index] = res[i][j];
            index++;
        }
    }

    sleep(5);

    shmdt(hasil);
    shmctl(shmid, IPC_RMID, NULL);
}

