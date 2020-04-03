#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<strings.h>
#include<stdlib.h>

int i, err;
void* pindah(void*);
int is_regular_file(const char*);

int main(int argc, char* argv[]){
    int j;
    // for(j=0; j<argc; j++){
    //     printf("argumen %d: %s\n", j, argv[j]);
    // }
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
                    //printf("path: %s\n", path);
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
    //pthread_mutex_destroy(&lock);
}

void* pindah(void* ptr){
    char* path;
    char* ext;
    char* name;
    path = (char*)ptr;

    name = strrchr(path, '/');  //nama file tapi ada /-nya, eg: "/soal1.sh"
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
        
    }
    else{
        printf("masuk unknown\n");
        char unk[100];
        strcat(unk, "Unknown");
        DIR* dir = opendir(unk);
        if(dir){
            printf("udah ada\n");
            strcat(unk, name);
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
                strcat(unk, name);
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
