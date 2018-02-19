#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <stdbool.h>
#include <time.h>

bool write2 = false;
bool cakaj = true;
const int lenght = 128;


int number = 0;
int opr_pom;
int ime_Pomnilnika2 = 0;
char ime_pomnilnika[128];



void sig_handler(int sig)
{
    if(sig == SIGUSR1) {
        write2 = true;
    }
    if(sig == SIGINT){
        cakaj = false;
    }
}

int main(int argc, char* argv[])
{
    struct sigaction sa;
    memset(&sa, 0, sizeof (sa));
    sa.sa_handler = sig_handler;
    sigaction(SIGINT, &sa, 0);
    sigaction(SIGUSR1, &sa, 0);



    int c;
    extern char *optarg;
    extern int optind;
    extern int optopt;

    int arg = argc;
    int pomoc = 0;
    int stetje = 1;

    if(argc==1) {
        printf("Napaka, uporabi pomoc: -h\n");
        return 0;
    }
    for(;stetje <= argc;){
        while((c = getopt(arg, argv, ":hn:")) != -1){
            pomoc = 1;
            switch(c){
            case 'n':
                number = atoi(optarg);
                break;
            case ':':
                printf("Parameter -%c nima vrednosti!\n", optopt);
                return -1;
                break;
            case '?':
                printf("Neznano stikalo -%c !\n", optopt);
                return -1;
                break;
            }
        }
        if(pomoc == 1){
            arg = optind;
            pomoc = 0;
        }
        stetje++;

        if(arg <= argc){
            if(ime_Pomnilnika2 == 0){
                strcpy(ime_pomnilnika, argv[arg]);
                ime_Pomnilnika2 = 1;
            }
        }
        stetje++;
        arg++;
    }

    if(ime_Pomnilnika2 == 0){
        printf("error ime pom \n");
        return -1;
    }

    opr_pom = shm_open(ime_pomnilnika,O_RDWR|O_CREAT|O_EXCL,0660);
    if(opr_pom == -1){
        perror("error ustvarjanje pom \n");
        shm_unlink(ime_pomnilnika);
        return -1;
    }
    ftruncate(opr_pom,number*sizeof(int));
    close(opr_pom);

    for(;cakaj == true;){

        if(write2 == true){
            opr_pom = shm_open(ime_pomnilnika,O_RDWR,0);
            if(opr_pom == -1){
                perror("tezava pri odpiranju pomnilnika v starsu");
                shm_unlink(ime_pomnilnika);
                return -1;
            }

            char *data = (char*) mmap(0,number,PROT_READ|PROT_WRITE,MAP_SHARED,opr_pom,0);
            if(data==MAP_FAILED){
                perror("tezava pri preslikavi pomnilnika v otroku");
                return -1;
            }

            int l = 0;
            while(l < number){
                printf("%c",data[l]);
                l++;
            }

            munmap(data,number);
            write2 = false;
        }

    }

    close(opr_pom);
    shm_unlink(ime_pomnilnika);

    return 0;
}
