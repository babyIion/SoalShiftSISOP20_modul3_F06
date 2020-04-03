# SoalShiftSISOP20_modul3_F06
## Soal 1
Qiqi adalah sahabat MamMam dan Kaka. Qiqi , Kaka dan MamMam sangat senang
bermain “Rainbow six” bersama-sama , akan tetapi MamMam sangat Toxic ia selalu
melakukan Team killing kepada Qiqi di setiap permainannya. Karena Qiqi orang yang
baik hati, meskipun marah Qiqi selalu berkata “Aku nggk marah!!”. Kaka ingin
meredam kemarahan Qiqi dengan membuatkannya sebuah game yaitu TapTap
Game. akan tetapi Kaka tidak bisa membuatnya sendiri, ia butuh bantuan mu. Ayo!!
Bantu Kaka menenangkan Qiqi.
TapTap Game adalah game online berbasis text console. Terdapat 2 program yaitu
tapserver.c dan tapplayer.c
Syarat :
- Menggunakan Socket, dan Thread

#### soal2_server.c
~~~
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
~~~
Penjelasan:
- Program server akan mengatur pencatatan register akun, pengecekan login, logout akun dan matchmaking.
- `new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);` diulang sebanyak 2x agar dapat menerima 2 client yang akan saling bermain game. id new_socket masing-masing client disimpan dalam ns1 dan ns2.
- Setelah menerima client, akan langsung membuat thread yang akan menjalankan segala perintah server. Menggunakan thread agar dapat menjalankan pengaturan 2 client sekaligus.
- Ketika pertama kali berjalan akan langsung membuka file akun untuk menyiapkan register dan login `f = fopen("akun.txt", "a+");`
- Untuk register, akan menerima kiriman dari client berupa username dan password lalu melakukan write ke file akun.txt.
- Untuk login, akan menerima kiriman dari client berupa username dan password lalu melakukan pengecekan file akun.txt untuk mengetahui apakah username dan password yang diterima terdapat pada database akun.
- Setelah login berhasil, server akan bersiap pada halaman find match. Apabila client memutuskan untuk find match, maka server akan menandakan salah satu pemain berstatus ready.
- Apabila kedua pemain sudah ready, server akan mengirimkan kepada kedua client bahwa match sudah ditemukan.
- Server akan terus menunggu hingga salah satu pemain kalah. Selama menunggu, server bekerja sebagai perantara kedua client yang bermain, mengirimkan hit-hit yang diberikan pemain.
- Ketika permainan berakhir, server stand-by pada halaman screen 2.

#### soal2_client.c
~~~
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
~~~
Penjelasan:
- Program client digunakan sebagai interface untuk user bermain game. Mengatur request register, login, logout dan find match.
- Saat register, client akan mengirimkan username dan password ke server lalu mendapat notifikasi register sukses.
- Saat login, client akan mengirimkan username dan password ke server lalu mendapat notifikasi apakah login sukses atau gagal. Apabila gagal, kembali ke menu utama. Apabila sukses, masuk ke screen 2.
- Pada screen 2, client dapat logout dan find match.
- Saat logout, client akan mengirimkan pemberitahuan ke server bahwa client logout dan kembali ke menu utama.
- Saat find match, client akan mengirimkan pemberitahuan ke server bahwa client sedang mencari lawan dan terus menunggu hingga lawan ketemu.
- Permainan akan dimulai ketika client menerima notifikasi dari server bahwa game telah found.
- Client akan bermain dengan men-tap spasi di keyboard yang akan menampilkan tulisan hit di layar client itu sendiri, dan mengirimkan hit tersebut ke server. Client lawan akan menerima masukan dari server bahwa client lawan tersebut menerima hit dari musuhnya.
- Client yang terkena hit akan menampilkan HP miliknya sekarang di layarnya sendiri.
- Setelah salah satu orang kalah, client akan menampilkan apakah client tersebut menang atau kalah.
- Client akan kembali ke menu screen 2 untuk find match kembali, atau logout.
