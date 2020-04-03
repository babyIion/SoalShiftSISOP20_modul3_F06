#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <termios.h>
#define PORT 8080

pthread_t tid[4];
pid_t child;
int sock;
char found[10];
int yourhp,enemyhp;
int gameend;

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

void* cleart(void *arg)
{
	char *argv1[] = {"clear", NULL};
	child = fork();
	if (child==0) {
	    execv("/usr/bin/clear", argv1);
	}

	return NULL;
}

void* getcht(void *arg)
{
    char gethit[10];
    strcpy(gethit, "running");
    
    while (enemyhp>0 && yourhp>0) {
        getch();
        if (enemyhp<=0 || yourhp <=0) {
            break;
            gameend=1;
        }
        printf("Hit !!!\n");
        enemyhp-=10;
        send(sock, gethit, 10, 0);
    }
    gameend=1;

	return NULL;
}

void* kepukul(void *arg)
{
    char kepukul[10];
    printf("Your HP : %d\n", yourhp);
    while (enemyhp>0 && yourhp>0) {
        read(sock, kepukul, 10);
        if (strcmp(kepukul, "running")==0) {
            yourhp -= 10;
            if (enemyhp<=0 || yourhp <=0) {
                break;
                gameend=1;
            }
            printf("Your HP : %d\n", yourhp);
        }
        else if (strcmp(kepukul, "gameend")==0) {
            gameend=1;
            break;
        }
    }
    gameend=1;

	return NULL;
}

void* findmatch(void *arg)
{
    while(strcmp(found, "found")!=0) {
        printf("Waiting for player...\n");
        sleep(1);
    }

    return NULL;
}

int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(1) {
        bzero(buffer, 1024);

        pthread_create(&(tid[0]), NULL, *cleart, NULL);
        pthread_join(tid[0], NULL);
        sleep(1);

        printf("1. Login\n");
        printf("2. Register\n");
        printf("Choices : ");
        scanf("%s", buffer);
        send(sock, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "login")==0) {
            char status[10], menu[10], username[50], password[50];

            printf("Username : ");
            scanf("%s", username);
            send(sock, username, 50, 0);
            printf("Password : ");
            scanf("%s", password);
            send(sock, password, 50, 0);

            read(sock, status, 10);
            if (strcmp(status, "sukses")==0) {
                printf("Login Sukses\n");
                sleep(1);
                while(1) {
                    pthread_create(&tid[0], NULL, *cleart, NULL);
                    pthread_join(tid[0], NULL);
                    sleep(1);
                    printf("1. Find Match\n");
                    printf("2. Logout\n");
                    printf("Choices : ");
                    scanf("%s", buffer);
                    
                    if (strcmp(buffer, "find")==0) {
                        strcpy(menu, "find");
                        send(sock, menu, 10, 0);
                        pthread_create(&(tid[2]), NULL, *findmatch, NULL);
                        while(strcmp(found,"found")!=0) {
                            read(sock, found, 10);
                            if (strcmp(found, "found")==0) {
                                yourhp=100,enemyhp=100,gameend=0;
                                printf("Game dimulai silahkan tap space secepat mungkin !\n");
                                getch();

                                // Hitter detect
                                pthread_create(&(tid[1]), NULL, *getcht, NULL);
                                // Info nyawa
                                pthread_create(&(tid[3]), NULL, *kepukul, NULL);
                                
                                while(gameend==0) {}
                                pthread_cancel(tid[1]);
                                pthread_cancel(tid[3]);
                                
                                if (yourhp<=0)
                                    printf("Game berakhir, kamu kalah !\n");
                                else
                                    printf("Game berakhir, kamu menang !\n");
                                sleep(3);

                                strcpy(found,"notfound");
                                break;
                            }
                        }
                    }  
                    else if (strcmp(buffer, "logout")==0) {
                        strcpy(menu, "biasa");
                        send(sock, menu, 10, 0);
                        break;
                    }
                }
            }
            else if (strcmp(status, "gagal")==0) {
                printf("Login Gagal\n");
                sleep(1);
            }
        }
        else if (strcmp(buffer, "register")==0) {
            char username[50], password[50];
            printf("Username : ");
            scanf("%s", username);
            send(sock, username, 50, 0);
            printf("Password : ");
            scanf("%s", password);
            send(sock, password, 50, 0);

            printf("Register Sukses\n");
            sleep(1);
        }   
    }
    return 0;
}
