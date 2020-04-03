#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#define PORT 8080

void dostuff(int);

typedef struct akun {
    char username[50];
    char password[50];
} akun_t;

pthread_t client1, client2, ggamehit1, ggamehit2;
int new_socket;
int ns1, ns2;
int hp1, hp2;
int ready1, ready2;
int gamerunning;


void* tred(void* arg){
    dostuff(new_socket);
}

void* gamehit1(void* arg) {
    char hit[10];
    strcpy(hit, "running");
    while (hp1 > 0 && hp2 > 0) {
        read(ns1, hit, 10);
        if (hp1<=0 || hp2 <=0) {
            break;
        }
        hp2-=10;
        send(ns2, hit, 10, 0);
    }
    gamerunning=0;
}
void* gamehit2(void* arg) {
    char hit[10];
    strcpy(hit, "running");
    while (hp1 > 0 && hp2 > 0) {
        read(ns2, hit, 10);
        if (hp1<=0 || hp2 <=0) {
            break;
        }
        hp1-=10;
        send(ns1, hit, 10, 0);
    }
    gamerunning=0;
}

int main(int argc, char const *argv[]) {
    int server_fd, pid;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    for (int i=0; i<2; i++) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if(new_socket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        if (i==0) {
            ns1 = new_socket;
            printf("join %d\n", ns1);
            pthread_create(&client1, NULL, &tred, NULL);
        }
        else {
            ns2 = new_socket;
            printf("join %d\n", ns2);
            pthread_create(&client2, NULL, &tred, NULL);
        }
    }

    pthread_join(client1, NULL);
    pthread_join(client2, NULL);

    return 0; 
}

void dostuff (int sock){
    int valread;
    char buffer[1024] = {0};
    char menu[10];

    ready1 = 0;
    ready2 = 0;

    FILE *f;
    
    while(1) {
        bzero(buffer, 1024);
        valread = read(sock, buffer, 1024);
        f = fopen("akun.txt", "a+");

        if (strcmp(buffer, "login")==0) {
            printf("masuk login\n");

            akun_t curr_akun;
            int auth = 0;

            char status[10], username[50], password[50];
            read(sock, username, 50);
            read(sock, password, 50);

            rewind(f);
            while( fscanf(f, "%s %s", curr_akun.username, curr_akun.password)!=EOF ) {
                if ( strcmp(username, curr_akun.username)==0 &&
                     strcmp(password, curr_akun.password)==0 )
                    auth = 1;
            }

            if (auth == 1) {
                char found[10], gameend[10];
                strcpy(found, "found");
                strcpy(gameend, "gameend");
                

                printf("Auth Success\n");
                strcpy(status, "sukses");
                send(sock, status, 10, 0);

                while(1) {
                    read(sock, menu, 10);
                    if (strcmp(menu, "biasa")==0) {
                        if (sock == ns1)
                            printf("%d logout\n", ns1);
                        else if (sock == ns2)
                            printf("%d logout\n", ns2);
                        break;
                    }
                    else if (strcmp(menu, "find")==0) {
                        if (sock == ns1) {
                            ready1 = 1;
                            printf("%d ready\n", ns1);
                        }
                        else if (sock == ns2) {
                            ready2 = 1;
                            printf("%d ready\n", ns2);
                        }
                        while (ready1 == 0 || ready2 == 0) {}
                        printf("both players are ready\n");
                        gamerunning=1;
                        send(sock, found, 10, 0);

                        if (sock == ns1)
                            hp1 = 100;
                        else if (sock == ns2)
                            hp2 = 100;

                        if (sock == ns1)
                            pthread_create(&ggamehit1, NULL, *gamehit1, NULL);
                        else if (sock == ns2)
                            pthread_create(&ggamehit2, NULL, *gamehit2, NULL);
                        
                        while(gamerunning) {}
                        pthread_cancel(ggamehit1);
                        pthread_cancel(ggamehit2);
                        hp1=100,hp2=100,ready1=0,ready2=0,gamerunning=0;
                    }
                }
            }
            else {
                printf("Auth Failed\n");
                strcpy(status, "gagal");
                send(sock, status, 10, 0);
            }

            fclose(f);
        }
        else if (strcmp(buffer, "register")==0) {
            printf("masuk register\n");
            akun_t reg_akun;
            
            char username[50], password[50];
            read(sock, username, 50);
            strcpy(reg_akun.username, username);
            read(sock, password, 50);
            strcpy(reg_akun.password, password);

            fprintf(f, "%s %s\n", reg_akun.username, reg_akun.password);

            // fwrite(&reg_akun, sizeof(reg_akun), 1, f);
            // printf("Username : %s %s\n", username, reg_akun.username);
            // printf("Password : %s %s\n\n", password, reg_akun.password);

            rewind(f);
            while( fscanf(f, "%s %s", username, password)!=EOF ) {
                printf("%s %s\n", username, password);
            }

            fclose(f);
        }
    }
}