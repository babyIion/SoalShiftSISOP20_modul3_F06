# SoalShiftSISOP20_modul3_F06
## Soal 2
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
~~~c
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
~~~c
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

## Soal 3
Membuat program untuk mengkategorikan file. (opsi -d belum benar :( )
~~~c
int i, err;
void* pindah(void*);
int is_regular_file(const char*);

int main(int argc, char* argv[]){
    if(argc < 2) 
        printf("argumennya kakak\n");
    else if(strcmp(argv[1], "-f") == 0){
        pthread_t thread[argc-2];

        for(i=0; i<(argc-2); i++){
            if(is_regular_file(argv[i+2])){
                err = pthread_create(&(thread[i]), NULL, &pindah, (void*)argv[i+2]);
            }
            else
                printf("bukan file\n");
        }

        for(i=0; i<(argc-2); i++){
            pthread_join(thread[i], NULL);
        }
    }
    else if(strcmp(argv[1], "*") == 0){
        int count = 0;
        int index = 0;
        DIR* d;
        struct dirent *dir;
        d = opendir(".");
        if(d){
            while((dir = readdir(d)) != NULL){
                if(dir->d_type == DT_REG){
                    count++;
                }
            }
        }
        closedir(d);
        pthread_t thread[count];
        //printf("%d\n", count);
        d = opendir(".");
        if(d){
            while((dir = readdir(d)) != NULL){
                if(dir->d_type == DT_REG){
                    //printf("%s\n", dir->d_name);
                    err = pthread_create(&(thread[index]), NULL, &pindah, (void*)dir->d_name);
                    //printf("%d\n", index);
                    index++;
                }
            }
        }
        closedir(d);
        for(i=0; i<count; i++){
            pthread_join(thread[i], NULL);
        }
        
    }
    else if(strcmp(argv[1], "-d") == 0){
        int count = 0;
        int index = 0;
        DIR* d;
        struct dirent *dir;
        if(is_regular_file(argv[2])){
            printf("Bukan direktori\n");
            exit(1);
        }
            
        d = opendir(argv[2]);
        if(d){
            while((dir = readdir(d)) != NULL){
                if(dir->d_type == DT_REG){
                    count++;
                }
            }
        }
        closedir(d);
        pthread_t thread[count];
        //printf("%d\n", count);
        d = opendir(argv[2]);
        if(d){
            while((dir = readdir(d)) != NULL){
                if(dir->d_type == DT_REG){
                    //printf("%s\n", dir->d_name);
                    char path[300];
                    sprintf(path, "%s/%s", argv[2], dir->d_name);
                    printf("path: %s\n", path);
                    err = pthread_create(&(thread[index]), NULL, &pindah, (void*)path);
                    //printf("%d\n", index);
                    index++;
                }
            }
        }
        closedir(d);
        for(i=0; i<count; i++){
            pthread_join(thread[i], NULL);
        }
    }
    else{
        printf("Salah argumen kakak\n");
    }
}

void* pindah(void* ptr){
    char* path;
    char* ext;
    char* name;
    path = (char*)ptr;

    printf("path pindah: %s\n", path);
    name = strrchr(path, '/');  //nama file tapi ada /-nya, eg: "/soal1.sh"
    if(name) printf("%s\n", name);
    char newfolder[100];        
    ext = strrchr(path, '.');   //extensi file
    //kalau jenis file diketahui
    if(ext){
        //printf("%s\n", path);

        for(i=0; i<(strlen(ext)); i++){
            newfolder[i] = ext[i+1];
        }
        //printf("newfolder: %s\n", newfolder);
        int compare;
        int sama = 0;
        DIR* d = opendir(".");
        struct dirent *dir;
        if(d){
            while((dir = readdir(d)) != NULL){
                compare = strcasecmp(newfolder, dir->d_name);
                //printf("%s %s %d\n", newfolder, dir->d_name, compare);
                if(compare == 0){
                        if(dir->d_type == DT_DIR){
                            sama = 1;
                            strcpy(newfolder, dir->d_name);
                            //printf("udah ada foldernya\n");
                            break;
                        }
                    break; 
                }
            }
        }
        closedir(d);
        if(sama == 1){
            //printf("masuk sama\n");
            if(name){
                strcat(newfolder, name);
                printf("ada nama, folder: %s\n", newfolder);
            }   
            else{
                sprintf(newfolder, "%s/%s", newfolder, path);
                printf("folder: %s\n", newfolder);
            }  
            
            if(rename(path, newfolder) < 0)
                printf("Error move file\n");
            else{
                printf("berhasil hore\n");
            }
        }
        else{
            printf("masuk beda\n");
            if(mkdir(newfolder, 0777) < 0)
                printf("Error create folder\n");
            else{
                if(name){
                    strcat(newfolder, name);
                    printf("ada nama, folder: %s\n", newfolder);
                }   
                else{
                    sprintf(newfolder, "%s/%s", newfolder, path);
                    printf("folder: %s\n", newfolder);
                }  
                if(rename(path, newfolder) < 0)
                    printf("Error move file\n");
                else{
                    printf("berhasil hore\n");
                }
            }
        }
        
    }
    else{
        printf("masuk unknown\n");
        char unk[100];
        strcat(unk, "Unknown");
        DIR* dir = opendir(unk);
        if(dir){
            printf("udah ada\n");
            if(name)
                strcat(unk, name);
            else
                sprintf(unk, "%s/%s", unk, path);
            //printf("folder: %s\npath: %s\n", unk, path);
            if(rename(path, unk) < 0)
                printf("Error move file\n");
            else{
                printf("berhasil hore\n");
            }
        }
        else{
            printf("newfolder: %s", unk);
            if(mkdir(unk, 0777) < 0)
                printf("Error create folder\n");
            else{
                if(name)
                    strcat(unk, name);
                else
                    sprintf(unk, "%s/%s", unk, path);
                if(rename(path, unk) < 0)
                    printf("Error move file\n");
                else{
                    printf("berhasil hore\n");
                }
            }
        }
        closedir(dir);
    }      
    
}

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
~~~
Penjelasan:
- kondisi apabila user tidak memberikan opsi
~~~c
if(argc < 2) 
        printf("argumennya kakak\n");
~~~
- kondisi apabila user memberikan opsi -f
~~~c
else if(strcmp(argv[1], "-f") == 0){
        pthread_t thread[argc-2];

        for(i=0; i<(argc-2); i++){
            if(is_regular_file(argv[i+2])){
                err = pthread_create(&(thread[i]), NULL, &pindah, (void*)argv[i+2]);
            }
            else
                printf("bukan file\n");
        }

        for(i=0; i<(argc-2); i++){
            pthread_join(thread[i], NULL);
        }
    }
~~~
Membuat thread sebanyak file yang diketik (argc-2 karena argumen pertama adalah program dan argumen kedua adalah opsi). Selanjutnya melakukan iterasi sebanyak file tadi, dicek terlebih dahulu apakah argumen yang dimasukkan merupakan file atau bukan. Jika file, maka akan melakukan `pthread_create` dengan parameter thread ke-i, NULL, fungsi pindah, dan path file yang telah dimasukkan. Setelah itu melakukan `pthread_join`.
- Kondisi apabila user memberikan opsi \*
~~~c
 else if(strcmp(argv[1], "*") == 0){
        int count = 0;
        int index = 0;
        DIR* d;
        struct dirent *dir;
        d = opendir(".");
        if(d){
            while((dir = readdir(d)) != NULL){
                if(dir->d_type == DT_REG){
                    count++;
                }
            }
        }
        closedir(d);
~~~

Pertama mendeklarasikan sekaligus menginisiasi variabel count untuk menyimpan jumlah file yang ada di folder dan variabel index untuk index dari array thread. Membuka direktori (".") atau current direktori, melakukan read pada direktori itu dan menghitung jumlah file yang ada di dalamnya.

~~~c
pthread_t thread[count];
//printf("%d\n", count);
d = opendir(".");
if(d){
    while((dir = readdir(d)) != NULL){
	if(dir->d_type == DT_REG){
	    //printf("%s\n", dir->d_name);
	    err = pthread_create(&(thread[index]), NULL, &pindah, (void*)dir->d_name);
	    //printf("%d\n", index);
	    index++;
	}
    }
}
closedir(d);
for(i=0; i<count; i++){
    pthread_join(thread[i], NULL);
}
~~~

Mendeklarasikan thread sebanyak jumlah file. Membuka direktori dan membacanya kembali, apabila file yang dibaca merupakan regular file (bukan direktori) maka file tersebut akan dikategorikan dengan melakukan `pthread_create` sama seperti opsi -f yang sudah dijelaskan diatas, hanya saja yang dipassing-kan adalah nama filenya.

- Kondisi apabila user memberikan opsi -d
~~~c
while((dir = readdir(d)) != NULL){
                if(dir->d_type == DT_REG){
                    //printf("%s\n", dir->d_name);
                    char path[300];
                    sprintf(path, "%s/%s", argv[2], dir->d_name);
                    //printf("path: %s\n", path);
                    err = pthread_create(&(thread[index]), NULL, &pindah, (void*)path);
                    //printf("%d\n", index);
                    index++;
                }
            }
~~~
Awalnya sama dengan opsi \*, namun yang dibuka adalah direktori path dari argumen yang telah diberikan. Menghitung jumlah file yang ada di dalam direktori tersebut, lalu membaca isi dari direktori. Di kondisi ini dibuat sebuah string path sebagai tempat untuk menyimpan path masing-masing file. Proses selanjutnya sama dengan proses yang ada di opsi \* hanya saja yang dipassing-kan adalah string path tadi.

- Fungsi pindah
~~~c
char* path;
char* ext;
char* name;
path = (char*)ptr;
~~~
Mendeklarasikan string path sebagai tempat menyimpan path yang ada dalam passing parameter. `path = (char*)ptr` meng-assign void pointer ptr pada parameter menjadi string.
~~~c
name = strrchr(path, '/');  //nama file tapi ada /-nya, eg: "/soal1.sh"
char newfolder[100];        
ext = strrchr(path, '.');   //extensi file
~~~
fungsi strrchr me-return sebuah string dari karakter terakhir yang ada pada parameter kedua dalam string pada parameter pertama hingga akhir string. Misal path-nya adalah "/home/tari/modul3/coba.txt", maka string name nantinya akan berisi "/coba.txt" sedangkan string ext berisi ".txt"

- Jika extensi file diketahui
~~~c
for(i=0; i<(strlen(ext)); i++){
    newfolder[i] = ext[i+1];
}
//printf("newfolder: %s\n", newfolder);
int compare;
int sama = 0;
~~~
Mengcopy string ext pada newfolder namun tanpa titik. Mendeklarasikan variabel compare dan sama = 0 (sebagai flag untuk membuat new folder)
~~~c
DIR* d = opendir(".");
struct dirent *dir;
if(d){
    while((dir = readdir(d)) != NULL){
	compare = strcasecmp(newfolder, dir->d_name);
	//printf("%s %s %d\n", newfolder, dir->d_name, compare);
	if(compare == 0){
		if(dir->d_type == DT_DIR){
		    sama = 1;
		    strcpy(newfolder, dir->d_name);
		    //printf("udah ada foldernya\n");
		    break;
		}
	    break; 
	}
    }
}
closedir(d);
~~~
Membuka current directory kemudian membacanya. Meng-assign nilai compare dengan nilai return fungsi strcasecmp yaitu fungsi untuk membandingkan dua string dengan mengabaikan besar kecil hurufnya. Apabila nilai compare 0, maka folder extensi dari file sudah ada. Flag sama diganti menjadi 1, lalu meng-copy nama folder yang sudah ada tersebut ke string newfolder.
~~~c
 if(sama == 1){
    //printf("masuk sama\n");
    if(name)
	strcat(newfolder, name);
    else
	sprintf(newfolder, "%s/%s", newfolder, path);
    printf("folder: %s\n", newfolder);
    if(rename(path, newfolder) < 0)
	printf("Error move file\n");
    else{
	printf("berhasil hore\n");
    }
}
~~~
Jika folder extensi sudah ada, cek apakah variabel name NULL atau tidak (NULL berarti opsi \* karena path yang di-passing hanya nama filenya saja). Jika name tidak NULL, string name akan ditambahkan di belakang string newfolder. Jika name NULL, string newfolder akan ditambahkan "/" dan string path (karena ini kondisi untuk \*, path adalah nama file). Selanjutnya file dipindahkan menggunakan fungsi rename() dengan parameter pertama adalah source path dan parameter kedua destination path.
~~~c
else{
    printf("masuk beda\n");
    if(mkdir(newfolder, 0777) < 0)
	printf("Error create folder\n");
    else{
	if(name)
	    strcat(newfolder, name);
	else
	    sprintf(newfolder, "%s/%s", newfolder, path);
	if(rename(path, newfolder) < 0)
	    printf("Error move file\n");
	else{
	    printf("berhasil hore\n");
	}
    }
}
~~~
Jika folder extensi belum ada, maka akan dibuat folder baru menggunakan fungsi mkdir(), selanjutnya melakukan pemindahan file sama seperti yang di atas.

- Jika extensi tidak diketahui
~~~c
char unk[100];
strcat(unk, "Unknown");
DIR* dir = opendir(unk);
if(dir){
    printf("udah ada\n");
    if(name)
	strcat(unk, name);
    else
	sprintf(unk, "%s/%s", unk, path);
    //printf("folder: %s\npath: %s\n", unk, path);
    if(rename(path, unk) < 0)
	printf("Error move file\n");
    else{
	printf("berhasil hore\n");
    }
}
~~~
Membuat string unk untuk nama folder baru, yang selanjutnya mengisi string tersebut dengan "Unknown". Membuka direktori unk untuk mengecek apakah direktori "Unknown" sudah ada atau belum. Jika sudah ada, menambahkan string name di belakang string unk (untuk opsi -f dan -d, sedangkan opsi \* masuk ke else) lalu memindah file dengan fungsi rename() dari path ke unk.
~~~c
else{
    printf("newfolder: %s", unk);
    if(mkdir(unk, 0777) < 0)
	printf("Error create folder\n");
    else{
	 if(name)
	    strcat(unk, name);
	else
	    sprintf(unk, "%s/%s", unk, path);
	if(rename(path, unk) < 0)
	    printf("Error move file\n");
	else{
	    printf("berhasil hore\n");
	}
    }
}
~~~
Jika belum ada, membuat folder baru "Unknown" dengan fungsi mkdir lalu memindahkan file dari path ke unk.

~~~c
int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
~~~
Merupakan fungsi tambahan untuk mengecek sebuah regular file atau bukan (direktori).

## Soal 4
### 4a 
1. Buatlah program C dengan nama "4a.c", yang berisi program untuk
melakukan perkalian matriks. Ukuran matriks pertama adalah 4x2, dan
matriks kedua 2x5. Isi dari matriks didefinisikan di dalam kodingan. Matriks
nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka).
2. Tampilkan matriks hasil perkalian tadi ke layar.
~~~c
#define M 4
#define K 2
#define N 5

int main(){
    key_t key = 1234;
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
~~~
Penjelasan:
- Mendefine ukuran ukuran matrix dengan variable yakni M, K, dan N dengan angka berturut-turut adalah 4, 2, 5.
- Seperti pada template, menginisialisasi key_t dengan suatu angka, di sini saya mengikuti template; 1234.
- Selanjutnya juga masih sama seperti yang di template: mendeklarasikan variabel pointer hasil yang nantinya akan dipass ke share memory, mendeklarasikan share memory id dengan shmget, dan mengattach variabel hasil tadi ke share memory dengan shmat.
- Deklarasi sekaligus inisialisasi matriks A dan matriks B dengan isi angka 1-20.
- Deklarasi matriks res dengan ukuran M * N yang nantinya akan dipakai untuk menyimpan hasil perkalian matriks.
- Menginisialisasi semua isi matriks res dengan 0
- Menghitung perkalian matriks seperti yang sudah pernah dipelajari di dasar pemrograman.
- Mencetak matriks res atau matriks hasil perkalian.
- Meng-assign isi dari variabel pointer hasil (dijadikan array satu dimensi) dengan isi dari matriks res.
- Sleep 5 detik untuk meng-share memory.

### 4b
1. Buatlah program C kedua dengan nama "4b.c". Program ini akan
mengambil variabel hasil perkalian matriks dari program "4a.c" (program
sebelumnya), dan tampilkan hasil matriks tersebut ke layar.
(Catatan!: gunakan shared memory)
2. Setelah ditampilkan, berikutnya untuk setiap angka dari matriks
tersebut, carilah nilai faktorialnya, dan tampilkan hasilnya ke layar dengan
format seperti matriks.
~~~c
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
~~~
Penjelasan:
- Mendefine ukuran matriks sama seperti 4a hanya saja cuma M dan N yaitu 4 dan 5
- Membuat fungsi void pointer jumlah dengan parameter void pointer yang nantinya akan menghitung penjumlahan tiap-tiap angka pada matriks hasil.
	- deklarasi variabel integer bil yang kemudian di-assign dengan variabel void pointer angka yang telah dipassing.
	- inisialisasi variabel hasil untuk menyimpan hasil dari penjumlahan bilangan
	- melakukan penjumlahan bilangan dengan iterasi dari 2 (karena hasil tadi sudah di-assign dengan 1) hingga bilangan itu sendiri.
	- deklarasi variabel integer pointer tambah untuk menyimpan hasil, yang selanjutnya akan dipakai sebagai return pthread_exit
- Dalam fungsi main program:
	- sama seperti 4a, yaitu melakukan template share memory.
	- menampilkan hasil dari perkalian matriks yang telah diambil dari share memory dalam bentuk array satu dimensi bernama hasil. ditampilkan dalam bentuk seperti matriks sehingga apabila index angkanya bisa habis dibagi 5 maka akan mengeprint newline.
	- mendeklarasikan thread berjumlah 20, membentuk thread menggunakan `pthread_create` dengan parameter thread ke-i, NULL, fungsi jumlah, dan angka ke-i dalam array hasil. 
	- melakukan pthread_join dengan k sebagai tempat menyimpan return dari pthread_exit.
	- meng-assign integer pointer p dengan k, lalu menampilkan p (hasil penjumlahan) dalam bentuk matriks.

### 4c
1. Buatlah program C ketiga dengan nama "4c.c". Program ini tidak
memiliki hubungan terhadap program yang lalu.
2. Pada program ini, Norland diminta mengetahui jumlah file dan
folder di direktori saat ini dengan command "ls | wc -l". Karena sudah belajar
IPC, Norland mengerjakannya dengan semangat.
(Catatan! : Harus menggunakan IPC Pipes)
~~~c
int pipes[2];

int main(){
    if (pipe(pipes) == -1)
        exit(1);

    if (fork() == 0){
        // output to pipes
        dup2(pipes[1], 1);

        // close reading end of pipe
        close(pipes[0]);

        // exec
        char *argv[] = {"ls", NULL};
        execv("/bin/ls", argv);
    }
    else{
        // input from pipes
        dup2(pipes[0], 0);

        // close writing end of pipe
        close(pipes[1]);

        char *argv[] = {"wc", "-l", NULL};
        execv("/usr/bin/wc", argv);
    }

}
~~~
Penjelasan:
- mendeklarasikan array pipes dengan size 2 (untuk 0 dan 1)
- melakukan pipe pada array pipes. `if (pipe(pipes) == -1) exit(1);` merupakan error handling apabila pipe gagal dibuat.
- melakukan fork, jika merupakan child process maka:
	- menggunakan fungsi dup2 untuk meng-copy suatu output (dalam hal ini 1 adalah stdout) ke dalam writing end dari pipes. setelah itu menutup reading end dari pipes.
	- melakukan `exec ls`
- dalam parent process:
	- menggunakan fungsi dup2 untuk me-repalce standard input (0) dengan input dari pipe. setelah itu menutup writing end dari pipes.
	- melakukan` exec wc -l`
